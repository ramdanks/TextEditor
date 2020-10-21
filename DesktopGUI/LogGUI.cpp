#include "LogGUI.h"

Util::Logging* LogGUI::sLog;
wxStyledTextCtrl* LogGUI::sDebugTextField;

void LogGUI::SetDebugTextField( wxStyledTextCtrl* debugTextField )
{
	LogGUI::sDebugTextField = debugTextField;
}

void LogGUI::SetLog( std::vector<Util::LogFormat> format )
{
	LogGUI::sLog = new Util::Logging( format );
}

Util::Logging* LogGUI::GetLog()
{
	return LogGUI::sLog;
}

void LogGUI::PrintDebug( Util::LogLevel l, const std::string& str )
{
	if ( LogGUI::sDebugTextField == nullptr ) return;
	LogGUI::sDebugTextField->SetEditable( true );
	LogGUI::sDebugTextField->AppendText( sLog->Log_String( l, str ) );
	LogGUI::sDebugTextField->SetEditable( false );
}
