#pragma once
#include "../Frame/AppFrame.h"

#define ERR_FILEPATH	"log/AppError.txt"
#define LOG_FILEPATH	"log/AppLog.txt"
#define TO_STR(data)    std::to_string(data)
#define CV_STR(str)     std::string(str)

class LogGUI
{
public:
	static char sCharBuf[256];

	LogGUI() = delete;
	LogGUI( const LogGUI& other ) = delete;	
	static void Init( wxWindow* parent, bool GUI = true );
	static void SetParent( wxWindow* parent );
	
	static void SetLogFile( const std::string& filepath );
	static void SetErrFile( const std::string& filepath );
	static void SetLogFormat( const std::vector<uint8_t>& format );

	static void LogFile( severity l, std::string&& msg );
	static void LogFile( severity l, const std::string& msg );
	static void LogConsoleDebug( severity l, std::string&& msg );
	static void LogConsoleDebug( severity l, const std::string& msg );
	static void LogConsoleFunction( severity l, std::string&& msg );
	static void LogConsoleFunction( severity l, const std::string& msg );
	static void LogConsoleThread( severity l, std::string&& msg );
	static void LogConsoleThread( severity l, const std::string& msg );

private:

	static void AppendTC( wxStyledTextCtrl* stc, std::string&& msg );
	static void AppendTC( wxStyledTextCtrl* stc, const std::string& msg );

	static void OnClear( wxCommandEvent& event );
	static void OnSaveLog( wxCommandEvent& event );
	static void OnClose( wxCloseEvent& event );

	static wxFrame* mFrame;
	static std::string mErrFile;
	static std::string mLogFile;
	static Util::Logging* mLog;
	static wxAuiNotebook* mNotebook;
	static wxStyledTextCtrl* mDebugTC;
	static wxStyledTextCtrl* mFunctionTC;
	static wxStyledTextCtrl* mThreadTC;

	friend class AppFrame;
};

struct ProfileFunc
{
public:
	ProfileFunc( std::string&& func );
	ProfileFunc( const std::string& func );
	ProfileFunc( const std::string& func, severity s );
	ProfileFunc( const std::string& func, severity s, TimerPoint tp );
	~ProfileFunc();

private:
	Util::Timer mTimer;
	severity mSeverity;
};

// format character buffer for multipurpose use
#define SET_STR_BUF( msg, ... )     snprintf( LogGUI::sCharBuf, sizeof( LogGUI::sCharBuf ), msg, __VA_ARGS__ )

// logging macros
#define ENABLELOG 1
#if ENABLELOG
#ifndef _DIST
	#define LOG_DEBUG(l,msg)                LogGUI::LogConsoleDebug(l,msg)
	#define LOG_DEBUG_FORMAT(l,msg,...)     SET_STR_BUF(msg,__VA_ARGS__); LOG_DEBUG(l,LogGUI::sCharBuf)
	#define LOG_FUNCTION(l,msg)             LogGUI::LogConsoleFunction(l,msg)
	#define LOG_FUNCTION_FORMAT(l,msg,...)  SET_STR_BUF(msg,__VA_ARGS__); LOG_FUNCTION(l,LogGUI::sCharBuf)
	#define LOG_THREAD(l,msg)               LogGUI::LogConsoleThread(l,msg)
	#define LOG_THREAD_FORMAT(l,msg,...)    SET_STR_BUF(msg,__VA_ARGS__); LOG_THREAD(l,LogGUI::sCharBuf)
#elif ENABLELOG
	#define LOG_DEBUG(l,msg)              
	#define LOG_DEBUG_FORMAT(l,msg,...)   
	#define LOG_FUNCTION(l,msg)           
	#define LOG_FUNCTION_FORMAT(l,msg,...)
	#define LOG_THREAD(l,msg)             
	#define LOG_THREAD_FORMAT(l,msg,...)  
#endif
	#define LOG_FILE(l,msg )		     LogGUI::LogFile(l,msg)
	#define LOG_FILE_FORMAT(l,msg,...)   SET_STR_BUF(msg,__VA_ARGS__); LOG_FILE(l,LogGUI::sCharBuf)									   
	#define LOG_ALL(l,msg)               LOG_FILE(l,msg); LOG_DEBUG(l,msg)
	#define LOG_ALL_FORMAT(l,msg,...)    SET_STR_BUF(msg,__VA_ARGS__); LOG_FILE(l,LogGUI::sCharBuf); LOG_DEBUG(l,LogGUI::sCharBuf)
#else	
	#define LOG_FILE(l,msg )		  
	#define LOG_FILE_FORMAT(l,msg,...)
	#define LOG_ALL(l,msg)            
	#define LOG_ALL_FORMAT(l,msg,...)
#endif

#define PROFILE_FUNC() ProfileFunc obj(__FUNCSIG__)