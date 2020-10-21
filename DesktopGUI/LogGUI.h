#pragma once
#include "../Utilities/Logging.h"
#include <wx/stc/stc.h>

#define ERR_FILEPATH	"log/AppError.txt"
#define LOG_FILEPATH	"log/AppLog.txt"

class LogGUI
{
public:
	static void SetDebugTextField( wxStyledTextCtrl* debugTextField );
	static void SetLog( std::vector<Util::LogFormat> format );

	static Util::Logging* GetLog();
	static void PrintDebug( Util::LogLevel l, const std::string& str );

private:
	static Util::Logging* sLog;
	static wxStyledTextCtrl* sDebugTextField;
};

#define LOGFILE(l,msg,file)  LogGUI::GetLog()->Log_File(l,msg,file)
#if defined( _DEBUG )
#define LOGCONSOLE(l,msg)    LogGUI::PrintDebug(l,msg)
#else
#define LOGCONSOLE(l,msg)
#endif
#define LOGALL(l,msg,file)   LOGFILE(l,msg,file); LOGCONSOLE(l,msg)