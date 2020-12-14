#include "Logging.h"
#include "Timer.h"
#include "Filestream.h"

void Util::Logging::Log_File( severity level, const std::string& str, const std::string& path )
{
	Filestream::Append_Text( Log_String( level, str ), path );
}

std::string Util::Logging::Log_String( severity level, std::string&& str )
{
	static char buffer[100];
	std::string combined;
	if ( mFormat.empty() ) combined = std::move( str );
	else
	{
		for ( auto format : this->mFormat )
		{
			if ( format == FORMAT_LEVEL && level != LV_NONE )
			{
				if      ( LV_INFO )  combined += "[INFO]";
				else if ( LV_TRACE ) combined += "[TRACE]";
				else if ( LV_WARN )  combined += "[WARN]";
				else if ( LV_ERROR ) combined += "[ERROR]";
				else if ( LV_FATAL ) combined += "[FATAL]";
			}
			else if ( format == FORMAT_MSG )    combined += std::move( str );
			else if ( format == FORMAT_SPACE )  combined += "  ";
			else if ( format == FORMAT_TIME )   combined += "[" + Util::Timer::Get_Current_Time() + "]";
		}
		combined += "\n";
	}
	return combined;
}

std::string Util::Logging::Log_String( severity level, const std::string& str )
{
	std::string combined;
	if ( this->mFormat.empty() ) combined = str;
	else
	{
		for ( auto format : this->mFormat )
		{
			if ( format == FORMAT_LEVEL && level != LV_NONE )
			{		
				if      ( LV_INFO  ) combined += "[INFO]";
				else if ( LV_TRACE ) combined += "[TRACE]";
				else if ( LV_WARN  ) combined += "[WARN]";
				else if ( LV_ERROR ) combined += "[ERROR]";
				else if ( LV_FATAL ) combined += "[FATAL]";
			}
			else if ( format == FORMAT_MSG )    combined += str;
			else if ( format == FORMAT_SPACE )  combined += "  ";
			else if ( format == FORMAT_TIME )   combined += "[" + Util::Timer::Get_Current_Time() + "]";
		}
		combined += "\n";
	}
	return combined;
}