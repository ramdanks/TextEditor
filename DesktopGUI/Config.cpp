#include "Config.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Err.h"
#include "LogGUI.h"

//translation unit for static member
uint32_t                          Config::mUseSplash;
uint32_t                          Config::mFontSize;
uint32_t                          Config::mFontID;
uint32_t                          Config::mLanguageID;
uint32_t                          Config::mZoomMin;
uint32_t                          Config::mZoomMax;
uint32_t                          Config::mZoomDefault;
std::vector<sConfigReference>     Config::mConfTemplate;

void Config::FetchConfiguration()
{
	if ( !Filestream::Is_Exist( CONFIG_FILEPATH ) )
	{
		LoadDefaultConfiguration();
		return;
	}
	if ( mConfTemplate.empty() ) MakeTemplate();
	
	try
	{
		auto vRead = Filestream::Read_Bin( CONFIG_FILEPATH );
		THROW_ERR_IF( vRead.empty(), "Failed to read configuration file!" );
		
		std::string sRead = (const char*) &vRead[0];
		auto vBuffer = Filestream::ParseString( sRead, '\n' );

		for ( uint32_t i = 0; i < mConfTemplate.size(); i++ )
		{
			auto vConfig = Filestream::ParseString( vBuffer[i], '=' );
			THROW_ERR_IF( vConfig[0] != mConfTemplate[i].Tag, "Config file is compromised!" );
			*mConfTemplate[i].RefData = std::stoi( vConfig[1] );
		}
		
		LOGALL( LEVEL_INFO, "Loading configuration file success!", LOG_FILEPATH );
	}
	catch ( Util::Err& e )
	{
		LOGALL( LEVEL_WARN, e.Seek(), LOG_FILEPATH );
		LoadDefaultConfiguration();
	}
	catch ( ... )
	{
		LOGALL( LEVEL_WARN, "Unhandled Exception found in FethConfiguration()!", LOG_FILEPATH );
		LoadDefaultConfiguration();
	}
}

void Config::LoadDefaultConfiguration()
{
	mUseSplash = 1;
	mLanguageID = 0;
	mFontID = 0;
	mFontSize = 10;
	mZoomMin = 5;
	mZoomMax = 20;
	mZoomDefault = 10;
	LOGALL( LEVEL_INFO, "Loading default configuration file!", LOG_FILEPATH );
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
	
		Filestream::Write_Bin( ConfigText.c_str(), ConfigText.size() + 1, CONFIG_FILEPATH );
		THROW_ERR_IFNOT( Filestream::Is_Exist( CONFIG_FILEPATH ), "Cannot save configuration file!" );
	}
	catch ( Util::Err& e )
	{
		LOGALL( LEVEL_WARN, e.Seek(), LOG_FILEPATH );
	}
}

void Config::MakeTemplate()
{
	mConfTemplate.push_back( sConfigReference( "mSplashScreen",      &mUseSplash ) );
	mConfTemplate.push_back( sConfigReference( "mSystemLanguage",    &mLanguageID ) );
	mConfTemplate.push_back( sConfigReference( "mFontID",            &mFontID ) );
	mConfTemplate.push_back( sConfigReference( "mFontSize",          &mFontSize ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMin",           &mZoomMin ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMax",           &mZoomMax ) );
	mConfTemplate.push_back( sConfigReference( "mZoomDefault",       &mZoomDefault ) );
}