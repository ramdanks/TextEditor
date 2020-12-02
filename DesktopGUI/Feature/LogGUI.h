#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include <wx/aui/auibook.h>
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

	void LogFile( Util::severity l, const std::string& msg );

	void LogConsoleDebug( Util::severity l, const std::string& msg );
	void LogConsoleFunction( Util::severity l, const std::string& msg );
	void LogConsoleThread( Util::severity l, const std::string& msg );

private:
	void AppendTC( wxStyledTextCtrl* stc, const std::string& msg );
	void OnClear( wxCommandEvent& event );
	void OnSaveLog( wxCommandEvent& event );
	void OnClose( wxCloseEvent& event );
	wxDECLARE_EVENT_TABLE();

	std::string mErrFile;
	std::string mLogFile;
	Util::Logging* mLog;
	wxAuiNotebook* mNotebook;
	wxStyledTextCtrl* mDebugTC;
	wxStyledTextCtrl* mFunctionTC;
	wxStyledTextCtrl* mThreadTC;
};

// format character buffer for multipurpose use
#define SET_STR_BUF( msg, ... )     snprintf( LogGUI::sCharBuf, sizeof( LogGUI::sCharBuf ), msg, __VA_ARGS__ )

// logging macros
#define ENABLELOG 1
#if ENABLELOG
#ifndef _DIST
	#define LOG_DEBUG(l,msg)                LogGUI::sLogGUI->LogConsoleDebug(l,msg )
	#define LOG_DEBUG_FORMAT(l,msg,...)     SET_STR_BUF(msg,__VA_ARGS__); LOG_DEBUG(l,LogGUI::sCharBuf)
	#define LOG_FUNCTION(l,msg)             LogGUI::sLogGUI->LogConsoleFunction(l,msg)
	#define LOG_FUNCTION_FORMAT(l,msg,...)  SET_STR_BUF(msg,__VA_ARGS__); LOG_FUNCTION(l,LogGUI::sCharBuf)
	#define LOG_THREAD(l,msg)               LogGUI::sLogGUI->LogConsoleThread(l,msg)
	#define LOG_THREAD_FORMAT(l,msg,...)    SET_STR_BUF(msg,__VA_ARGS__); LOG_THREAD(l,LogGUI::sCharBuf)
#elif ENABLELOG
	#define LOG_DEBUG(l,msg)              
	#define LOG_DEBUG_FORMAT(l,msg,...)   
	#define LOG_FUNCTION(l,msg)           
	#define LOG_FUNCTION_FORMAT(l,msg,...)
	#define LOG_THREAD(l,msg)             
	#define LOG_THREAD_FORMAT(l,msg,...)  
#endif
	#define LOG_FILE(l,msg )		     LogGUI::sLogGUI->LogFile(l,msg)
	#define LOG_FILE_FORMAT(l,msg,...)   SET_STR_BUF(msg,__VA_ARGS__); LOG_FILE(l,LogGUI::sCharBuf)									   
	#define LOG_ALL(l,msg)               LOG_FILE(l,msg); LOG_DEBUG(l,msg)
	#define LOG_ALL_FORMAT(l,msg,...)    SET_STR_BUF(msg,__VA_ARGS__); LOG_FILE(l,LogGUI::sCharBuf); LOG_DEBUG(l,LogGUI::sCharBuf)
#else	
	#define LOG_FILE(l,msg )		  
	#define LOG_FILE_FORMAT(l,msg,...)
	#define LOG_ALL(l,msg)            
	#define LOG_ALL_FORMAT(l,msg,...)
#endif