#pragma once
#include <string>
#include <vector>

#define CONFIG_FILEPATH "config.conf"

struct sConfigReference
{
	sConfigReference( std::string tag, uint32_t* ref ) : Tag(tag), RefData(ref) {}
	std::string Tag;
	uint32_t* RefData;
};


class Config
{
public:
	static void FetchConfiguration();
	static void LoadDefaultConfiguration();
	static void SaveConfiguration();

	static uint32_t mUseSplash;
	static uint32_t mFontSize;
	static uint32_t mFontID;
	static uint32_t mLanguageID;
	static uint32_t mZoomMin;
	static uint32_t mZoomMax;
	static uint32_t mZoomDefault;

private:
	static void MakeTemplate();

	static std::vector<sConfigReference> mConfTemplate;
};