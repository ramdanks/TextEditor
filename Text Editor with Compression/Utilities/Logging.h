#pragma once
#include <vector>
#include <string>

typedef enum LLevel
{
	INFO, TRACE, WARN, ERROR, FATAL
} LLevel;

typedef enum LFormat
{
	LEVEL, TIME, MSG
} LFormat;

//singleton pattern
class Logging
{
	std::vector<LFormat> mFormat;

public:
	Logging( const std::vector<LFormat>& format ) : mFormat(format) {}

	void Set_Format(const std::vector<LFormat>& format );
	void Log( LLevel lv, const std::string& string );
	void Log_File( LLevel lv, const std::string& string, const std::string& path );
};