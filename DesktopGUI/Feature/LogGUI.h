#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include "../../Utilities/Logging.h"

#define ERR_FILEPATH	"log/AppError.txt"
#define LOG_FILEPATH	"log/AppLog.txt"
#define TO_STR(data)    std::to_string(data)
#define CV_STR(str)     std::string(str)

class LogGUI : public wxFrame
{
public:
	//use this everywhere
	static LogGUI* LGUI;

	LogGUI( wxWindow* parent, bool GUI = true );
	
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
#define LOGCONSOLE(l,msg)   LogGUI::LGUI->LogConsole(l,msg)
#ifdef _DIST		
	#define LOGALL(l,msg)       LOGFILE(l,msg)
#else
	#define LOGALL(l,msg)       LOGFILE(l,msg); LOGCONSOLE(l,msg)
#endif