#include "PreferencesFrame.h"
#include "../Feature/LogGUI.h"
#include "../Feature/Language.h"
#include "../Feature/Config.h"
#include "../TextField.h"
#include "EventID.h"

wxFrame* PreferencesFrame::mFrame;
PreferencesFrame::DictionaryPage PreferencesFrame::mDP;
PreferencesFrame::GeneralPage PreferencesFrame::mGP;
wxAuiNotebook* PreferencesFrame::mNotebook;
PreferencesFrame::MainFrame PreferencesFrame::mMF;
PreferencesFrame::TempPage PreferencesFrame::mTP;
std::future<void> sFuture;

void PreferencesFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, MSG_PREFERENCES, wxDefaultPosition, wxSize( 450, 440 ),
						  wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW );
	CreateContent();
	Update();

	mFrame->Bind( wxEVT_CLOSE_WINDOW, OnClose );
	mGP.LocalizationCB->Bind( wxEVT_COMBOBOX, OnLocalization );
}

void PreferencesFrame::ShowAndFocus( bool show, bool focus )
{
	if ( show ) mFrame->Show();
	if ( focus ) mFrame->SetFocus();
}

void PreferencesFrame::Update()
{
	mGP.CB_SB->SetValue( Config::mGeneral.UseStatbar );
	mGP.CB_DD->SetValue( Config::mGeneral.UseDragDrop );
	mGP.CB_SS->SetValue( Config::mGeneral.UseSplash );
	mGP.LocalizationCB->SetSelection( Config::mGeneral.LanguageID );

	mGP.CB_NB_Hide->SetValue( Config::mNotebook.Hide );
	mGP.RB_NB_Top->SetValue( Config::mNotebook.Orientation );
	mGP.RB_NB_Bot->SetValue( !Config::mNotebook.Orientation );
	mGP.CB_NB_Fixed->SetValue( Config::mNotebook.FixedWidth );
	mGP.CB_NB_Lock->SetValue( Config::mNotebook.LockMove );
	mGP.CB_NB_Middle->SetValue( Config::mNotebook.MiddleClose );
	mGP.CB_NB_ShowClose->SetValue( Config::mNotebook.ShowCloseBtn );
	mGP.RB_NB_OnAll->SetValue( Config::mNotebook.CloseBtnOn );
	mGP.RB_NB_OnAct->SetValue( !Config::mNotebook.CloseBtnOn );

	if ( Config::mNotebook.Hide )
	{
		mGP.RB_NB_Top->Disable();
		mGP.RB_NB_Bot->Disable();
	}
	if ( !Config::mNotebook.ShowCloseBtn )
	{
		mGP.RB_NB_OnAll->Disable();
		mGP.RB_NB_OnAct->Disable();
	}
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

	auto panel = new wxPanel( mFrame );
	auto vsizer = new wxBoxSizer( wxVERTICAL );

	int sliderstyle = wxSL_HORIZONTAL | wxSL_MIN_MAX_LABELS;
	int style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_SCROLL_BUTTONS;
	mNotebook = new wxAuiNotebook( panel, -1, wxDefaultPosition, wxDefaultSize, style );

	mGP.Panel = new wxPanel( mNotebook );
	auto local = new wxStaticBox( mGP.Panel, wxID_ANY, "Localization",
								  wxPoint( 10, 10 ), wxSize( 200, 60 ) );
	mGP.LocalizationCB = new wxComboBox( local, 0, "",wxPoint( 15, 23 ),
										 wxSize( 170, 30 ), LanguageList, wxCB_READONLY );

	auto notebook  = new wxStaticBox( mGP.Panel, wxID_ANY, "Tab Bar (Notebook)",
									  wxPoint( 225, 10 ), wxSize( 185, 270 ) );
	mGP.CB_NB_Hide = new wxCheckBox( notebook, wxID_ANY, "Hide Notebook", wxPoint( 15, 25 ) );
	mGP.RB_NB_Top  = new wxRadioButton( notebook, 0, "On Top", wxPoint( 15, 50 ), wxDefaultSize, wxRB_GROUP );
	mGP.RB_NB_Bot = new wxRadioButton( notebook, 0, "On Bottom", wxPoint( 15, 75 ) );
	mGP.CB_NB_Lock = new wxCheckBox( notebook, wxID_ANY, "Lock Page Moving", wxPoint( 15, 100 ) );
	mGP.CB_NB_Fixed = new wxCheckBox( notebook, wxID_ANY, "Fixed Width", wxPoint( 15, 125 ) );
	mGP.CB_NB_Middle = new wxCheckBox( notebook, wxID_ANY, "Middle Mouse to Close", wxPoint( 15, 150 ) );
	mGP.CB_NB_ShowClose = new wxCheckBox( notebook, wxID_ANY, "Show Close Button", wxPoint( 15, 175 ) );
	mGP.RB_NB_OnAll = new wxRadioButton( notebook, 1, "On All Pages", wxPoint( 15, 200 ), wxDefaultSize, wxRB_GROUP );
	mGP.RB_NB_OnAct = new wxRadioButton( notebook, 1, "On Active Pages", wxPoint( 15, 225 ) );

	mGP.CB_NB_Hide->Bind( wxEVT_CHECKBOX, OnCheckHide );
	mGP.CB_NB_ShowClose->Bind( wxEVT_CHECKBOX, OnCheckShowClose );

	auto system = new wxStaticBox( mGP.Panel, wxID_ANY, "System",               wxPoint( 10, 75 ), wxSize( 200, 100 ) );
	mGP.CB_SB   = new wxCheckBox( system, wxID_ANY, "Show Statusbar",           wxPoint( 15, 25 ) );
	mGP.CB_DD   = new wxCheckBox( system, wxID_ANY, "Use Drag and Drop",        wxPoint( 15, 50 ) );
	mGP.CB_SS   = new wxCheckBox( system, wxID_ANY, "Splash Screen on Startup", wxPoint( 15, 75 ) );

	auto toolbar = new wxStaticBox( mGP.Panel, wxID_ANY, "Toolbar", wxPoint( 10, 180 ), wxSize( 200, 100 ) );
	new wxCheckBox( toolbar, wxID_ANY, "Hide Toolbar", wxPoint( 15, 25 ) );
	new wxRadioButton( toolbar, wxID_ANY, "Standard Icons", wxPoint( 15, 50 ) );
	new wxRadioButton( toolbar, wxID_ANY, "Big Icons", wxPoint( 15, 75 ) );

	// Dictionary Setting
	mDP.Panel = new wxPanel( mNotebook );

	auto dpsb = new wxStaticBox( mDP.Panel, -1, "Global Dictionary", wxPoint( 10, 10 ), wxSize( 400, 135 ) );
	new wxCheckBox( dpsb, wxID_ANY, "Disable", wxPoint( 15, 25 ) ); 
	new wxStaticText( dpsb, -1, "Global Path:", wxPoint( 15, 50 ) );
	new wxDirPickerCtrl( dpsb, -1, wxEmptyString, "Global Dictionary", wxPoint( 90, 39 ), wxSize( 295, 40 ) );
	new wxStaticText( dpsb, wxID_ANY, "Automatically Apply to:", wxPoint( 15, 80 ) );
	new wxRadioButton( dpsb, wxID_ANY, "All Documents", wxPoint( 15, 105 ) );
	new wxRadioButton( dpsb, wxID_ANY, "Only Temporary", wxPoint( 120, 105 ) );
	new wxRadioButton( dpsb, wxID_ANY, "Only Opened", wxPoint( 235, 105 ) );

	auto ahsb = new wxStaticBox( mDP.Panel, -1, "Auto-Highlighting", wxPoint( 10, 150 ), wxSize( 400, 80 ) );
	mDP.CB_AH = new wxCheckBox( ahsb, wxID_ANY, "Disable", wxPoint( 15, 25 ) );
	new wxStaticText( ahsb, -1, "Set Interval [ms]:", wxPoint( 15, 50 ) );
	mDP.SpinAutohigh = new wxSpinCtrl( ahsb, -1, wxEmptyString, wxPoint( 340, 45 ),
									   wxDefaultSize, 16384L, MIN_AUTOHIGH_INTERVAL, MAX_AUTOHIGH_INTERVAL );
	auto shigh = new wxSlider( ahsb, WND_ID_SAUTOHIGH, 0, MIN_AUTOHIGH_INTERVAL, MAX_AUTOHIGH_INTERVAL,
							   wxPoint( 120, 45 ), wxSize( 200, 400 ), sliderstyle );

	auto acsb = new wxStaticBox( mDP.Panel, -1, "Auto-Completion", wxPoint( 10, 235 ), wxSize( 400, 80 ) );
	mDP.CB_AC = new wxCheckBox( acsb, wxID_ANY, "Disable", wxPoint( 15, 25 ) );
	new wxStaticText( acsb, -1, "Set Limit [words]:", wxPoint( 15, 50 ) );
	mDP.SpinAutocomp = new wxSpinCtrl( acsb, -1, wxEmptyString, wxPoint( 330, 45 ),
									   wxDefaultSize, 16384L, MIN_AUTOCOMP_WORDS, MAX_AUTOCOMP_WORDS );
	auto scomp = new wxSlider( acsb, WND_ID_SAUTOCOMP, 0, MIN_AUTOCOMP_WORDS, MAX_AUTOCOMP_WORDS,
							   wxPoint( 120, 45 ), wxSize( 190, 400 ), sliderstyle );

	// Autosave Setting
	mTP.Panel = new wxPanel( mNotebook );

	auto assb = new wxStaticBox( mTP.Panel, -1, "Temporary Files", wxPoint( 10, 10 ), wxSize( 400, 135 ) );
	mTP.CB_AS = new wxCheckBox( assb, wxID_ANY, "Disable", wxPoint( 15, 25 ) );
	new wxStaticText( assb, -1, "Temporary Path:", wxPoint( 15, 50 ) );
	new wxDirPickerCtrl( assb, -1, wxEmptyString, "Temporary", wxPoint( 110, 39 ), wxSize( 275, 40 ) );
	new wxStaticText( assb, wxID_ANY, "Create Temporary for:", wxPoint( 15, 80 ) );
	new wxRadioButton( assb, wxID_ANY, "All Documents", wxPoint( 15, 105 ) );
	new wxRadioButton( assb, wxID_ANY, "New Documents", wxPoint( 120, 105 ) );

	auto autosave = new wxStaticBox( mTP.Panel, -1, "Auto-Save", wxPoint( 10, 150 ), wxSize( 400, 80 ) );
	new wxCheckBox( autosave, wxID_ANY, "Disable", wxPoint( 15, 25 ) );
	new wxStaticText( autosave, -1, "Set Interval [sec]:", wxPoint( 15, 50 ) );
	mTP.SpinAutosave = new wxSpinCtrl( autosave, -1, wxEmptyString, wxPoint( 340, 45 ),
									   wxDefaultSize, 16384L, MIN_AUTOSAVE_INTERVAL, MAX_AUTOSAVE_INTERVAL );
	auto ssave = new wxSlider( autosave, WND_ID_SAUTOSAVE, 0, MIN_AUTOSAVE_INTERVAL, MAX_AUTOSAVE_INTERVAL,
							   wxPoint( 120, 45 ), wxSize( 200, 25 ), sliderstyle );

	shigh->Bind( wxEVT_SCROLL_CHANGED, OnScroll );
	scomp->Bind( wxEVT_SCROLL_CHANGED, OnScroll );
	ssave->Bind( wxEVT_SCROLL_CHANGED, OnScroll );

	mNotebook->AddPage( mGP.Panel, "General" );
	mNotebook->AddPage( mDP.Panel, "Dictionary and Autocomp" );
	mNotebook->AddPage( mTP.Panel, "Temporary and Autosave" );

	auto btnsizer = new wxBoxSizer( wxHORIZONTAL );
	auto btnOK = new wxButton( panel, -1, "OK" );
	auto btnCC = new wxButton( panel, -1, "Cancel" );
	btnOK->Bind( wxEVT_BUTTON, OnOK );
	btnCC->Bind( wxEVT_BUTTON, OnCancel );

	btnsizer->Add( btnOK, 0, wxRIGHT, 10 );
	btnsizer->Add( btnCC, 0 );

	vsizer->Add( mNotebook, 1, wxEXPAND );
	vsizer->Add( btnsizer, 0, wxALIGN_CENTER | wxBOTTOM | wxTOP, 10 );

	panel->SetSizer( vsizer );
}

