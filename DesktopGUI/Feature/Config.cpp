#include "Config.h"
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Timer.h"
#include "../../Utilities/Err.h"
#include "LogGUI.h"

//translation unit for static member
int Config::mUseSplash;
int Config::mSaveInterval;
int Config::mUseAutoSave;
int Config::mFontSize;
int Config::mFontID;
int Config::mLanguageID;
int Config::mZoomMin;
int Config::mZoomMax;
int Config::mZoomDefault;
int Config::mUseAutoHighlight;
int Config::mHighlightInterval;
int Config::mTextBack;
int Config::mTextFore;
int Config::mCaret;
int Config::mLineBack;
int Config::mSelection;
int Config::mLinenumBack;
int Config::mLinenumFore;
std::vector<sConfigReference> Config::mConfTemplate;

void Config::FetchData()
{
	TIMER_FUNCTION( timer, MS, false );

	if ( !Filestream::Is_Exist( CONFIG_FILEPATH ) )
	{
		LoadDefaultConfig();
		Config::SaveConfig();
		return;
	}
	if ( mConfTemplate.empty() ) MakeTemplate();
	
	try
	{
		auto vRead = Filestream::Read_Bin( CONFIG_FILEPATH );
		THROW_ERR_IFEMPTY( vRead, "Failed to read configuration file!" );
		
		std::string sRead = (const char*) &vRead[0];
		auto vBuffer = Filestream::ParseString( sRead, '\n' );
		THROW_ERR_IFEMPTY( vRead, "Failed to read configuration file!" );
		THROW_ERR_IF( vBuffer.size() != mConfTemplate.size(), "Config file is compromised!" );

		for ( int i = 0; i < mConfTemplate.size(); i++ )
		{
			auto vConfig = Filestream::ParseString( vBuffer[i], '=' );
			THROW_ERR_IF( vConfig[0] != mConfTemplate[i].Tag, "Config file is compromised!" );
			*mConfTemplate[i].RefData = std::stoi( vConfig[1] );
		}
		
		LOG_ALL( LEVEL_INFO, "Loading configuration file success!" );
	}
	catch ( Util::Err& e )
	{
		LOG_ALL( LEVEL_WARN, e.Seek().c_str() );
		LoadDefaultConfig();
		Config::SaveConfig();
	}
	catch ( ... )
	{
		LOG_ALL( LEVEL_WARN, "Unknown Exception found in FethConfiguration()!" );
		LoadDefaultConfig();
		Config::SaveConfig();
	}

	LOG_FUNCTION( LEVEL_INFO, TIMER_GETSTR( timer ) );
}

void Config::LoadDefaultConfig()
{
	mUseSplash = true;
	mLanguageID = 0;
	mSaveInterval = 3600;
	mUseAutoSave = true;
	mHighlightInterval = 100;
	mUseAutoHighlight = true;
	mFontID = 0;
	mFontSize = 10;
	mZoomMin = -3;
	mZoomMax = 15;
	mZoomDefault = 2;
	mTextBack = 2302755;
	mTextFore = 16777215;
	mCaret = 13405661;
	mLineBack = 4605510;
	mSelection = 13203258;
	mLinenumBack = 3945010;
	mLinenumFore = 13408221;
	LOG_ALL( LEVEL_INFO, "Loading default configuration file!" );
}

void Config::SaveConfig()
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
		LOG_ALL( LEVEL_WARN, e.Seek() );
	}
}

std::string Config::GetSupportedFormat()
{
	std::string supp;
	supp = "All types (*.*)|*.*";
	supp += "|Normal text file (*.txt)|*.txt";
	supp += "|Memoriser file (*.mtx)|*.mtx";
	supp += "|Memoriser dictionary (*.mdt)|*.mdt";
	supp += "|Memoriser archive (*.mac)|*.mac";
	return supp;
}

void Config::MakeTemplate()
{
	mConfTemplate.push_back( sConfigReference( "mSplashScreen",      &mUseSplash ) );
	mConfTemplate.push_back( sConfigReference( "mSystemLanguage",    &mLanguageID ) );
	mConfTemplate.push_back( sConfigReference( "mUseAutoSave",       &mUseAutoSave ) );
	mConfTemplate.push_back( sConfigReference( "mSaveInterval",      &mSaveInterval ) );
	mConfTemplate.push_back( sConfigReference( "mUseAutoHighlight",  &mUseAutoHighlight ) );
	mConfTemplate.push_back( sConfigReference( "mHighlightInterval", &mHighlightInterval ) );
	mConfTemplate.push_back( sConfigReference( "mFontID",            &mFontID ) );
	mConfTemplate.push_back( sConfigReference( "mFontSize",          &mFontSize ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMin",           &mZoomMin ) );
	mConfTemplate.push_back( sConfigReference( "mZoomMax",           &mZoomMax ) );
	mConfTemplate.push_back( sConfigReference( "mZoomDefault",       &mZoomDefault ) );
	mConfTemplate.push_back( sConfigReference( "mTextBack",          &mTextBack ) );
	mConfTemplate.push_back( sConfigReference( "mTextFore",          &mTextFore ) );
	mConfTemplate.push_back( sConfigReference( "mCaret",             &mCaret ) );
	mConfTemplate.push_back( sConfigReference( "mLineBack",          &mLineBack ) );
	mConfTemplate.push_back( sConfigReference( "mSelection",         &mSelection ) );
	mConfTemplate.push_back( sConfigReference( "mLinenumBack",       &mLinenumBack ) );
	mConfTemplate.push_back( sConfigReference( "mLinenumFore",       &mLinenumFore ) );
}