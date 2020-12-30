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
	PROFILE_FUNC();

	wxCommandEvent evt = wxEVT_NULL;

	mGP.CB_SB->SetValue( Config::sGeneral.UseStatbar );
	mGP.CB_DD->SetValue( Config::sGeneral.UseDragDrop );
	mGP.CB_SS->SetValue( Config::sGeneral.UseSplash );
	mGP.LocalizationCB->SetSelection( Config::sGeneral.LanguageID );

	// notebook setting
	mGP.CB_NB_Hide->SetValue  ( Config::sNotebook.Hide         );
	mGP.RB_NB_Top->SetValue   ( Config::sNotebook.Orientation  );
	mGP.RB_NB_Bot->SetValue   ( !Config::sNotebook.Orientation );
	mGP.CB_NB_Fixed->SetValue ( Config::sNotebook.FixedWidth   );
	mGP.CB_NB_Lock->SetValue  ( Config::sNotebook.LockMove     );
	mGP.CB_NB_Middle->SetValue( Config::sNotebook.MiddleClose  );
	mGP.CB_NB_Close->SetValue ( Config::sNotebook.ShowCloseBtn );
	mGP.RB_NB_OnAll->SetValue ( Config::sNotebook.CloseBtnOn   );
	mGP.RB_NB_OnAct->SetValue ( !Config::sNotebook.CloseBtnOn  );
	OnCheckHide( evt );
	OnCheckShowClose( evt );

	// dictionary
	mDP.Disable->SetValue( !Config::sDictionary.UseGlobal );
	mDP.OnAll->SetValue  ( Config::sDictionary.ApplyOn == DICT_ALL_DOCS );
	mDP.OnTemp->SetValue ( Config::sDictionary.ApplyOn == DICT_TMP_DOCS );
	mDP.OnOpen->SetValue ( Config::sDictionary.ApplyOn == DICT_OPN_DOCS );
	mDP.DirPick->SetPath ( Config::sDictionary.Directory );
	OnCheckDict( evt );

	// autohighlight setting
	mDP.CB_AH->SetValue( !Config::sAutohigh.Use );
	mDP.SL_AH->SetValue( Config::sAutohigh.Param );
	mDP.SC_AH->SetValue( Config::sAutohigh.Param );
	UpdatePanelAuto( WND_ID_SAUTOHIGH );

	// autocompletion setting
	mDP.CB_AC->SetValue( !Config::sAutocomp.Use );
	mDP.SL_AC->SetValue( Config::sAutocomp.Param );
	mDP.SC_AC->SetValue( Config::sAutocomp.Param );
	UpdatePanelAuto( WND_ID_SAUTOCOMP );

	// temporary
	mTP.Disable->SetValue( !Config::sTemp.UseTemp );
	mTP.DirPick->SetPath ( Config::sTemp.Directory );
	mTP.OnAll->SetValue  ( Config::sTemp.ApplyOn == TEMP_APPLY_ALL );
	mTP.OnNew->SetValue  ( Config::sTemp.ApplyOn == TEMP_APPLY_NEW );
	OnCheckTemp( evt );

	// autosave setting
	mTP.CB_AS->SetValue( !Config::sAutosave.Use );
	mTP.SL_AS->SetValue( Config::sAutosave.Param / 1000 );
	mTP.SC_AS->SetValue( Config::sAutosave.Param / 1000 );
	UpdatePanelAuto( WND_ID_SAUTOSAVE );

	// toolbar doesnt available yey
	mGP.CB_TB_Hide->Disable();
	mGP.RB_TB_Std->Disable();
	mGP.RB_TB_Big->Disable();
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
	mNotebook = new wxAuiNotebook( panel, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP );

	mGP.Panel = new wxPanel( mNotebook );
	auto local = new wxStaticBox( mGP.Panel, wxID_ANY, "Localization", wxPoint( 10, 10 ), wxSize( 200, 60 ) );
	mGP.LocalizationCB = new wxComboBox( local, 0, "",wxPoint( 15, 23 ), wxSize( 170, 30 ), LanguageList, wxCB_READONLY );

	auto notebook    = new wxStaticBox  ( mGP.Panel, 0, "Tab Bar (Notebook)", wxPoint( 225, 10 ), wxSize( 185, 270 ) );
	mGP.CB_NB_Hide   = new wxCheckBox   ( notebook,  0, "Hide Notebook",      wxPoint( 15, 25 ) );
	mGP.RB_NB_Top    = new wxRadioButton( notebook,  1, "On Top",             wxPoint( 15, 50 ), wxDefaultSize, wxRB_GROUP );
	mGP.RB_NB_Bot    = new wxRadioButton( notebook,  1, "On Bottom",          wxPoint( 15, 75 ) );
	mGP.CB_NB_Lock   = new wxCheckBox   ( notebook,  0, "Lock Page Moving",   wxPoint( 15, 100 ) );
	mGP.CB_NB_Fixed  = new wxCheckBox   ( notebook,  0, "Fixed Width",        wxPoint( 15, 125 ) );
	mGP.CB_NB_Middle = new wxCheckBox   ( notebook,  0, "Middle Mouse Close", wxPoint( 15, 150 ) );
	mGP.CB_NB_Close  = new wxCheckBox   ( notebook,  0, "Show Close Button",  wxPoint( 15, 175 ) );
	mGP.RB_NB_OnAll  = new wxRadioButton( notebook,  1, "On All Pages",       wxPoint( 15, 200 ), wxDefaultSize, wxRB_GROUP );
	mGP.RB_NB_OnAct  = new wxRadioButton( notebook,  1, "On Active Pages",    wxPoint( 15, 225 ) );

	auto system = new wxStaticBox( mGP.Panel, 0, "System",                   wxPoint( 10, 75 ), wxSize( 200, 100 ) );
	mGP.CB_SB   = new wxCheckBox ( system,    0, "Show Statusbar",           wxPoint( 15, 25 ) );
	mGP.CB_DD   = new wxCheckBox ( system,    0, "Use Drag and Drop",        wxPoint( 15, 50 ) );
	mGP.CB_SS   = new wxCheckBox ( system,    0, "Splash Screen on Startup", wxPoint( 15, 75 ) );

	auto toolbar   = new wxStaticBox  ( mGP.Panel, 0, "Toolbar",        wxPoint( 10, 180 ), wxSize( 200, 100 ) );
	mGP.CB_TB_Hide = new wxCheckBox   ( toolbar,   0, "Hide Toolbar",   wxPoint( 15, 25 ) );
	mGP.RB_TB_Std  = new wxRadioButton( toolbar,   0, "Standard Icons", wxPoint( 15, 50 ) );
	mGP.RB_TB_Big  = new wxRadioButton( toolbar,   0, "Big Icons",      wxPoint( 15, 75 ) );

	// Dictionary Setting
	mDP.Panel = new wxPanel( mNotebook );

	auto dpsb = new wxStaticBox( mDP.Panel, -1, "Global Dictionary", wxPoint( 10, 10 ), wxSize( 400, 135 ) );
	new wxStaticText                 ( dpsb, -1, "Global Path:", wxPoint( 15, 50 ) );
	new wxStaticText                 ( dpsb, wxID_ANY, "Automatically Apply to:", wxPoint( 15, 80 ) );
	mDP.Disable = new wxCheckBox     ( dpsb, wxID_ANY, "Disable", wxPoint( 15, 25 ) ); 
	mDP.DirPick = new wxDirPickerCtrl( dpsb, -1, wxEmptyString, "Global Dictionary", wxPoint( 90, 39 ), wxSize( 295, 40 ) );
	mDP.OnAll   = new wxRadioButton  ( dpsb, wxID_ANY, "All Documents", wxPoint( 15, 105 ) );
	mDP.OnTemp  = new wxRadioButton  ( dpsb, wxID_ANY, "Only New", wxPoint( 120, 105 ) );
	mDP.OnOpen  = new wxRadioButton  ( dpsb, wxID_ANY, "Only Opened", wxPoint( 203, 105 ) );

	auto ahsb = new wxStaticBox( mDP.Panel, -1, "Auto-Highlighting", wxPoint( 10, 150 ), wxSize( 400, 80 ) );
	new wxStaticText          ( ahsb, -1, "Set Interval [ms]:", wxPoint( 15, 50 ) );
	mDP.CB_AH = new wxCheckBox( ahsb, WND_ID_SAUTOHIGH, "Disable", wxPoint( 15, 25 ) );
	mDP.SC_AH = new wxSpinCtrl( ahsb, WND_ID_SAUTOHIGH, wxEmptyString, wxPoint( 340, 45 ), wxDefaultSize, 16384L, MIN_AUTOHIGH_INTERVAL, MAX_AUTOHIGH_INTERVAL );
	mDP.SL_AH = new wxSlider  ( ahsb, WND_ID_SAUTOHIGH, 0, MIN_AUTOHIGH_INTERVAL, MAX_AUTOHIGH_INTERVAL, wxPoint( 120, 45 ), wxSize( 200, 400 ), sliderstyle );

	auto acsb = new wxStaticBox( mDP.Panel, -1, "Auto-Completion", wxPoint( 10, 235 ), wxSize( 400, 80 ) );
	new wxStaticText          ( acsb, -1, "Set Limit [words]:", wxPoint( 15, 50 ) );
	mDP.CB_AC = new wxCheckBox( acsb, WND_ID_SAUTOCOMP, "Disable", wxPoint( 15, 25 ) );
	mDP.SC_AC = new wxSpinCtrl( acsb, WND_ID_SAUTOCOMP, wxEmptyString, wxPoint( 330, 45 ), wxDefaultSize, 16384L, MIN_AUTOCOMP_WORDS, MAX_AUTOCOMP_WORDS );
	mDP.SL_AC = new wxSlider  ( acsb, WND_ID_SAUTOCOMP, 0, MIN_AUTOCOMP_WORDS, MAX_AUTOCOMP_WORDS, wxPoint( 120, 45 ), wxSize( 190, 400 ), sliderstyle );

	// Autosave Setting
	mTP.Panel = new wxPanel( mNotebook );

	auto assb = new wxStaticBox( mTP.Panel, -1, "Temporary Files", wxPoint( 10, 10 ), wxSize( 400, 135 ) );
	new wxStaticText                 ( assb, -1, "Create Temporary for:", wxPoint( 15, 80 ) );
	new wxStaticText                 ( assb, -1, "Temporary Path:", wxPoint( 15, 50 ) );
	mTP.Disable = new wxCheckBox     ( assb, -1, "Disable", wxPoint( 15, 25 ) );
	mTP.DirPick = new wxDirPickerCtrl( assb, -1, wxEmptyString, "Temporary", wxPoint( 110, 39 ), wxSize( 275, 40 ) );
	mTP.OnAll   = new wxRadioButton  ( assb, -1, "All Documents", wxPoint( 15, 105 ) );
	mTP.OnNew   = new wxRadioButton  ( assb, -1, "New Documents", wxPoint( 120, 105 ) );

	auto autosave = new wxStaticBox( mTP.Panel, -1, "Auto-Save", wxPoint( 10, 150 ), wxSize( 400, 80 ) );
	new wxStaticText( autosave, -1, "Set Interval [sec]:", wxPoint( 15, 50 ) );
	mTP.CB_AS = new wxCheckBox( autosave, WND_ID_SAUTOSAVE, "Disable", wxPoint( 15, 25 ) );
	mTP.SC_AS = new wxSpinCtrl( autosave, WND_ID_SAUTOSAVE, wxEmptyString, wxPoint( 340, 45 ), wxDefaultSize, 16384L, MIN_AUTOSAVE_INTERVAL, MAX_AUTOSAVE_INTERVAL );
	mTP.SL_AS = new wxSlider( autosave, WND_ID_SAUTOSAVE, 0, MIN_AUTOSAVE_INTERVAL, MAX_AUTOSAVE_INTERVAL, wxPoint( 120, 45 ), wxSize( 200, 25 ), sliderstyle );
	
	mGP.CB_NB_Hide->Bind( wxEVT_CHECKBOX, OnCheckHide );
	mGP.CB_NB_Close->Bind( wxEVT_CHECKBOX, OnCheckShowClose );

	mTP.Disable->Bind( wxEVT_CHECKBOX, OnCheckTemp );
	mDP.Disable->Bind( wxEVT_CHECKBOX, OnCheckDict );
	mDP.CB_AH->Bind( wxEVT_CHECKBOX, OnCheckAuto );
	mDP.CB_AC->Bind( wxEVT_CHECKBOX, OnCheckAuto );
	mTP.CB_AS->Bind( wxEVT_CHECKBOX, OnCheckAuto );

	mDP.SL_AC->Bind( wxEVT_SCROLL_CHANGED, OnScroll );
	mDP.SL_AH->Bind( wxEVT_SCROLL_CHANGED, OnScroll );
	mTP.SL_AS->Bind( wxEVT_SCROLL_CHANGED, OnScroll );

	mDP.SC_AC->Bind( wxEVT_SPINCTRL, OnSpin );
	mDP.SC_AH->Bind( wxEVT_SPINCTRL, OnSpin );
	mTP.SC_AS->Bind( wxEVT_SPINCTRL, OnSpin );

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