void PreferencesFrame::RefreshMessage()
{
	PROFILE_FUNC();

	mMF.Statbar->SetLabelText( MSG_STATUSBAR );
	// file menu
	{
		mMF.File->SetLabel( ID_NEWFILE, MSG_NEW + "\tCtrl-N" );
		mMF.File->SetLabel( ID_SAVEFILE, MSG_SAVE + "\tCtrl-S" );
		mMF.File->SetLabel( ID_SAVEFILEAS, MSG_SAVEAS + "\tCtrl-Alt-S" );
		mMF.File->SetLabel( ID_SAVEFILEALL, MSG_SAVEALL + "\tCtrl-Shift-S" );
		mMF.File->SetLabel( ID_OPENFILE, MSG_OPEN + "\tCtrl-O" );
		mMF.File->SetLabel( ID_RENAMEFILE, MSG_RENAME );
		mMF.File->SetLabel( ID_TABCLOSE, MSG_CLOSE + "\tCtrl-W" );
		mMF.File->SetLabel( ID_TABCLOSEALL, MSG_CLOSEALL + "\tCtrl-Shift-W" );
		mMF.File->SetLabel( ID_EMBEDDICT, "Text Highlighting" );
		mMF.File->SetLabel( ID_REFRESHDICT, "Refresh\tCtrl-R" );
		mMF.File->SetLabel( ID_SHARE, "Share" );
		mMF.File->SetLabel( wxID_EXIT, MSG_EXIT + "\tAlt+F4" );
	}
	// edit menu
	{
		mMF.Edit->SetLabel( ID_UNDO, MSG_UNDO + "\tCtrl-Z" );
		mMF.Edit->SetLabel( ID_REDO, MSG_REDO + "\tCtrl-Y" );
		mMF.Edit->SetLabel( ID_CUT, MSG_CUT + "\tCtrl-X" );
		mMF.Edit->SetLabel( ID_COPY, MSG_COPY + "\tCtrl-C" );
		mMF.Edit->SetLabel( ID_PASTE, MSG_PASTE + "\tCtrl-V" );
		mMF.Edit->SetLabel( ID_DELETE, MSG_DELETE + "\tDel" );
		mMF.Edit->SetLabel( ID_SELECTALL, MSG_SELECTALL + "\tCtrl-A" );
		// case submenu
		{
			mMF.Case->SetLabel( ID_UPPERCASE, MSG_UPPCASE );
			mMF.Case->SetLabel( ID_LOWERCASE, MSG_LOWCASE );
			mMF.Case->SetLabel( ID_INVERSECASE, MSG_INVCASE );
			mMF.Case->SetLabel( ID_RANDOMCASE, MSG_RANCASE );
		}
		// eol submenu
		{
			mMF.EOL->SetLabel( ID_EOL_LF, MSG_EOLUNX );
			mMF.EOL->SetLabel( ID_EOL_CR, MSG_EOLMAC );
			mMF.EOL->SetLabel( ID_EOL_CRLF, MSG_EOLWIN );
		}
	}
	// find menu
	{
		mMF.Search->SetLabel( ID_FIND, MSG_FIND + "\tCtrl-F" );
		mMF.Search->SetLabel( ID_SELECTFNEXT, MSG_SELECTFNEXT + "\tCtrl-F3" );
		mMF.Search->SetLabel( ID_SELECTFPREV, MSG_SELECTFPREV + "\tCtrl-Shift-F3" );
		mMF.Search->SetLabel( ID_REPLACE, MSG_REPLACE + "\tCtrl-H" );
		mMF.Search->SetLabel( ID_GOTO, MSG_GOTO + "\tCtrl-G" );
	}
	// view menu
	{
		mMF.View->SetLabel( ID_STAYONTOP, MSG_AOT );
		mMF.View->SetLabel( ID_ZOOMIN, MSG_ZOOMIN + "\tCtrl-Num +" );
		mMF.View->SetLabel( ID_ZOOMOUT, MSG_ZOOMOUT + "\tCtrl-Num -" );
		mMF.View->SetLabel( ID_ZOOMRESTORE, MSG_ZOOMRESTORE + "\tCtrl-Num /" );
		mMF.View->SetLabel( ID_TEXTSUM, MSG_TEXTSUM );
		mMF.View->SetLabel( ID_COMPSUM, MSG_COMPRESSIONSUM );
#ifndef _DIST                                                                  
		mMF.View->SetLabel( ID_DEBUGCONSOLE, MSG_DEBUGCONSOLE );
#endif                                                                        
	}
	// setting menu
	{
		mMF.Setting->SetLabel( ID_PREFERENCES, MSG_PREFERENCES );
		mMF.Setting->SetLabel( ID_STYLECONFIG, MSG_STYLECONFIG );
	}
	// help menu
	{
		mMF.Help->SetLabel( ID_REPORTBUG, MSG_REPORTBUG );
		mMF.Help->SetLabel( ID_LOGDIR, MSG_OPENLOGDIR );
		mMF.Help->SetLabel( ID_DOCUMENTATION, MSG_SEEDOC );
		mMF.Help->SetLabel( wxID_ABOUT, MSG_ABOUT );
	}
	// main menu
	mMF.MenuBar->SetMenuLabel( 0, MSG_FILE );
	mMF.MenuBar->SetMenuLabel( 1, MSG_EDIT );
	mMF.MenuBar->SetMenuLabel( 2, MSG_SEARCH );
	mMF.MenuBar->SetMenuLabel( 3, MSG_VIEW );
	mMF.MenuBar->SetMenuLabel( 4, MSG_SETTINGS );
	mMF.MenuBar->SetMenuLabel( 5, MSG_HELP );
}

