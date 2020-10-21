#pragma once
#include <vector>
#include <string>

namespace Util
{
	typedef std::string LogLevel;
	typedef uint8_t LogFormat;

	#define LEVEL_INFO "INFO"
	#define LEVEL_TRACE "TRACE"
	#define LEVEL_WARN  "WARN"
	#define LEVEL_ERROR "ERROR"
	#define LEVEL_FATAL "FATAL"

	#define FORMAT_SPACE 0
	#define FORMAT_LEVEL 1
	#define FORMAT_TIME 2
	#define FORMAT_MSG 3

	class Logging
	{
		std::vector<LogFormat> mFormat = std::vector<LogFormat>();

	public:
		Logging( const std::vector<LogFormat>& format ) : mFormat(format) {}
		void Set_Format( const std::vector<LogFormat>& format ) { mFormat = format; }

		void Log( LogLevel level, const std::string& str );
		void Log_File( LogLevel level, const std::string& str, const std::string& path );
		std::string Log_String( LogLevel level, const std::string& str );
	};
}