#pragma once

#define MAJOR_VERSION     "1"
#define CHILD_VERSION     "0"
#define DATE_UPDATED      "02-December-2020"
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
public:
	static void FetchData();
	static void LoadDefaultConfig();
	static void SaveConfig();
	static std::string GetSupportedFormat();
	static void SetFont( wxFont font );


	static wxFont mFont;
	static int mFontSize;
	static int mFontWeight;
	static int mFontFamily;
	static int mFontStyle;

	static int mUseSplash;
	static int mLanguageID;
	static int mZoomDefault;
	static int mSaveInterval;
	static int mUseAutoSave;
	static int mUseAutoHighlight;
	static int mHighlightInterval;
	static const int mCheckConnectionInterval = 100;

	// decimal value of BGR colour (NOT RGB)
	static int mTextBack;
	static int mTextFore;
	static int mCaret;
	static int mLineBack;
	static int mSelection;
	static int mLinenumBack;
	static int mLinenumFore;

private:
	static void MakeTemplate();

	static std::vector<sConfigReference> mConfTemplate;
};