#include "LogGUI.h"
#include "../../Utilities/Filestream.h"
#include "Config.h"

wxBEGIN_EVENT_TABLE( LogGUI, wxFrame )
EVT_CLOSE( LogGUI::OnClose )
wxEND_EVENT_TABLE()

LogGUI* LogGUI::LGUI;

LogGUI::LogGUI( wxWindow* parent, bool GUI )
	: wxFrame( parent, wxID_ANY, DEBUG_NAME, wxDefaultPosition, wxSize( 600, 300 ) )
{
	Filestream::Create_Directories( "log" );
	mLogFile = LOG_FILEPATH;
	mErrFile = ERR_FILEPATH;

	std::vector<uint8_t> Format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG };
	mLog = new Util::Logging( Format );

	if ( GUI )
	{
		mDebugTextField = new wxStyledTextCtrl( this );
		mDebugTextField->SetEditable( false );
		mDebugTextField->SetScrollWidth( 1 );
		mDebugTextField->StyleSetForeground( wxSTC_STYLE_DEFAULT, *wxWHITE ); //foreground such as text
		mDebugTextField->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 35, 35, 35 ) ); //background for field
		mDebugTextField->StyleClearAll();
		mDebugTextField->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color
	}
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

void LogGUI::LogFile( Util::LogLevel l, const std::string& msg )
{
	if ( !mLogFile.empty() )
		mLog->Log_File( l, msg, mLogFile );
}

void LogGUI::LogConsole( Util::LogLevel l, const std::string& msg )
{
	if ( mDebugTextField == nullptr ) return;
	mDebugTextField->SetEditable( true );
	mDebugTextField->AppendText( mLog->Log_String( l, msg ) );
	mDebugTextField->SetEditable( false );
	mDebugTextField->ScrollToEnd();
}

void LogGUI::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}