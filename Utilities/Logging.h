#pragma once
#include <vector>
#include <string>

namespace Util
{
	typedef std::string LogLevel;

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
		std::vector<uint8_t> mFormat = std::vector<uint8_t>();

	public:
		Logging( const std::vector<uint8_t>& format ) : mFormat(format) {}
		void Set_Format( const std::vector<uint8_t>& format ) { mFormat = format; }

		void Log( LogLevel level, const std::string& str );
		void Log_File( LogLevel level, const std::string& str, const std::string& path );
		std::string Log_String( LogLevel level, const std::string& str );
	};
}