void PreferencesFrame::UpdateNotebook()
{
	Config::mNotebook.Hide         = mGP.CB_NB_Hide->GetValue();
	Config::mNotebook.Orientation  = mGP.RB_NB_Top->GetValue();
	Config::mNotebook.FixedWidth   = mGP.CB_NB_Fixed->GetValue();
	Config::mNotebook.LockMove     = mGP.CB_NB_Lock->GetValue();
	Config::mNotebook.MiddleClose  = mGP.CB_NB_Middle->GetValue();
	Config::mNotebook.ShowCloseBtn = mGP.CB_NB_ShowClose->GetValue();
	Config::mNotebook.CloseBtnOn   = mGP.RB_NB_OnAll->GetValue();
	
	if ( Config::mNotebook.Hide )  TextField::mNotebook->Show( false );
	else                           TextField::mNotebook->Show( true );
	
	TextField::mNotebook->SetWindowStyle( Config::GetNotebookStyle() );
}

void PreferencesFrame::UpdateDragDrop()
{
	auto isDragDrop = mGP.CB_DD->GetValue();
	if ( Config::mGeneral.UseDragDrop != isDragDrop )
	{
		Config::mGeneral.UseDragDrop = isDragDrop;
		for ( const auto& page : TextField::mPageData )
			page.TextField->DragAcceptFiles( isDragDrop );		
	}
}

