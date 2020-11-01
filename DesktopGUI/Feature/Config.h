#pragma once
#include <string>
#include <vector>

#define MAJOR_VERSION     "0"
#define CHILD_VERSION     "5"
#define APP_NAME          "Mémoriser"
#define CONFIG_FILEPATH   "config.conf"

struct sConfigReference
{
	sConfigReference( std::string tag, int* ref ) : Tag(tag), RefData(ref) {}
	std::string Tag;
	int* RefData;
};

class Config
{
public:
	static void FetchConfiguration();
	static void LoadDefaultConfiguration();
	static void SaveConfiguration();

	static int mUseSplash;
	static int mFontSize;
	static int mFontID;
	static int mLanguageID;
	static int mZoomMin;
	static int mZoomMax;
	static int mZoomDefault;
	static int mAutosaveInterval;
	static int mUseAutosave;

private:
	static void MakeTemplate();

	static std::vector<sConfigReference> mConfTemplate;
};