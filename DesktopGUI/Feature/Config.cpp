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
Config::Dictionary Config::mDictionary;
Config::Temporary Config::mTemp;
std::vector<sConfigReference> Config::mTemplate;

void Config::FetchData()
{
	PROFILE_FUNC();

	Filestream::Create_Directories( "temp" );
	if ( !Filestream::Is_Exist( CONFIG_FILEPATH ) )
	{
		LoadDefaultConfig();
		Config::SaveConfig();
		return;
	}
	if ( mTemplate.empty() ) MakeTemplate();

	try
	{
		auto vRead = Filestream::Read_Bin( CONFIG_FILEPATH );
		THROW_ERR_IFEMPTY( vRead, "Failed to read configuration file!" );
		
		std::string sRead = (const char*) &vRead[0];
		auto vBuffer = Filestream::ParseString( sRead, '\n' );
		THROW_ERR_IFEMPTY( vRead, "Failed to read configuration file!" );
		THROW_ERR_IF( vBuffer.size() != mTemplate.size(), "Config file is not compatible!" );

		for ( int i = 0; i < mTemplate.size(); i++ )
		{
			auto vConfig = Filestream::ParseString( vBuffer[i], '=' );
			THROW_ERR_IF( vConfig[0] != mTemplate[i].Tag, "Config file is not compatible!" );
			THROW_ERR_IF( vConfig.size() != 2, "Config file doesnt expect empty value!" );

			if ( mTemplate[i].Type == TYPE_INT )
				*(int*) mTemplate[i].RefData = std::stoi( vConfig[1] );
			else if ( mTemplate[i].Type == TYPE_STR )
				*(std::string*) mTemplate[i].RefData = vConfig[1];
		}
		LOG_ALL( LV_INFO, "Loading configuration file success!" );
	}
	catch ( Util::Err& e )
	{
		LOG_ALL( LV_WARN, e.Seek() );
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
	// notebook default
	mNotebook.Hide         = false;
	mNotebook.Orientation  = true;
	mNotebook.LockMove     = false;
	mNotebook.FixedWidth   = false;
	mNotebook.MiddleClose  = true;
	mNotebook.ShowCloseBtn = true;
	mNotebook.CloseBtnOn   = false;
	// dictionary defaukt
	mDictionary.UseGlobal  = true;
	mDictionary.ApplyOn    = DICT_ALL_DOCS;
	mDictionary.Directory  = "dictionary";
	mDictionary.UniformClr = false;
	mDictionary.MatchCase  = false;
	mDictionary.MatchWhole = true;
	// temporary default
	mTemp.UseTemp          = true;
	mTemp.ApplyOn          = TEMP_APPLY_NEW;
	mTemp.Directory        = "temp";
	// autosave default
	mAutosave.Use          = true;
	mAutosave.Param        = 60000;
	// autohighlight default
	mAutohigh.Use          = true;
	mAutohigh.Param        = 500;
	// autocompletion default
	mAutocomp.Use          = true;
	mAutocomp.Param        = 100;
	// font default	       
	mFont.Face             = "Calibri";
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
	if ( mTemplate.empty() ) MakeTemplate();

	try
	{
		std::string ConfigText;
		for ( auto t : mTemplate )
		{
			if ( t.Type == TYPE_INT ) 
				ConfigText += t.Tag + '=' + std::to_string( *(int*)t.RefData ) + '\n';
			else if ( t.Type == TYPE_STR )
				ConfigText += t.Tag + '=' + *(std::string*) t.RefData + '\n';
		}
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
	return supp;
}

int Config::GetDictionaryFlags()
{
	int flags = 0;
	if ( Config::mDictionary.MatchWhole ) flags |= wxSTC_FIND_WHOLEWORD;
	if ( Config::mDictionary.MatchCase )  flags |= wxSTC_FIND_MATCHCASE;
	return flags;
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
	mTemplate.reserve( 30 );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.Hide,         "Hide"             );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.Orientation,  "Orientation"      );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.LockMove,     "LockMove"         );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.FixedWidth,   "FixedWidth"       );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.MiddleClose,  "MiddleMouseClose" );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.ShowCloseBtn, "ShowCloseBtn"     );
	mTemplate.emplace_back( TYPE_INT, &mNotebook.CloseBtnOn,   "CloseBtnOn"       );
	mTemplate.emplace_back( TYPE_INT, &mGeneral.UseSplash,     "SplashScreen"     );
	mTemplate.emplace_back( TYPE_INT, &mGeneral.UseStatbar,    "UseStatusbar"     );
	mTemplate.emplace_back( TYPE_INT, &mGeneral.UseDragDrop,   "UseDragDrop"      );
	mTemplate.emplace_back( TYPE_INT, &mGeneral.LanguageID,    "SystemLanguage"   );
	mTemplate.emplace_back( TYPE_INT, &mGeneral.ZoomDefault,   "ZoomDefault"      );
	mTemplate.emplace_back( TYPE_INT, &mDictionary.UseGlobal,  "UseGlobal"        );
	mTemplate.emplace_back( TYPE_INT, &mDictionary.ApplyOn,    "DictApplyOn"      );
	mTemplate.emplace_back( TYPE_STR, &mDictionary.Directory,  "DictGlobalDir"    );
	mTemplate.emplace_back( TYPE_INT, &mTemp.UseTemp,          "UseTemp"          );
	mTemplate.emplace_back( TYPE_INT, &mTemp.ApplyOn,          "TempApplyOn"      );
	mTemplate.emplace_back( TYPE_STR, &mTemp.Directory,        "TempDir"          );
	mTemplate.emplace_back( TYPE_INT, &mDictionary.MatchCase,  "MatchCase"        );
	mTemplate.emplace_back( TYPE_INT, &mDictionary.MatchWhole, "MatchWhole"       );
	mTemplate.emplace_back( TYPE_INT, &mDictionary.UniformClr, "UniformColour"    );
	mTemplate.emplace_back( TYPE_INT, &mAutosave.Use,          "UseAutosave"      );
	mTemplate.emplace_back( TYPE_INT, &mAutosave.Param,        "AutosaveInterval" );
	mTemplate.emplace_back( TYPE_INT, &mAutohigh.Use,          "UseAutohigh"      );
	mTemplate.emplace_back( TYPE_INT, &mAutohigh.Param,        "AutohighInterval" );
	mTemplate.emplace_back( TYPE_INT, &mAutocomp.Use,          "UseAutocomp"      );
	mTemplate.emplace_back( TYPE_INT, &mAutocomp.Param,        "AutocompWords"    );
	mTemplate.emplace_back( TYPE_INT, &mStyle.TextBack,        "TextBack"         );
	mTemplate.emplace_back( TYPE_INT, &mStyle.TextFore,        "TextFore"         );
	mTemplate.emplace_back( TYPE_INT, &mStyle.Caret,           "Caret"            );
	mTemplate.emplace_back( TYPE_INT, &mStyle.LineBack,        "LineBack"         );
	mTemplate.emplace_back( TYPE_INT, &mStyle.Selection,       "Selection"        );
	mTemplate.emplace_back( TYPE_INT, &mStyle.LinenumBack,     "LinenumBack"      );
	mTemplate.emplace_back( TYPE_INT, &mStyle.LinenumFore,     "LinenumFore"      );
	mTemplate.emplace_back( TYPE_STR, &mFont.Face,             "FontFace"         );
	mTemplate.emplace_back( TYPE_INT, &mFont.Size,             "FontSize"         );
	mTemplate.emplace_back( TYPE_INT, &mFont.Style,            "FontStyle"        );
	mTemplate.emplace_back( TYPE_INT, &mFont.Family,           "FontFamily"       );
	mTemplate.emplace_back( TYPE_INT, &mFont.Weight,           "FontWeight"       );
}