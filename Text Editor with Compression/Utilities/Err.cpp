#include "Err.h"

void Err::Log( LLevel lv, const std::string& filepath )
{
	std::vector<LFormat> format = { LEVEL, TIME, MSG };
	Logging log( format );
	log.Log_File( lv, this->message, filepath );
}

std::string Err::Seek() const
{
	return this->message;
}