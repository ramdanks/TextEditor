#include "LogGUI.h"
#include "../../Utilities/Filestream.h"

wxBEGIN_EVENT_TABLE( LogGUI, wxFrame )
EVT_CLOSE( LogGUI::OnClose )
wxEND_EVENT_TABLE()

LogGUI* LogGUI::LGUI;

LogGUI::LogGUI( wxWindow* parent )
	: wxFrame( parent, wxID_ANY, "Mémoriser - Debug Console", wxDefaultPosition, wxSize( 600, 300 ) )
{
	std::vector<uint8_t> Format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG };
	mLog = new Util::Logging( Format );
	mDebugTextField = new wxStyledTextCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 600, 300 ) );
	mDebugTextField->SetEditable( false );

	Filestream::Create_Directories( "log" );
	mLogFile = LOG_FILEPATH;
	mErrFile = ERR_FILEPATH;
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
}

void LogGUI::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}