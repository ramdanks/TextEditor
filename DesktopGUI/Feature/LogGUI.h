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
	static LogGUI* sLogGUI;
	static char sCharBuf[256];

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

// format character buffer for multipurpose use
#define SET_STR_BUF( msg, ... )     snprintf( LogGUI::sCharBuf, sizeof( LogGUI::sCharBuf ), msg, __VA_ARGS__ )

// logging macros
#ifdef _DIST		
#define LOG_CONSOLE( l, msg )
#define LOG_CONSOLE_FORMAT( l, msg, ... )
#else
#define LOG_CONSOLE( l, msg )              LogGUI::sLogGUI->LogConsole( l, msg )
#define LOG_CONSOLE_FORMAT( l, msg, ... )  SET_STR_BUF( msg, __VA_ARGS__ ); LOG_CONSOLE( l, LogGUI::sCharBuf )
#endif									   
#define LOG_FILE( l, msg )		           LogGUI::sLogGUI->LogFile( l, msg )
#define LOG_FILE_FORMAT( l, msg, ... )     SET_STR_BUF( msg, __VA_ARGS__ ); LOG_FILE( l, LogGUI::sCharBuf )
										   
#define LOG_ALL( l, msg )                  LOG_FILE( l, msg ); LOG_CONSOLE( l, msg )
#define LOG_ALL_FORMAT( l, msg, ... )      SET_STR_BUF( msg, __VA_ARGS__ ); LOG_FILE( l, LogGUI::sCharBuf ); LOG_CONSOLE( l, LogGUI::sCharBuf )