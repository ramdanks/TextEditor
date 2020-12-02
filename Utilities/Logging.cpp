#include "Logging.h"
#include "Timer.h"
#include "Filestream.h"

void Util::Logging::Log( severity level, const std::string& str )
{
	printf( "%s\n", Log_String( level, str ).c_str() );
}

void Util::Logging::Log_File( severity level, const std::string& str, const std::string& path )
{
	Filestream::Append_Text( Log_String( level, str ), path );
}

std::string Util::Logging::Log_String( severity level, const std::string& str )
{
	static char buffer[100];
	std::string combined;
	if ( this->mFormat.empty() ) combined = str;
	else
	{
		for ( auto format : this->mFormat )
		{
			if ( format == FORMAT_LEVEL )
			{		
				snprintf( buffer, 100, "[%s]", level );
				combined += buffer;
			}
			else if ( format == FORMAT_MSG )    combined += str;
			else if ( format == FORMAT_SPACE )  combined += "  ";
			else if ( format == FORMAT_TIME )   combined += "[" + Util::Timer::Get_Current_Time() + "]";
		}
		combined += "\n";
	}
	return combined;
}