void PreferencesFrame::UpdateAutocomp()
{
	auto isAutocomp = !mDP.CB_AC->GetValue();
	Config::mAutocomp.Param = mDP.SpinAutocomp->GetValue();
	if ( Config::mAutocomp.Use != isAutocomp )
	{
		Config::mAutocomp.Use = isAutocomp;
		TextField::isAutocomp = isAutocomp;
	}
}

void PreferencesFrame::UpdateAutohigh()
{
	auto isAutohigh = !mDP.CB_AH->GetValue();
	Config::mAutohigh.Param = mDP.SpinAutohigh->GetValue();
	if ( Config::mAutohigh.Use != isAutohigh )
	{
		Config::mAutohigh.Use = isAutohigh;
		if ( isAutohigh )
			if ( AutoThread::isDeploy( THREAD_HIGHLIGHTER ) ) AutoThread::Continue( THREAD_HIGHLIGHTER );
			else                                              AutoThread::DeployThread( THREAD_HIGHLIGHTER );
		else AutoThread::Pause( THREAD_HIGHLIGHTER );
	}
}

void PreferencesFrame::UpdateAutosave()
{
	auto isAutosave = !mTP.CB_AS->GetValue();
	Config::mAutosave.Param = mTP.SpinAutosave->GetValue() * 1000;
	if ( Config::mAutosave.Use != isAutosave )
	{
		Config::mAutosave.Use = isAutosave;
		if ( isAutosave )
			if ( AutoThread::isDeploy( THREAD_SAVER ) ) AutoThread::Continue( THREAD_SAVER );
			else                                        AutoThread::DeployThread( THREAD_SAVER );
		else AutoThread::Pause( THREAD_SAVER );
	}
}