void PreferencesFrame::UpdateDictionary()
{
	Config::sDictionary.UseGlobal = !mDP.Disable->GetValue();
	Config::sDictionary.Directory = mDP.DirPick->GetPath();
	if      ( mDP.OnAll->GetValue() ) Config::sDictionary.ApplyOn = DICT_ALL_DOCS;
	else if ( mDP.OnTemp->GetValue() ) Config::sDictionary.ApplyOn = DICT_TMP_DOCS;
	else if ( mDP.OnOpen->GetValue() ) Config::sDictionary.ApplyOn = DICT_OPN_DOCS;
}

void PreferencesFrame::UpdateNotebook()
{
	Config::sNotebook.Hide         = mGP.CB_NB_Hide->GetValue();
	Config::sNotebook.Orientation  = mGP.RB_NB_Top->GetValue();
	Config::sNotebook.FixedWidth   = mGP.CB_NB_Fixed->GetValue();
	Config::sNotebook.LockMove     = mGP.CB_NB_Lock->GetValue();
	Config::sNotebook.MiddleClose  = mGP.CB_NB_Middle->GetValue();
	Config::sNotebook.ShowCloseBtn = mGP.CB_NB_Close->GetValue();
	Config::sNotebook.CloseBtnOn   = mGP.RB_NB_OnAll->GetValue();
	
	if ( Config::sNotebook.Hide )  TextField::mNotebook->Show( false );
	else                           TextField::mNotebook->Show( true );
	
	TextField::mNotebook->SetWindowStyle( Config::GetNotebookStyle() );
}

