#pragma once
#include <string>
#include <algorithm>

namespace Util
{
	struct sFindAttrib
	{
		size_t StartPos;
		std::string Find;
		bool MatchCase, MatchWholeWord;
	};

	void ToLowerCase( std::string& buffer )
	{
		std::transform( buffer.begin(), buffer.end(), buffer.begin(), ::tolower );
	}

	void ToUpperCase( std::string& buffer )
	{
		std::transform( buffer.begin(), buffer.end(), buffer.begin(), ::toupper );
	}

	size_t ReplaceAll( std::string& buffer, const std::string& replace, sFindAttrib& attr )
	{
		if ( buffer.empty() || attr.Find.empty() ) return 0;

		std::string AdjustedBuffer = buffer;
		if ( !attr.MatchCase )
		{
			ToLowerCase( AdjustedBuffer );
			ToLowerCase( attr.Find );
		}
		if ( !attr.MatchWholeWord )
		{
			
		}

		auto iteratePos = attr.StartPos;
		size_t count = 0;
		while ( true )
		{
			auto found = AdjustedBuffer.find( attr.Find, iteratePos );
			if ( found == std::string::npos ) break;
			buffer.replace( buffer.begin() + found, buffer.begin() + found + attr.Find.size(), replace );
			AdjustedBuffer.replace( AdjustedBuffer.begin() + found, AdjustedBuffer.begin() + found + attr.Find.size(), replace );
			iteratePos = found + 1;
			count++;
		}
		return count;
	}
}