void PreferencesFrame::UpdateStatbar()
{
	auto isStatbar = mGP.CB_SB->GetValue();
	if ( Config::mGeneral.UseStatbar != isStatbar )
	{
		Config::mGeneral.UseStatbar = isStatbar;
		auto size = mMF.Frame->GetSize();
		if ( isStatbar )
		{
			mMF.Statbar->Show( true );
			size.y += 20;
		}
		else
		{
			mMF.Statbar->Show( false );
			size.y -= 20;
		}
		mMF.Frame->SetSize( size );
	}
}

void PreferencesFrame::OnCheckHide( wxCommandEvent& event )
{
	auto btn = (wxCheckBox*) event.GetEventObject();
	if ( !btn->GetValue() )
	{
		mGP.RB_NB_Top->Enable();
		mGP.RB_NB_Bot->Enable();
	}
	else
	{
		mGP.RB_NB_Top->Disable();
		mGP.RB_NB_Bot->Disable();
	}
}

void PreferencesFrame::OnCheckShowClose( wxCommandEvent& event )
{
	auto btn = (wxCheckBox*) event.GetEventObject();
	if ( btn->GetValue() )
	{
		mGP.RB_NB_OnAll->Enable();
		mGP.RB_NB_OnAct->Enable();
	}
	else
	{
		mGP.RB_NB_OnAll->Disable();
		mGP.RB_NB_OnAct->Disable();
	}
}

