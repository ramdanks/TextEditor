#include "LogGUI.h"
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Timer.h"
#include "Config.h"

wxBEGIN_EVENT_TABLE( LogGUI, wxFrame )
EVT_CLOSE( LogGUI::OnClose )
EVT_BUTTON( 1, LogGUI::OnClear )
EVT_BUTTON( 2, LogGUI::OnSaveLog )
wxEND_EVENT_TABLE()

LogGUI* LogGUI::sLogGUI;
char LogGUI::sCharBuf[256];

LogGUI::LogGUI( wxWindow* parent, bool GUI )
	: wxFrame( parent, wxID_ANY, DEBUG_NAME, wxDefaultPosition, wxSize( 800, 400 ) )
{
	Filestream::Create_Directories( "log" );

	mLogFile = LOG_FILEPATH;
	mErrFile = ERR_FILEPATH;

	std::vector<uint8_t> Format = { FORMAT_TIME, FORMAT_SPACE, FORMAT_LEVEL, FORMAT_SPACE, FORMAT_MSG };
	mLog = new Util::Logging( Format );

	if ( GUI )
	{
		this->CreateStatusBar();
		this->SetStatusText( "Debugger will Catch any Event" );

		auto panel = new wxPanel( this );
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
	}
	this->SetIcon( wxICON( ICON_BUG ) );
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

void LogGUI::LogConsoleDebug( Util::severity l, const std::string& msg )
{
	AppendTC( mDebugTC, mLog->Log_String( l, msg ) );
}

void LogGUI::LogConsoleFunction( Util::severity l, const std::string& msg )
{
	AppendTC( mFunctionTC, mLog->Log_String( l, msg ) );
}

void LogGUI::LogConsoleThread( Util::severity l, const std::string& msg )
{
	AppendTC( mThreadTC, mLog->Log_String( l, msg ) );
}

void LogGUI::LogFile( Util::severity l, const std::string& msg )
{
	if ( !mLogFile.empty() )
		mLog->Log_File( l, msg, mLogFile );
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
	this->Show( false );
}