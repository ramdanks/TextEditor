#pragma once
#include <vector>
#include <string>

enum severity
{ LV_NONE, LV_INFO, LV_TRACE, LV_WARN, LV_ERROR, LV_FATAL };

namespace Util
{

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

		void Log_File( severity level, const std::string& str, const std::string& path );
		std::string Log_String( severity level, std::string&& str );
		std::string Log_String( severity level, const std::string& str );
	};
}