void PreferencesFrame::UpdateDragDrop()
{
	auto isDragDrop = mGP.CB_DD->GetValue();
	if ( Config::sGeneral.UseDragDrop != isDragDrop )
	{
		Config::sGeneral.UseDragDrop = isDragDrop;
		for ( const auto& page : TextField::mPageData )
			page.TextField->DragAcceptFiles( isDragDrop );		
	}
}

void PreferencesFrame::UpdateAutocomp()
{
	auto isAutocomp = !mDP.CB_AC->GetValue();
	Config::sAutocomp.Param = mDP.SC_AC->GetValue();
	if ( Config::sAutocomp.Use != isAutocomp )
		Config::sAutocomp.Use = isAutocomp;
}

void PreferencesFrame::UpdateAutohigh()
{
	auto isAutohigh = !mDP.CB_AH->GetValue();
	Config::sAutohigh.Param = mDP.SC_AH->GetValue();
	if ( Config::sAutohigh.Use != isAutohigh )
	{
		Config::sAutohigh.Use = isAutohigh;
		if ( isAutohigh )
			if ( AutoThread::isDeploy( THREAD_HIGHLIGHTER ) ) AutoThread::Continue( THREAD_HIGHLIGHTER );
			else                                              AutoThread::DeployThread( THREAD_HIGHLIGHTER );
		else AutoThread::Pause( THREAD_HIGHLIGHTER );
	}
}

