#include "Config.h"
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Err.h"
#include "LogGUI.h"

//translation unit for static member
int Config::mUseSplash;
int Config::mAutosaveInterval;
int Config::mUseAutosave;
int Config::mFontSize;
int Config::mFontID;
int Config::mLanguageID;
int Config::mZoomMin;
int Config::mZoomMax;
int Config::mZoomDefault;
std::vector<sConfigReference> Config::mConfTemplate;

void Config::FetchConfiguration()
{
	if ( !Filestream::Is_Exist( CONFIG_FILEPATH ) )
	{
		LoadDefaultConfiguration();
		Config::SaveConfiguration();
		return;
	}
	if ( mConfTemplate.empty() ) MakeTemplate();
	
	try
	{
		auto vRead = Filestream::Read_Bin( CONFIG_FILEPATH );
		THROW_ERR_IF( vRead.empty(), "Failed to read configuration file!" );
		
		std::string sRead = (const char*) &vRead[0];
		auto vBuffer = Filestream::ParseString( sRead, '\n' );

		for ( int i = 0; i < mConfTemplate.size(); i++ )
		{
			auto vConfig = Filestream::ParseString( vBuffer[i], '=' );
			THROW_ERR_IF( vConfig[0] != mConfTemplate[i].Tag, "Config file is compromised!" );
			*mConfTemplate[i].RefData = std::stoi( vConfig[1] );
		}
		
		LOGALL( LEVEL_INFO, "Loading configuration file success!" );
	}
	catch ( Util::Err& e )
	{
		LOGALL( LEVEL_WARN, e.Seek() );
		LoadDefaultConfiguration();
		Config::SaveConfiguration();
	}
	catch ( ... )
	{
		LOGALL( LEVEL_WARN, "Unknown Exception found in FethConfiguration()!" );
		LoadDefaultConfiguration();
		Config::SaveConfiguration();
	}
}

void Config::LoadDefaultConfiguration()
{
	mUseSplash = true;
	mLanguageID = 0;
	mAutosaveInterval = 30;
	mUseAutosave = true;
	mFontID = 0;
	mFontSize = 10;
	mZoomMin = -3;
	mZoomMax = 15;
	mZoomDefault = 2;
	LOGALL( LEVEL_INFO, "Loading default configuration file!" );
}

void Config::SaveConfiguration()
{
	if ( mConfTemplate.empty() ) MakeTemplate();

	try
	{
		std::string ConfigText;
		for ( auto t : mConfTemplate )
			ConfigText += t.Tag + '=' + std::to_string( *t.RefData ) + '\n';
		ConfigText.pop_back();
	
		Filestream::Write_Bin( ConfigText.c_str(), ConfigText.size(), CONFIG_FILEPATH );
		THROW_ERR_IFNOT( Filestream::Is_Exist( CONFIG_FILEPATH ), "Cannot save configuration file!" );
	}
	catch ( Util::Err& e )
	{
		LOGALL( LEVEL_WARN, e.Seek() );
	}
}

void Config::MakeTemplate()
{
	mConfTemplate.push_back( sConfigReference( "mSplashScreen",      &mUseSplash ) );
	mConfTemplate.push_back( sConfigReference( "mSystemLanguage",    &mLanguageID ) );
	mConfTemplate.push_back( sConfigReference( "mAutosaveInterval",  &mAutosaveInterval ) );
	mConfTemplate.push_back( sConfigReference( "mUseAutosave",       &mUseAutosave ) );
	mConfTemplate.push_back( sConfigReference( "mFontID",            &mFontID ) );
	mConfTemplate.push_back( sConfigReference( "mFontSize",          &mFontSize ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMin",           &mZoomMin ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMax",           &mZoomMax ) );
	mConfTemplate.push_back( sConfigReference( "mZoomDefault",       &mZoomDefault ) );
}