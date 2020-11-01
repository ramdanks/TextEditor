#pragma once
#include <vector>
#include <string>

#define LANG_ENGLISH_FILEPATH      "resource/lang/english.txt"
#define LANG_BAHASA_FILEPATH       "resource/lang/bahasa.txt"
#define LANG_SPANISH_FILEPATH      "resource/lang/spanish.txt"
#define LANG_PORTUGUESE_FILEPATH   "resource/lang/portuguese.txt"
#define LANG_GERMAN_FILEPATH       "resource/lang/german.txt"
#define LANG_ITALIAN_FILEPATH      "resource/lang/italian.txt"
#define LANG_FRENCH_FILEPATH       "resource/lang/french.txt"
#define LANG_DUTCH_FILEPATH        "resource/lang/dutch.txt"

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

class Language
{
public:
	static bool LoadMessage( LanguageID id );

	static std::string whatLanguage();
	static std::string getMessage( size_t index );

private:
	static void LoadMessageDefault();
	static std::string IdentifyID( LanguageID id );
	static std::string mTitle;
	static std::vector<std::string> mMessage;
};

#define EMPTY_IDENTIFIER        "<NULL>"
#define MSG_LANG                Language::getMessage(0)
#define MSG_STATUSBAR           Language::getMessage(1)
#define MSG_DEBUGCONSOLE        Language::getMessage(2)
#define MSG_FILE                Language::getMessage(3)
#define MSG_NEW                 Language::getMessage(4)
#define MSG_SAVE                Language::getMessage(5)
#define MSG_SAVEAS              Language::getMessage(6)
#define MSG_SAVEALL             Language::getMessage(7)
#define MSG_OPEN                Language::getMessage(8)
#define MSG_RENAME              Language::getMessage(9)
#define MSG_CLOSE               Language::getMessage(10)
#define MSG_CLOSEALL            Language::getMessage(11)
#define MSG_NEWDICT             Language::getMessage(12)
#define MSG_OPENDICT            Language::getMessage(13)
#define MSG_EXIT                Language::getMessage(14)
#define MSG_EDIT                Language::getMessage(15)
#define MSG_UNDO                Language::getMessage(16)
#define MSG_REDO                Language::getMessage(17)
#define MSG_CUT                 Language::getMessage(18)
#define MSG_COPY                Language::getMessage(19)
#define MSG_PASTE               Language::getMessage(20)
#define MSG_DELETE              Language::getMessage(21)
#define MSG_SELECTALL           Language::getMessage(22)
#define MSG_SEARCH              Language::getMessage(23)
#define MSG_FIND                Language::getMessage(24)
#define MSG_SELECTFNEXT         Language::getMessage(25)
#define MSG_SELECTFPREV         Language::getMessage(26)
#define MSG_REPLACE             Language::getMessage(27)
#define MSG_GOTO                Language::getMessage(28)
#define MSG_VIEW                Language::getMessage(29)
#define MSG_AOT                 Language::getMessage(30)
#define MSG_ZOOMIN              Language::getMessage(31)
#define MSG_ZOOMOUT             Language::getMessage(32)
#define MSG_ZOOMRESTORE         Language::getMessage(33)
#define MSG_TEXTSUM             Language::getMessage(34)
#define MSG_COMPRESSIONSUM      Language::getMessage(35)
#define MSG_DEBUGCONSOLE        Language::getMessage(36)
#define MSG_SETTINGS            Language::getMessage(37)
#define MSG_PREFERENCES         Language::getMessage(38)
#define MSG_STYLECONFIG         Language::getMessage(39)
#define MSG_HELP                Language::getMessage(40)
#define MSG_REPORTBUG           Language::getMessage(41)
#define MSG_OPENLOGDIR          Language::getMessage(42)
#define MSG_SEEDOC              Language::getMessage(43)
#define MSG_ABOUT               Language::getMessage(44)