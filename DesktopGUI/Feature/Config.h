#pragma once

#define MAJOR_VERSION     "1"
#define CHILD_VERSION     "0"
#define DATE_UPDATED      "05-December-2020"
#define APP_NAME          "Mémoriser"
#define DEBUG_NAME        "Mémoriser - Debug Console"
#define DICT_NAME         "Mémoriser - Dictionary"
#define CONFIG_FILEPATH   "config.cfg"

struct sConfigReference
{
	sConfigReference( std::string tag, int* ref ) : Tag(tag), RefData(ref) {}
	std::string Tag;
	int* RefData;
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

	struct Font
	{
		wxString Face;
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
	static void FetchData();
	static void LoadDefaultConfig();
	static void SaveConfig();
	static std::string GetSupportedFormat();

	static int GetNotebookStyle();
	static void SetNotebookStyle( int style );
	static wxFont BuildFont();
	static void SetFont( wxFont font );

	static Font mFont;
	static Style mStyle;
	static General mGeneral;
	static AutoThread mAutosave;
	static AutoThread mAutohigh;
	static AutoThread mAutocomp;
	static Notebook mNotebook;

private:
	static void MakeTemplate();

	static std::vector<sConfigReference> mConfTemplate;
};

#define MIN_AUTOCOMP_WORDS     1000
#define MAX_AUTOCOMP_WORDS     10000
#define MIN_AUTOSAVE_INTERVAL  1
#define MAX_AUTOSAVE_INTERVAL  60
#define MIN_AUTOHIGH_INTERVAL  50
#define MAX_AUTOHIGH_INTERVAL  500