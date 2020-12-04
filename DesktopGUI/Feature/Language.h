#pragma once

enum LanguageID
{
	ENGLISH    = 0,
	BAHASA     = 1,
	SPANISH    = 2,
	PORTUGUESE = 3,
	GERMAN     = 4,
	ITALIAN    = 5,
	FRENCH     = 6,
	DUTCH      = 7,
};

// singleton pattern
class Language
{
public:
	Language() = delete;
	Language( const Language& other ) = delete;

	static bool LoadMessage( LanguageID id );
	static wxString GetMessage( size_t index );

private:
	static void LoadMessageDefault();
	static wxString IdentifyID( LanguageID id );

	static std::vector<wxString> mMessage;
};

#define LANG_ENGLISH_FILEPATH      "resource/lang/english.txt"
#define LANG_BAHASA_FILEPATH       "resource/lang/bahasa.txt"
#define LANG_SPANISH_FILEPATH      "resource/lang/spanish.txt"
#define LANG_PORTUGUESE_FILEPATH   "resource/lang/portuguese.txt"
#define LANG_GERMAN_FILEPATH       "resource/lang/german.txt"
#define LANG_ITALIAN_FILEPATH      "resource/lang/italian.txt"
#define LANG_FRENCH_FILEPATH       "resource/lang/french.txt"
#define LANG_DUTCH_FILEPATH        "resource/lang/dutch.txt"

#define EMPTY_IDENTIFIER        "<NULL>"
#define MSG_LANG                Language::GetMessage(0)
#define MSG_STATUSBAR           Language::GetMessage(1)
#define MSG_DEBUGCONSOLE        Language::GetMessage(2)
#define MSG_FILE                Language::GetMessage(3)
#define MSG_NEW                 Language::GetMessage(4)
#define MSG_SAVE                Language::GetMessage(5)
#define MSG_SAVEAS              Language::GetMessage(6)
#define MSG_SAVEALL             Language::GetMessage(7)
#define MSG_OPEN                Language::GetMessage(8)
#define MSG_RENAME              Language::GetMessage(9)
#define MSG_CLOSE               Language::GetMessage(10)
#define MSG_CLOSEALL            Language::GetMessage(11)
#define MSG_NEWDICT             Language::GetMessage(12)
#define MSG_OPENDICT            Language::GetMessage(13)
#define MSG_EXIT                Language::GetMessage(14)
#define MSG_EDIT                Language::GetMessage(15)
#define MSG_UNDO                Language::GetMessage(16)
#define MSG_REDO                Language::GetMessage(17)
#define MSG_CUT                 Language::GetMessage(18)
#define MSG_COPY                Language::GetMessage(19)
#define MSG_PASTE               Language::GetMessage(20)
#define MSG_DELETE              Language::GetMessage(21)
#define MSG_SELECTALL           Language::GetMessage(22)
#define MSG_SEARCH              Language::GetMessage(23)
#define MSG_FIND                Language::GetMessage(24)
#define MSG_SELECTFNEXT         Language::GetMessage(25)
#define MSG_SELECTFPREV         Language::GetMessage(26)
#define MSG_REPLACE             Language::GetMessage(27)
#define MSG_GOTO                Language::GetMessage(28)
#define MSG_VIEW                Language::GetMessage(29)
#define MSG_AOT                 Language::GetMessage(30)
#define MSG_ZOOMIN              Language::GetMessage(31)
#define MSG_ZOOMOUT             Language::GetMessage(32)
#define MSG_ZOOMRESTORE         Language::GetMessage(33)
#define MSG_TEXTSUM             Language::GetMessage(34)
#define MSG_COMPRESSIONSUM      Language::GetMessage(35)
#define MSG_DEBUGCONSOLE        Language::GetMessage(36)
#define MSG_SETTINGS            Language::GetMessage(37)
#define MSG_PREFERENCES         Language::GetMessage(38)
#define MSG_STYLECONFIG         Language::GetMessage(39)
#define MSG_HELP                Language::GetMessage(40)
#define MSG_REPORTBUG           Language::GetMessage(41)
#define MSG_OPENLOGDIR          Language::GetMessage(42)
#define MSG_SEEDOC              Language::GetMessage(43)
#define MSG_ABOUT               Language::GetMessage(44)
#define MSG_HIGHLIGHTING        "Text Highlighting"
#define MSG_REFRESH             "Refresh"
#define MSG_SHARE               "Share"
#define MSG_UPPCASE             "Uppercase"
#define MSG_LOWCASE             "Lowercase"
#define MSG_INVCASE             "Inverse-case"
#define MSG_RANCASE             "Random-case"
#define MSG_EOLUNX              "Unix (LF)"
#define MSG_EOLMAC              "Macintosh (CR)"
#define MSG_EOLWIN              "Windows (CR-LF)"
#define MSG_CASECONV            "Case Conversion"
#define MSG_EOLCONV             "EOL Conversion"
