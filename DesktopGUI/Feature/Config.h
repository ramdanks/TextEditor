#pragma once
#include <string>
#include <vector>

#define MAJOR_VERSION     "0"
#define CHILD_VERSION     "8"
#define APP_NAME          "M�moriser"
#define DEBUG_NAME        "M�moriser - Debug Console"
#define DICT_NAME         "M�moriser - Dictionary"
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
	static std::string LoadSupportedFormat();

	static int mUseSplash;
	static int mFontSize;
	static int mFontID;
	static int mLanguageID;
	static int mZoomMin;
	static int mZoomMax;
	static int mZoomDefault;
	static int mSaveInterval;
	static int mUseAutoSave;
	static int mUseAutoHighlight;
	static int mHighlightInterval;

private:
	static void MakeTemplate();

	static std::vector<sConfigReference> mConfTemplate;
};