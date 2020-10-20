#include "Err.h"

namespace Util
{
	void Err::Log( LogLevel lv, const std::string& filepath )
	{
		std::vector<LogFormat> format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_MSG };
		Logging log( format );
		log.Log_File( lv, this->message, filepath );
	}

	std::string Err::Seek() const
	{
		return this->message;
	}
}