void PreferencesFrame::OnOK( wxCommandEvent& event )
{
	PROFILE_FUNC();
	UpdateStatbar();
	UpdateAutosave();
	UpdateAutohigh();
	UpdateAutocomp();
	UpdateDragDrop();
	UpdateNotebook();
	Config::mGeneral.UseSplash = mGP.CB_SS->GetValue();

	mFrame->Show( false );
}

void PreferencesFrame::OnCancel( wxCommandEvent& event )
{
	mFrame->Show( false );
}

void PreferencesFrame::OnLocalization( wxCommandEvent& event )
{
	PROFILE_FUNC();
	auto sel = mGP.LocalizationCB->GetSelection();
	if ( sel == wxID_NONE ) return;

	if ( Config::mGeneral.LanguageID != sel )
	{
		Config::mGeneral.LanguageID = sel;
		if ( Language::LoadMessage( static_cast<LanguageID>( sel ) ) )
			sFuture = std::async( std::launch::async, RefreshMessage );
	}
}

void PreferencesFrame::OnClose( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void PreferencesFrame::OnScroll( wxScrollEvent& event )
{
	auto slider = (wxSlider*) event.GetEventObject();
	switch ( slider->GetId() )
	{
	case WND_ID_SAUTOHIGH:
		mDP.SpinAutohigh->SetValue( slider->GetValue() );
	case WND_ID_SAUTOCOMP:
		mDP.SpinAutocomp->SetValue( slider->GetValue() );
	case WND_ID_SAUTOSAVE:
		mTP.SpinAutosave->SetValue( slider->GetValue() );
	default:
		break;
	}
}