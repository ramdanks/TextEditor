#include "LogGUI.h"
#include "Config.h"

char              LogGUI::sCharBuf[256];
wxFrame*          LogGUI::mFrame;
std::string       LogGUI::mErrFile;
std::string       LogGUI::mLogFile;
Util::Logging*    LogGUI::mLog;
wxAuiNotebook*    LogGUI::mNotebook;
wxStyledTextCtrl* LogGUI::mDebugTC;
wxStyledTextCtrl* LogGUI::mFunctionTC;
wxStyledTextCtrl* LogGUI::mThreadTC;

void LogGUI::Init( wxWindow* parent, bool GUI )
{
	if ( !wxDir::Exists( Config::sAppPath + LOG_DIR_RELATIVE ) )
		wxDir::Make( Config::sAppPath + LOG_DIR_RELATIVE );

	mLogFile = Config::sAppPath + LOG_PATH_RELATIVE;
	mErrFile = Config::sAppPath + ERR_PATH_RELATIVE;

	std::vector<uint8_t> Format = { FORMAT_TIME, FORMAT_SPACE, FORMAT_LEVEL, FORMAT_SPACE, FORMAT_MSG };
	mLog = new Util::Logging( Format );

	if ( GUI )
	{
		mFrame = new wxFrame( parent, wxID_ANY, DEBUG_NAME, wxDefaultPosition, wxSize( 700, 400 ) );
		mFrame->SetIcon( wxICON( ICON_BUG ) );

		mFrame->CreateStatusBar();
		mFrame->SetStatusText( "Debugger will Catch any Event" );

		auto panel = new wxPanel( mFrame );
		auto panelsizer = new wxBoxSizer( wxVERTICAL );
		auto btnsizer = new wxBoxSizer( wxHORIZONTAL );

		auto btnClear = new wxButton( panel, 1, "Clear" );
		auto btnSave = new wxButton( panel, 2, "Save Log" );
		btnsizer->Add( btnClear, 0, wxRIGHT, 5 );
		btnsizer->Add( btnSave, 0 );

		int style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_SCROLL_BUTTONS;
		mNotebook = new wxAuiNotebook( panel, -1, wxDefaultPosition, wxDefaultSize, style );

		mDebugTC = new wxStyledTextCtrl( mNotebook );
		mDebugTC->SetEditable( false );
		mDebugTC->StyleSetForeground( wxSTC_STYLE_DEFAULT, wxColour( 225, 225, 225 ) ); //foreground such as text
		mDebugTC->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 35, 35, 35 ) ); //background for field
		mDebugTC->StyleClearAll();
		mDebugTC->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color

		mFunctionTC = new wxStyledTextCtrl( mNotebook );
		mFunctionTC->SetEditable( false );
		mFunctionTC->StyleSetForeground( wxSTC_STYLE_DEFAULT, wxColour( 225, 225, 225 ) ); //foreground such as text
		mFunctionTC->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 35, 35, 35 ) ); //background for field
		mFunctionTC->StyleClearAll();
		mFunctionTC->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color

		mThreadTC = new wxStyledTextCtrl( mNotebook );
		mThreadTC->SetEditable( false );
		mThreadTC->StyleSetForeground( wxSTC_STYLE_DEFAULT, wxColour( 225, 225, 225 ) ); //foreground such as text
		mThreadTC->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 35, 35, 35 ) ); //background for field
		mThreadTC->StyleClearAll();
		mThreadTC->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color

		mNotebook->AddPage( mDebugTC, "Debugging" );
		mNotebook->AddPage( mFunctionTC, "Function" );
		mNotebook->AddPage( mThreadTC, "Thread" );

		panelsizer->Add( -1, 5 );
		panelsizer->Add( btnsizer, 0, wxLEFT | wxRIGHT, 10 );
		panelsizer->Add( -1, 5 );
		panelsizer->Add( mNotebook, 1, wxEXPAND );
		panel->SetSizer( panelsizer );

		mFrame->Bind( wxEVT_CLOSE_WINDOW, OnClose );
		btnSave->Bind( wxEVT_BUTTON, OnSaveLog );
		btnClear->Bind( wxEVT_BUTTON, OnClear );
	}
}

