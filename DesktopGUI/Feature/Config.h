#pragma once

#define MAJOR_VERSION     "1"
#define CHILD_VERSION     "0"
#define DATE_UPDATED      "05-December-2020"
#define APP_NAME          "Mémoriser"
#define DEBUG_NAME        "Mémoriser - Debug Console"
#define DICT_NAME         "Mémoriser - Dictionary"
#define CONFIG_FILE       "config.cfg"

#define TYPE_BOOL 1
#define TYPE_INT 4
#define TYPE_STR 0

#define DICT_ALL_DOCS 0
#define DICT_OPN_DOCS 1
#define DICT_TMP_DOCS 2

#define TEMP_APPLY_ALL 0
#define TEMP_APPLY_NEW 1

struct sConfigReference
{
	sConfigReference( uint8_t type, void* ref, const std::string& tag )
		: Type( type ), RefData( ref ), Tag( tag ) {}
	uint8_t Type;
	void* RefData;
	std::string Tag;
};

class Config
{
	struct Notebook
	{
		int Hide;
		int Orientation;
		int LockMove;
		int FixedWidth;
		int MiddleClose;
		int ShowCloseBtn;
		int CloseBtnOn;
	};

	struct Dictionary
	{
		int UseGlobal;
		int ApplyOn;
		int MatchCase;
		int MatchWhole;
		int UniformClr;
		std::string Directory;
	};

	struct Temporary
	{
		int UseTemp;
		int ApplyOn;
		std::string Directory;
	};

	struct Font
	{
		std::string Face;
		int Size;
		int Weight;
		int Family;
		int Style;
	};

	struct General
	{
		int UseSplash;
		int LanguageID;
		int ZoomDefault;
		int UseStatbar;
		int UseDragDrop;
		const int CheckConnectionInterval = 100;
	};

	struct AutoThread
	{
		int Use;
		int Param;
	};

	struct Style
	{
		// decimal value of BGR colour (NOT RGB)
		int TextBack;
		int TextFore;
		int Caret;
		int LineBack;
		int Selection;
		int LinenumBack;
		int LinenumFore;
	};

public:
	static void Init();
	static void FetchData();
	static void LoadDefaultConfig();
	static void SaveConfig();
	static std::string GetSupportedFormat();

	static int GetDictionaryFlags();
	static int GetNotebookStyle();
	static void SetNotebookStyle( int style );
	static wxFont BuildFont();
	static void SetFont( wxFont font );

	static wxString sAppPath;
	static Font sFont;
	static Style sStyle;
	static General sGeneral;
	static AutoThread sAutosave;
	static AutoThread sAutohigh;
	static AutoThread sAutocomp;
	static Notebook sNotebook;
	static Dictionary sDictionary;
	static Temporary sTemp;

private:
	static void MakeTemplate();
	static wxString mConfigFilepath;

	static std::vector<sConfigReference> mTemplate;
};

#define MIN_AUTOCOMP_WORDS     100
#define MAX_AUTOCOMP_WORDS     1000
#define MIN_AUTOSAVE_INTERVAL  1
#define MAX_AUTOSAVE_INTERVAL  60
#define MIN_AUTOHIGH_INTERVAL  50
#define MAX_AUTOHIGH_INTERVAL  500