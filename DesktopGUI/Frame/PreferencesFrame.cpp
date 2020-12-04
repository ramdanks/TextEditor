#include "PreferencesFrame.h"
#include "../Feature/LogGUI.h"
#include "../Feature/Language.h"
#include "../Feature/Config.h"
#include "EventID.h"

wxFrame* PreferencesFrame::mFrame;
PreferencesFrame::DictionaryPage PreferencesFrame::mDP;
PreferencesFrame::GeneralPage PreferencesFrame::mGP;
wxAuiNotebook* PreferencesFrame::mNotebook;
PreferencesFrame::MainFrame PreferencesFrame::mMFmenu;
std::future<void> sFuture;

void PreferencesFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, MSG_PREFERENCES, wxDefaultPosition, wxSize( 600, 400 ),
						  wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW );
	CreateContent();

	mFrame->Bind( wxEVT_CLOSE_WINDOW, OnClose );
	mGP.LocalizationCB->Bind( wxEVT_COMBOBOX, OnLocalization );
}

void PreferencesFrame::ShowAndFocus( bool show, bool focus )
{
	if ( show ) mFrame->Show();
	if ( focus ) mFrame->SetFocus();
	mGP.LocalizationCB->SetSelection( Config::mLanguageID );
}

void PreferencesFrame::CreateContent()
{
	wxArrayString LanguageList;
	LanguageList.Add( "English" );
	LanguageList.Add( "Bahasa Indonesia" );
	LanguageList.Add( "Spanish" );
	LanguageList.Add( "Portuguese" );
	LanguageList.Add( "German" );
	LanguageList.Add( "Italian" );
	LanguageList.Add( "French" );
	LanguageList.Add( "Dutch" );

	int style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_SCROLL_BUTTONS;
	mNotebook = new wxAuiNotebook( mFrame, -1, wxDefaultPosition, wxDefaultSize, style );

	mGP.Panel = new wxPanel( mNotebook );
	auto local = new wxStaticBox( mGP.Panel, wxID_ANY, "Localization", wxPoint( 100, 20 ), wxSize( 196, 60 ) );
	mGP.LocalizationCB = new wxComboBox( local, 0, "", wxPoint( 13, 23 ), wxSize( 170, 30 ), LanguageList, wxCB_READONLY );

	auto system = new wxStaticBox( mGP.Panel, wxID_ANY, "System", wxPoint( 320, 20 ), wxSize( 200, 300 ) );
	new wxCheckBox( system, wxID_ANY, "Hide", wxPoint( 15, 25 ) );
	new wxCheckBox( system, wxID_ANY, "Lock Page Moving", wxPoint( 15, 50 ) );
	new wxCheckBox( system, wxID_ANY, "Show Statusbar", wxPoint( 15, 75 ) );
	new wxCheckBox( system, wxID_ANY, "Show Close Button", wxPoint( 15, 100 ) );
	new wxCheckBox( system, wxID_ANY, "", wxPoint( 15, 125 ) );
	new wxCheckBox( system, wxID_ANY, "", wxPoint( 15, 150 ) );
	new wxCheckBox( system, wxID_ANY, "", wxPoint( 15, 175 ) );

	mDP.Panel = new wxPanel( mNotebook );

	mNotebook->AddPage( mGP.Panel, "General" );
	mNotebook->AddPage( mDP.Panel, "Dictionary" );
}

