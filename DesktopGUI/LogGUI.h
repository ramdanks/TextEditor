#pragma once
#include "../Utilities/Logging.h"
#include <wx/wxprec.h>
#include <wx/stc/stc.h>

#define ERR_FILEPATH	"log/AppError.txt"
#define LOG_FILEPATH	"log/AppLog.txt"

class LogGUI : public wxFrame
{
public:
	static LogGUI* LGUI; 
	LogGUI( wxWindow* parent );
	
	void SetLogFile( const std::string& filepath );
	void SetErrFile( const std::string& filepath );
	void SetLogFormat( const std::vector<uint8_t>& format );

	void LogFile( Util::LogLevel l, const std::string& msg );
	void LogConsole( Util::LogLevel l, const std::string& msg );

private:
	void OnClose( wxCloseEvent& event );
	wxDECLARE_EVENT_TABLE();
	
	std::string mErrFile;
	std::string mLogFile;
	Util::Logging* mLog;
	wxStyledTextCtrl* mDebugTextField;
};

#define LOGFILE(l,msg)		LogGUI::LGUI->LogFile(l,msg)
#if defined( _DEBUG )		
#define LOGCONSOLE(l,msg)   LogGUI::LGUI->LogConsole(l,msg)
#else
#define LOGCONSOLE(l,msg)
#endif
#define LOGALL(l,msg)       LOGFILE(l,msg); LOGCONSOLE(l,msg)