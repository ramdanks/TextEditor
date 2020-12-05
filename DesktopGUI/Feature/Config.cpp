#include "Config.h"
#include "LogGUI.h"

//translation unit for static member
Config::Font Config::mFont;
Config::Style Config::mStyle;
Config::General Config::mGeneral;
Config::AutoThread Config::mAutosave;
Config::AutoThread Config::mAutohigh;
Config::AutoThread Config::mAutocomp;
Config::Notebook Config::mNotebook;
std::vector<sConfigReference> Config::mConfTemplate;

void Config::FetchData()
{
	PROFILE_FUNC();

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
		LOG_ALL( LV_INFO, "Loading configuration file success!" );
	}
	catch ( Util::Err& e )
	{
		LOG_ALL( LV_WARN, e.Seek().c_str() );
		LoadDefaultConfig();
		Config::SaveConfig();
	}
	catch ( ... )
	{
		LOG_ALL( LV_WARN, "Unknown Exception found in FethConfiguration()!" );
		LoadDefaultConfig();
		Config::SaveConfig();
	}
}

void Config::LoadDefaultConfig()
{
	// system default
	mGeneral.UseSplash     = true;
	mGeneral.LanguageID    = 0;
	mGeneral.ZoomDefault   = 5;
	mGeneral.UseStatbar    = true;
	mGeneral.UseDragDrop   = true;
	// autosave default
	mAutosave.Use          = true;
	mAutosave.Param        = 5000;
	// autohighlight default
	mAutohigh.Use          = true;
	mAutohigh.Param        = 5000;
	// autocompletion default
	mAutocomp.Use          = true;
	mAutocomp.Param        = 5000;
	// font default	       
	mFont.Size             = 10;
	mFont.Family           = wxFONTFAMILY_MODERN;
	mFont.Style            = wxFONTSTYLE_NORMAL;
	mFont.Weight           = wxFONTWEIGHT_NORMAL;
	// style default       
	mStyle.TextBack        = 2302755;
	mStyle.TextFore        = 16777215;
	mStyle.Caret           = 13405661;
	mStyle.LineBack        = 4605510;
	mStyle.Selection       = 13203258;
	mStyle.LinenumBack     = 3945010;
	mStyle.LinenumFore     = 13408221;

	LOG_ALL( LV_INFO, "Loading default configuration file!" );
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
		LOG_ALL( LV_WARN, e.Seek() );
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

int Config::GetNotebookStyle()
{
	int style = 0;
	if ( Config::mNotebook.Orientation )   style |= wxAUI_NB_TOP;
	else                                   style |= wxAUI_NB_BOTTOM;
	if ( Config::mNotebook.FixedWidth )    style |= wxAUI_NB_TAB_FIXED_WIDTH;
	if ( !Config::mNotebook.LockMove )     style |= wxAUI_NB_TAB_MOVE;
	if ( Config::mNotebook.MiddleClose )   style |= wxAUI_NB_MIDDLE_CLICK_CLOSE;
	if ( Config::mNotebook.ShowCloseBtn )
	{
		if ( Config::mNotebook.CloseBtnOn )    style |= wxAUI_NB_CLOSE_ON_ALL_TABS;
		else                                   style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
	}
	return style;
}

void Config::SetNotebookStyle( int style )
{
	Config::mNotebook.Orientation  = style & wxAUI_NB_TOP;
	Config::mNotebook.FixedWidth   = style & wxAUI_NB_BOTTOM;
	Config::mNotebook.LockMove     = style & wxAUI_NB_TAB_FIXED_WIDTH;
	Config::mNotebook.MiddleClose  = style & wxAUI_NB_TAB_MOVE;
	Config::mNotebook.ShowCloseBtn = style & wxAUI_NB_MIDDLE_CLICK_CLOSE;
	Config::mNotebook.CloseBtnOn   = style & wxAUI_NB_CLOSE_ON_ALL_TABS;
}

wxFont Config::BuildFont()
{
	return wxFont( mFont.Size, (wxFontFamily) mFont.Family, (wxFontStyle) mFont.Style,
				   (wxFontWeight) mFont.Weight, false, mFont.Face );
}

void Config::SetFont( wxFont font )
{
	mFont.Face   = font.GetFaceName();
	mFont.Size   = font.GetPointSize();
	mFont.Style  = font.GetStyle();
	mFont.Family = font.GetFamily();
	mFont.Weight = font.GetWeight();
}

void Config::MakeTemplate()
{
	mConfTemplate.reserve( 30 );
	mConfTemplate.emplace_back( "Hide",              &mNotebook.Hide );
	mConfTemplate.emplace_back( "Orientation",       &mNotebook.Orientation );
	mConfTemplate.emplace_back( "LockMove",          &mNotebook.LockMove );
	mConfTemplate.emplace_back( "FixedWidth",        &mNotebook.FixedWidth );
	mConfTemplate.emplace_back( "MiddleMouseClose",  &mNotebook.MiddleClose );
	mConfTemplate.emplace_back( "ShowCloseBtn",      &mNotebook.ShowCloseBtn );
	mConfTemplate.emplace_back( "CloseBtnOn",        &mNotebook.CloseBtnOn );
	mConfTemplate.emplace_back( "SplashScreen",      &mGeneral.UseSplash );
	mConfTemplate.emplace_back( "UseStatusbar",      &mGeneral.UseStatbar );
	mConfTemplate.emplace_back( "UseDragDrop",       &mGeneral.UseDragDrop );
	mConfTemplate.emplace_back( "SystemLanguage",    &mGeneral.LanguageID );
	mConfTemplate.emplace_back( "ZoomDefault",       &mGeneral.ZoomDefault );
	mConfTemplate.emplace_back( "UseAutosave",       &mAutosave.Use );
	mConfTemplate.emplace_back( "AutosaveInterval",  &mAutosave.Param );
	mConfTemplate.emplace_back( "UseAutohigh",       &mAutohigh.Use );
	mConfTemplate.emplace_back( "AutohighInterval",  &mAutohigh.Param );
	mConfTemplate.emplace_back( "UseAutocomp",       &mAutocomp.Use );
	mConfTemplate.emplace_back( "AutocompWords",     &mAutocomp.Param );
	mConfTemplate.emplace_back( "TextBack",          &mStyle.TextBack );
	mConfTemplate.emplace_back( "TextFore",          &mStyle.TextFore );
	mConfTemplate.emplace_back( "Caret",             &mStyle.Caret );
	mConfTemplate.emplace_back( "LineBack",          &mStyle.LineBack );
	mConfTemplate.emplace_back( "Selection",         &mStyle.Selection );
	mConfTemplate.emplace_back( "LinenumBack",       &mStyle.LinenumBack );
	mConfTemplate.emplace_back( "LinenumFore",       &mStyle.LinenumFore );
	mConfTemplate.emplace_back( "FontSize",          &mFont.Size );
	mConfTemplate.emplace_back( "FontStyle",         &mFont.Style );
	mConfTemplate.emplace_back( "FontFamily",        &mFont.Family );
	mConfTemplate.emplace_back( "FontWeight",        &mFont.Weight );
}