void PreferencesFrame::RefreshMessage()
{
	PROFILE_FUNC();

	mMFmenu.Statbar->SetLabelText( MSG_STATUSBAR );
	// file menu
	{
		mMFmenu.File->SetLabel( ID_NEWFILE, MSG_NEW + "\tCtrl-N" );
		mMFmenu.File->SetLabel( ID_SAVEFILE, MSG_SAVE + "\tCtrl-S" );
		mMFmenu.File->SetLabel( ID_SAVEFILEAS, MSG_SAVEAS + "\tCtrl-Alt-S" );
		mMFmenu.File->SetLabel( ID_SAVEFILEALL, MSG_SAVEALL + "\tCtrl-Shift-S" );
		mMFmenu.File->SetLabel( ID_OPENFILE, MSG_OPEN + "\tCtrl-O" );
		mMFmenu.File->SetLabel( ID_RENAMEFILE, MSG_RENAME );
		mMFmenu.File->SetLabel( ID_TABCLOSE, MSG_CLOSE + "\tCtrl-W" );
		mMFmenu.File->SetLabel( ID_TABCLOSEALL, MSG_CLOSEALL + "\tCtrl-Shift-W" );
		mMFmenu.File->SetLabel( ID_EMBEDDICT, "Text Highlighting" );
		mMFmenu.File->SetLabel( ID_REFRESHDICT, "Refresh\tCtrl-R" );
		mMFmenu.File->SetLabel( ID_SHARE, "Share" );
		mMFmenu.File->SetLabel( wxID_EXIT, MSG_EXIT + "\tAlt+F4" );
	}
	// edit menu
	{
		mMFmenu.Edit->SetLabel( ID_UNDO, MSG_UNDO + "\tCtrl-Z" );
		mMFmenu.Edit->SetLabel( ID_REDO, MSG_REDO + "\tCtrl-Y" );
		mMFmenu.Edit->SetLabel( ID_CUT, MSG_CUT + "\tCtrl-X" );
		mMFmenu.Edit->SetLabel( ID_COPY, MSG_COPY + "\tCtrl-C" );
		mMFmenu.Edit->SetLabel( ID_PASTE, MSG_PASTE + "\tCtrl-V" );
		mMFmenu.Edit->SetLabel( ID_DELETE, MSG_DELETE + "\tDel" );
		mMFmenu.Edit->SetLabel( ID_SELECTALL, MSG_SELECTALL + "\tCtrl-A" );
		// case submenu
		{
			mMFmenu.Case->SetLabel( ID_UPPERCASE, MSG_UPPCASE );
			mMFmenu.Case->SetLabel( ID_LOWERCASE, MSG_LOWCASE );
			mMFmenu.Case->SetLabel( ID_INVERSECASE, MSG_INVCASE );
			mMFmenu.Case->SetLabel( ID_RANDOMCASE, MSG_RANCASE );
		}
		// eol submenu
		{
			mMFmenu.EOL->SetLabel( ID_EOL_LF, MSG_EOLUNX );
			mMFmenu.EOL->SetLabel( ID_EOL_CR, MSG_EOLMAC );
			mMFmenu.EOL->SetLabel( ID_EOL_CRLF, MSG_EOLWIN );
		}
	}
	// find menu
	{
		mMFmenu.Search->SetLabel( ID_FIND, MSG_FIND + "\tCtrl-F" );
		mMFmenu.Search->SetLabel( ID_SELECTFNEXT, MSG_SELECTFNEXT + "\tCtrl-F3" );
		mMFmenu.Search->SetLabel( ID_SELECTFPREV, MSG_SELECTFPREV + "\tCtrl-Shift-F3" );
		mMFmenu.Search->SetLabel( ID_REPLACE, MSG_REPLACE + "\tCtrl-H" );
		mMFmenu.Search->SetLabel( ID_GOTO, MSG_GOTO + "\tCtrl-G" );
	}
	// view menu
	{
		mMFmenu.View->SetLabel( ID_STAYONTOP, MSG_AOT );
		mMFmenu.View->SetLabel( ID_ZOOMIN, MSG_ZOOMIN + "\tCtrl-Num +" );
		mMFmenu.View->SetLabel( ID_ZOOMOUT, MSG_ZOOMOUT + "\tCtrl-Num -" );
		mMFmenu.View->SetLabel( ID_ZOOMRESTORE, MSG_ZOOMRESTORE + "\tCtrl-Num /" );
		mMFmenu.View->SetLabel( ID_TEXTSUM, MSG_TEXTSUM );
		mMFmenu.View->SetLabel( ID_COMPSUM, MSG_COMPRESSIONSUM );
#ifndef _DIST                                                                  
		mMFmenu.View->SetLabel( ID_DEBUGCONSOLE, MSG_DEBUGCONSOLE );
#endif                                                                        
	}
	// setting menu
	{
		mMFmenu.Setting->SetLabel( ID_PREFERENCES, MSG_PREFERENCES );
		mMFmenu.Setting->SetLabel( ID_STYLECONFIG, MSG_STYLECONFIG );
	}
	// help menu
	{
		mMFmenu.Help->SetLabel( ID_REPORTBUG, MSG_REPORTBUG );
		mMFmenu.Help->SetLabel( ID_LOGDIR, MSG_OPENLOGDIR );
		mMFmenu.Help->SetLabel( ID_DOCUMENTATION, MSG_SEEDOC );
		mMFmenu.Help->SetLabel( wxID_ABOUT, MSG_ABOUT );
	}
	// main menu
	mMFmenu.MenuBar->SetMenuLabel( 0, MSG_FILE );
	mMFmenu.MenuBar->SetMenuLabel( 1, MSG_EDIT );
	mMFmenu.MenuBar->SetMenuLabel( 2, MSG_SEARCH );
	mMFmenu.MenuBar->SetMenuLabel( 3, MSG_VIEW );
	mMFmenu.MenuBar->SetMenuLabel( 4, MSG_SETTINGS );
	mMFmenu.MenuBar->SetMenuLabel( 5, MSG_HELP );
}

void PreferencesFrame::OnLocalization( wxCommandEvent& event )
{
	PROFILE_FUNC();
	auto sel = mGP.LocalizationCB->GetSelection();
	if ( sel == wxID_NONE ) return;

	if ( Config::mLanguageID != sel )
	{
		Config::mLanguageID = sel;
		if ( Language::LoadMessage( static_cast<LanguageID>( sel ) ) )
			sFuture = std::async( std::launch::async, RefreshMessage );
	}
}

void PreferencesFrame::OnClose( wxCloseEvent& event )
{
	mFrame->Show( false );
}