void PreferencesFrame::UpdateAutosave()
{
	auto isAutosave = !mTP.CB_AS->GetValue();
	Config::sAutosave.Param = mTP.SC_AS->GetValue() * 1000; //change from ms to sec
	if ( Config::sAutosave.Use != isAutosave )
	{
		Config::sAutosave.Use = isAutosave;
		if ( isAutosave )
			if ( AutoThread::isDeploy( THREAD_SAVER ) ) AutoThread::Continue( THREAD_SAVER );
			else                                        AutoThread::DeployThread( THREAD_SAVER );
		else AutoThread::Pause( THREAD_SAVER );
	}
}

void PreferencesFrame::UpdateStatbar()
{
	auto isStatbar = mGP.CB_SB->GetValue();
	if ( Config::sGeneral.UseStatbar != isStatbar )
	{
		Config::sGeneral.UseStatbar = isStatbar;
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

void PreferencesFrame::UpdateTemp()
{
	Config::sTemp.UseTemp = !mTP.Disable->GetValue();
	Config::sTemp.Directory = mTP.DirPick->GetPath();
	if      ( mTP.OnAll->GetValue() ) Config::sTemp.ApplyOn = TEMP_APPLY_ALL;
	else if ( mTP.OnNew->GetValue() ) Config::sTemp.ApplyOn = TEMP_APPLY_NEW;
}

void PreferencesFrame::UpdatePanelAuto( int id )
{
	if ( id == WND_ID_SAUTOHIGH )
	{
		if ( mDP.CB_AH->GetValue() )
		{
			mDP.SL_AH->Disable();
			mDP.SC_AH->Disable();
		}
		else
		{
			mDP.SL_AH->Enable();
			mDP.SC_AH->Enable();
		}
	}
	else if ( id == WND_ID_SAUTOCOMP )
	{
		if ( mDP.CB_AC->GetValue() )
		{
			mDP.SL_AC->Disable();
			mDP.SC_AC->Disable();
		}
		else
		{
			mDP.SL_AC->Enable();
			mDP.SC_AC->Enable();
		}
	}
	else if ( id == WND_ID_SAUTOSAVE )
	{
		if ( mTP.CB_AS->GetValue() )
		{
			mTP.SL_AS->Disable();
			mTP.SC_AS->Disable();
		}
		else
		{
			mTP.SL_AS->Enable();
			mTP.SC_AS->Enable();
		}
	}
}

void PreferencesFrame::OnCheckTemp( wxCommandEvent& event )
{
	if ( mTP.Disable->GetValue() )
	{
		mTP.OnAll->Disable();
		mTP.OnNew->Disable();
		mTP.DirPick->Disable();
		mTP.CB_AS->Disable();
		mTP.CB_AS->SetValue( true );
	}
	else
	{
		mTP.OnAll->Enable();
		mTP.OnNew->Enable();
		mTP.DirPick->Enable();
		mTP.CB_AS->Enable();
		mTP.CB_AS->SetValue( false );
	}
	UpdatePanelAuto( WND_ID_SAUTOSAVE );
}

void PreferencesFrame::OnCheckDict( wxCommandEvent& event )
{
	if ( mDP.Disable->GetValue() )
	{
		mDP.OnAll->Disable();
		mDP.OnTemp->Disable();
		mDP.OnOpen->Disable();
		mDP.DirPick->Disable();
	}
	else
	{
		mDP.OnAll->Enable();
		mDP.OnTemp->Enable();
		mDP.OnOpen->Enable();
		mDP.DirPick->Enable();
	}
}

void PreferencesFrame::OnCheckAuto( wxCommandEvent& event )
{
	auto cb = (wxCheckBox*) event.GetEventObject();
	auto id = cb->GetId();
	UpdatePanelAuto( id );
}

void PreferencesFrame::OnCheckHide( wxCommandEvent& event )
{
	if ( mGP.CB_NB_Hide->GetValue() )
	{
		mGP.RB_NB_Top->Disable();
		mGP.RB_NB_Bot->Disable();
	}
	else
	{	
		mGP.RB_NB_Top->Enable();
		mGP.RB_NB_Bot->Enable();
	}
}

void PreferencesFrame::OnCheckShowClose( wxCommandEvent& event )
{
	if ( mGP.CB_NB_Close->GetValue() )
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
	UpdateDictionary();
	UpdateTemp();
	Config::sGeneral.UseSplash = mGP.CB_SS->GetValue();

	mFrame->Show( false );
}

void PreferencesFrame::OnCancel( wxCommandEvent& event )
{
	Update();
	mFrame->Show( false );
}

void PreferencesFrame::OnLocalization( wxCommandEvent& event )
{
	PROFILE_FUNC();
	auto sel = mGP.LocalizationCB->GetSelection();
	if ( sel == wxID_NONE ) return;

	if ( Config::sGeneral.LanguageID != sel )
	{
		Config::sGeneral.LanguageID = sel;
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
	auto id = slider->GetId();
	if      ( id == WND_ID_SAUTOHIGH ) mDP.SC_AH->SetValue( slider->GetValue() );
	else if ( id == WND_ID_SAUTOCOMP ) mDP.SC_AC->SetValue( slider->GetValue() );
	else if ( id == WND_ID_SAUTOSAVE ) mTP.SC_AS->SetValue( slider->GetValue() );
}

void PreferencesFrame::OnSpin( wxSpinEvent& event )
{
	auto spin = (wxSpinCtrl*) event.GetEventObject();
	auto id = spin->GetId();
	if      ( id == WND_ID_SAUTOHIGH ) mDP.SL_AH->SetValue( spin->GetValue() );
	else if ( id == WND_ID_SAUTOCOMP ) mDP.SL_AC->SetValue( spin->GetValue() );
	else if ( id == WND_ID_SAUTOSAVE ) mTP.SL_AS->SetValue( spin->GetValue() );
}