void LogGUI::SetParent( wxWindow* parent )
{
	mFrame->SetParent( parent );
}

void LogGUI::SetLogFile( const std::string& filepath )
{
	mLogFile = filepath;
}

void LogGUI::SetErrFile( const std::string& filepath )
{
	mErrFile = filepath;
}

void LogGUI::SetLogFormat( const std::vector<uint8_t>& format )
{
	mLog->Set_Format( format );
}

void LogGUI::LogFile( severity l, std::string&& msg )
{
	if ( !mLogFile.empty() )
		mLog->Log_File( l, std::move( msg ), mLogFile );
}

void LogGUI::LogFile( severity l, const std::string& msg )
{
	if ( !mLogFile.empty() )
		mLog->Log_File( l, msg, mLogFile );
}

void LogGUI::LogConsoleDebug( severity l, std::string&& msg )
{
	AppendTC( mDebugTC, mLog->Log_String( l, std::move( msg ) ) );
}

void LogGUI::LogConsoleDebug( severity l, const std::string& msg )
{
	AppendTC( mDebugTC, mLog->Log_String( l, msg ) );
}

void LogGUI::LogConsoleFunction( severity l, std::string&& msg )
{
	AppendTC( mFunctionTC, mLog->Log_String( l, std::move( msg ) ) );
}

void LogGUI::LogConsoleFunction( severity l, const std::string& msg )
{
	AppendTC( mFunctionTC, mLog->Log_String( l, msg ) );
}

void LogGUI::LogConsoleThread( severity l, std::string&& msg )
{
	AppendTC( mThreadTC, mLog->Log_String( l, std::move( msg ) ) );
}

void LogGUI::LogConsoleThread( severity l, const std::string& msg )
{
	AppendTC( mThreadTC, mLog->Log_String( l, msg ) );
}

void LogGUI::AppendTC( wxStyledTextCtrl* stc, std::string&& msg )
{
	if ( stc == nullptr ) return;
	stc->SetEditable( true );
	stc->AppendText( std::move( msg ) );
	stc->SetEditable( false );
	stc->ScrollToEnd();
}

void LogGUI::AppendTC( wxStyledTextCtrl* stc, const std::string& msg )
{
	if ( stc == nullptr ) return;
	stc->SetEditable( true );
	stc->AppendText( msg );
	stc->SetEditable( false );
	stc->ScrollToEnd();
}

void LogGUI::OnClear( wxCommandEvent& event )
{
	auto sel = mNotebook->GetSelection();
	if ( sel == wxNOT_FOUND ) return;

	if ( sel == 0 )
	{
		mDebugTC->SetEditable( true );
		mDebugTC->ClearAll();
		mDebugTC->SetEditable( false );
	}
	else if ( sel == 1 )
	{
		mFunctionTC->SetEditable( true );
		mFunctionTC->ClearAll();
		mFunctionTC->SetEditable( false );
	}
	else if ( sel == 2 )
	{
		mThreadTC->SetEditable( true );
		mThreadTC->ClearAll();
		mThreadTC->SetEditable( false );
	}
}

void LogGUI::OnSaveLog( wxCommandEvent& event )
{
}

void LogGUI::OnClose( wxCloseEvent& event )
{
	mFrame->Show( false );
}

ProfileFunc::ProfileFunc( const char* func, severity s, TimerPoint time )
	: mTimer( func, time, false ), mSeverity( s )
{}

ProfileFunc::ProfileFunc( const std::string& func, severity s, TimerPoint time )
	: mTimer( func.c_str(), time, false ), mSeverity( s )
{}

ProfileFunc::~ProfileFunc()
{
	LogGUI::LogConsoleFunction( mSeverity, mTimer.Toc_String() );
}