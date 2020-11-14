#pragma once
#include <wx/string.h>
#include <wx/stc/stc.h>

#define CR_CHAR   '\r'
#define LF_CHAR   '\n'

enum StringCase { CASE_INVERSE, CASE_RANDOM, CASE_UPPER, CASE_LOWER };

class FontEncoding
{
public:
	FontEncoding() = delete;
	FontEncoding( const FontEncoding& other ) = delete;

	static wxString GetEncodingString( const wxFontEncoding& enc );
	static bool ConvertEOLString( wxString& str, int mode );

	static int GetEOLMode( const wxString& str );
	static wxString GetEOLModeString( const wxString& str );
	static wxString EOLModeString( int mode );

	static void CaseConversion( wxStyledTextCtrl* stc, const StringCase& sc );

	static void UpperCase( wxString& str, uint32_t from, uint32_t to );
	static void LowerCase( wxString& str, uint32_t from, uint32_t to );
	static void InverseCase( wxString& str, uint32_t from, uint32_t to );
	static void RandomCase( wxString& str, uint32_t from, uint32_t to );

private:
	static char ReverseCaseChar( char c );
	static char RandomCaseChar( char c );
};