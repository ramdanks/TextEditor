#pragma once
#include <wx/string.h>

#define CR_CHAR   '\r'
#define LF_CHAR   '\n'

class FontEncoding
{
public:
	static wxString GetEncodingString( const wxFontEncoding& enc );
	static void AdjustEOLString( wxString& str, int mode );

	static int GetEOLMode( wxString& str );
};