#include "Logging.h"
#include "Timer.h"
#include "Filestream.h"

constexpr const char* levelv[] = { "INFO", "TRACE", "WARN", "ERROR", "FATAL" };

void Logging::Set_Format( const std::vector<LFormat>& format )
{
	this->mFormat = format;
}

void Logging::Log( LLevel lv, const std::string& string )
{
	if ( this->mFormat.empty() )
	{
		printf( "[%s]:%s\n", levelv[lv], string.c_str() );
	}
	else
	{
		for ( auto format : this->mFormat )
		{
			if ( format == LFormat::LEVEL )		printf( "[%s]", levelv[lv] );
			else if ( format == LFormat::MSG )	printf( ":%s", string.c_str() );
			else if ( format == LFormat::TIME ) printf( "{%s}", Timer::Get_Current_Time().c_str() );
		}
		printf( "\n" );
	}
}

void Logging::Log_File( LLevel lv, const std::string& string, const std::string& path )
{
	std::string combined;

	if ( this->mFormat.empty() ) combined = string;
	else
	{
		for ( auto format : this->mFormat )
		{
			if ( format == LFormat::LEVEL )		combined += "[" + std::string( levelv[lv] ) + "]";
			else if ( format == LFormat::MSG )	combined += ":" + string;
			else if ( format == LFormat::TIME ) combined += "{" + Timer::Get_Current_Time() + "}";
		}
		combined += "\n";
	}
	Filestream::Append_Text( combined, path );
}