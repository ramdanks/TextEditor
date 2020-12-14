#include "Config.h"
#include "LogGUI.h"

//translation unit for static member
wxString Config::sAppPath;
wxString Config::mConfigFilepath;
Config::Font Config::sFont;
Config::Style Config::sStyle;
Config::General Config::sGeneral;
Config::AutoThread Config::sAutosave;
Config::AutoThread Config::sAutohigh;
Config::AutoThread Config::sAutocomp;
Config::Notebook Config::sNotebook;
Config::Dictionary Config::sDictionary;
Config::Temporary Config::sTemp;
std::vector<sConfigReference> Config::mTemplate;

void Config::Init()
{
	wxFileName f( wxStandardPaths::Get().GetExecutablePath() );
	Config::sAppPath = f.GetPath();
}

void Config::FetchData()
{
	PROFILE_FUNC();

	try
	{
		mConfigFilepath = sAppPath + '\\' + CONFIG_FILE;
		auto vRead = Filestream::Read_Bin( std::string( mConfigFilepath ) );
		THROW_ERR_IFEMPTY( vRead, "Failed to read configuration file!" );
		
		if ( mTemplate.empty() ) MakeTemplate();

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

	//create temporary dir if not exists
	if ( !wxDir::Exists( sTemp.Directory ) )
		wxDir::Make( sTemp.Directory );
}

void Config::LoadDefaultConfig()
{
	// system default
	sGeneral.UseSplash     = true;
	sGeneral.LanguageID    = 0;
	sGeneral.ZoomDefault   = 5;
	sGeneral.UseStatbar    = true;
	sGeneral.UseDragDrop   = true;
	// notebook default
	sNotebook.Hide         = false;
	sNotebook.Orientation  = true;
	sNotebook.LockMove     = false;
	sNotebook.FixedWidth   = false;
	sNotebook.MiddleClose  = true;
	sNotebook.ShowCloseBtn = true;
	sNotebook.CloseBtnOn   = false;
	// dictionary defaukt
	sDictionary.UseGlobal  = true;
	sDictionary.ApplyOn    = DICT_ALL_DOCS;
	sDictionary.Directory  = sAppPath + "\\dictionary";
	sDictionary.UniformClr = false;
	sDictionary.MatchCase  = false;
	sDictionary.MatchWhole = true;
	// temporary default
	sTemp.UseTemp          = true;
	sTemp.ApplyOn          = TEMP_APPLY_NEW;
	sTemp.Directory        = sAppPath + "\\temp";
	// autosave default
	sAutosave.Use          = true;
	sAutosave.Param        = 30000;
	// autohighlight default
	sAutohigh.Use          = true;
	sAutohigh.Param        = 200;
	// autocompletion default
	sAutocomp.Use          = true;
	sAutocomp.Param        = 100;
	// font default	       
	sFont.Face             = "Calibri";
	sFont.Size             = 10;
	sFont.Family           = wxFONTFAMILY_MODERN;
	sFont.Style            = wxFONTSTYLE_NORMAL;
	sFont.Weight           = wxFONTWEIGHT_NORMAL;
	// style default       
	sStyle.TextBack        = 2302755;
	sStyle.TextFore        = 16777215;
	sStyle.Caret           = 13405661;
	sStyle.LineBack        = 4605510;
	sStyle.Selection       = 13203258;
	sStyle.LinenumBack     = 3945010;
	sStyle.LinenumFore     = 13408221;

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
		
		Filestream::Write_Bin( ConfigText.c_str(), ConfigText.size(), std::string( mConfigFilepath ) );
		THROW_ERR_IFNOT( wxFile::Exists( mConfigFilepath ), "Cannot save configuration file!" );
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
	if ( Config::sDictionary.MatchWhole ) flags |= wxSTC_FIND_WHOLEWORD;
	if ( Config::sDictionary.MatchCase )  flags |= wxSTC_FIND_MATCHCASE;
	return flags;
}

int Config::GetNotebookStyle()
{
	int style = wxAUI_NB_SCROLL_BUTTONS;
	if ( Config::sNotebook.Orientation )   style |= wxAUI_NB_TOP;
	else                                   style |= wxAUI_NB_BOTTOM;
	if ( Config::sNotebook.FixedWidth )    style |= wxAUI_NB_TAB_FIXED_WIDTH;
	if ( !Config::sNotebook.LockMove )     style |= wxAUI_NB_TAB_MOVE;
	if ( Config::sNotebook.MiddleClose )   style |= wxAUI_NB_MIDDLE_CLICK_CLOSE;
	if ( Config::sNotebook.ShowCloseBtn )
	{
		if ( Config::sNotebook.CloseBtnOn )    style |= wxAUI_NB_CLOSE_ON_ALL_TABS;
		else                                   style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
	}
	return style;
}

void Config::SetNotebookStyle( int style )
{
	Config::sNotebook.Orientation  = style & wxAUI_NB_TOP;
	Config::sNotebook.FixedWidth   = style & wxAUI_NB_BOTTOM;
	Config::sNotebook.LockMove     = style & wxAUI_NB_TAB_FIXED_WIDTH;
	Config::sNotebook.MiddleClose  = style & wxAUI_NB_TAB_MOVE;
	Config::sNotebook.ShowCloseBtn = style & wxAUI_NB_MIDDLE_CLICK_CLOSE;
	Config::sNotebook.CloseBtnOn   = style & wxAUI_NB_CLOSE_ON_ALL_TABS;
}

wxFont Config::BuildFont()
{
	return wxFont( sFont.Size, (wxFontFamily) sFont.Family, (wxFontStyle) sFont.Style,
				   (wxFontWeight) sFont.Weight, false, sFont.Face );
}

void Config::SetFont( wxFont font )
{
	sFont.Face   = font.GetFaceName();
	sFont.Size   = font.GetPointSize();
	sFont.Style  = font.GetStyle();
	sFont.Family = font.GetFamily();
	sFont.Weight = font.GetWeight();
}

void Config::MakeTemplate()
{
	mTemplate.reserve( 30 );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.Hide,         "Hide"             );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.Orientation,  "Orientation"      );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.LockMove,     "LockMove"         );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.FixedWidth,   "FixedWidth"       );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.MiddleClose,  "MiddleMouseClose" );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.ShowCloseBtn, "ShowCloseBtn"     );
	mTemplate.emplace_back( TYPE_INT, &sNotebook.CloseBtnOn,   "CloseBtnOn"       );
	mTemplate.emplace_back( TYPE_INT, &sGeneral.UseSplash,     "SplashScreen"     );
	mTemplate.emplace_back( TYPE_INT, &sGeneral.UseStatbar,    "UseStatusbar"     );
	mTemplate.emplace_back( TYPE_INT, &sGeneral.UseDragDrop,   "UseDragDrop"      );
	mTemplate.emplace_back( TYPE_INT, &sGeneral.LanguageID,    "SystemLanguage"   );
	mTemplate.emplace_back( TYPE_INT, &sGeneral.ZoomDefault,   "ZoomDefault"      );
	mTemplate.emplace_back( TYPE_INT, &sDictionary.UseGlobal,  "UseGlobal"        );
	mTemplate.emplace_back( TYPE_INT, &sDictionary.ApplyOn,    "DictApplyOn"      );
	mTemplate.emplace_back( TYPE_STR, &sDictionary.Directory,  "DictGlobalDir"    );
	mTemplate.emplace_back( TYPE_INT, &sTemp.UseTemp,          "UseTemp"          );
	mTemplate.emplace_back( TYPE_INT, &sTemp.ApplyOn,          "TempApplyOn"      );
	mTemplate.emplace_back( TYPE_STR, &sTemp.Directory,        "TempDir"          );
	mTemplate.emplace_back( TYPE_INT, &sDictionary.MatchCase,  "MatchCase"        );
	mTemplate.emplace_back( TYPE_INT, &sDictionary.MatchWhole, "MatchWhole"       );
	mTemplate.emplace_back( TYPE_INT, &sDictionary.UniformClr, "UniformColour"    );
	mTemplate.emplace_back( TYPE_INT, &sAutosave.Use,          "UseAutosave"      );
	mTemplate.emplace_back( TYPE_INT, &sAutosave.Param,        "AutosaveInterval" );
	mTemplate.emplace_back( TYPE_INT, &sAutohigh.Use,          "UseAutohigh"      );
	mTemplate.emplace_back( TYPE_INT, &sAutohigh.Param,        "AutohighInterval" );
	mTemplate.emplace_back( TYPE_INT, &sAutocomp.Use,          "UseAutocomp"      );
	mTemplate.emplace_back( TYPE_INT, &sAutocomp.Param,        "AutocompWords"    );
	mTemplate.emplace_back( TYPE_INT, &sStyle.TextBack,        "TextBack"         );
	mTemplate.emplace_back( TYPE_INT, &sStyle.TextFore,        "TextFore"         );
	mTemplate.emplace_back( TYPE_INT, &sStyle.Caret,           "Caret"            );
	mTemplate.emplace_back( TYPE_INT, &sStyle.LineBack,        "LineBack"         );
	mTemplate.emplace_back( TYPE_INT, &sStyle.Selection,       "Selection"        );
	mTemplate.emplace_back( TYPE_INT, &sStyle.LinenumBack,     "LinenumBack"      );
	mTemplate.emplace_back( TYPE_INT, &sStyle.LinenumFore,     "LinenumFore"      );
	mTemplate.emplace_back( TYPE_STR, &sFont.Face,             "FontFace"         );
	mTemplate.emplace_back( TYPE_INT, &sFont.Size,             "FontSize"         );
	mTemplate.emplace_back( TYPE_INT, &sFont.Style,            "FontStyle"        );
	mTemplate.emplace_back( TYPE_INT, &sFont.Family,           "FontFamily"       );
	mTemplate.emplace_back( TYPE_INT, &sFont.Weight,           "FontWeight"       );
}