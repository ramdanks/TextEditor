#include "FontEncoding.h"
#include "Feature/LogGUI.h"

wxString FontEncoding::GetEncodingString( const wxFontEncoding& enc )
{
    if      ( enc == wxFONTENCODING_SYSTEM )         return "System";
    else if ( enc == wxFONTENCODING_DEFAULT )        return "Default";
    else if ( enc == wxFONTENCODING_ISO8859_1 )      return "West European (Latin1)";
    else if ( enc == wxFONTENCODING_ISO8859_2 )      return "Centrar and East European (Latin2)";
    else if ( enc == wxFONTENCODING_ISO8859_3 )      return "Esperanto (Latin3)";
    else if ( enc == wxFONTENCODING_ISO8859_4 )      return "Baltic (Latin4)";
    else if ( enc == wxFONTENCODING_ISO8859_5 )      return "Cyrillic";
    else if ( enc == wxFONTENCODING_ISO8859_6 )      return "Arabic";
    else if ( enc == wxFONTENCODING_ISO8859_7 )      return "Greek";
    else if ( enc == wxFONTENCODING_ISO8859_8 )      return "Hebrew";
    else if ( enc == wxFONTENCODING_ISO8859_9 )      return "Turkish (Latin5)";
    else if ( enc == wxFONTENCODING_ISO8859_10 )     return "Baltic (Latin6)";
    else if ( enc == wxFONTENCODING_ISO8859_11 )     return "Thai";
    else if ( enc == wxFONTENCODING_ISO8859_12 )     return "?";
    else if ( enc == wxFONTENCODING_ISO8859_13 )     return "Baltic (Latin7)";
    else if ( enc == wxFONTENCODING_ISO8859_14 )     return "Latin8";
    else if ( enc == wxFONTENCODING_ISO8859_15 )     return "Latin9 or Latin0";
    else if ( enc == wxFONTENCODING_ISO8859_MAX )    return "Cyrillic (Charsets)";
    else if ( enc == wxFONTENCODING_KOI8 )           return "Russian (KOI8)";
    else if ( enc == wxFONTENCODING_KOI8_U )         return "Ukrainian (KOI8)";
    else if ( enc == wxFONTENCODING_ALTERNATIVE )    return "MS-DOS CP866";
    else if ( enc == wxFONTENCODING_BULGARIAN )      return "Bulgaria (Linux)";
    else if ( enc == wxFONTENCODING_CP437 )          return "MS-DOS CP437";
    else if ( enc == wxFONTENCODING_CP850 )          return "MS-DOS CP850";
    else if ( enc == wxFONTENCODING_CP852 )          return "MS-DOS CP852";
    else if ( enc == wxFONTENCODING_CP855 )          return "MS-DOS CP855";
    else if ( enc == wxFONTENCODING_CP866 )          return "MS-DOS CP866";
    else if ( enc == wxFONTENCODING_CP874 )          return "Thai (Win)";
    else if ( enc == wxFONTENCODING_CP932 )          return "Japanese (Shift-JIS)";
    else if ( enc == wxFONTENCODING_CP936 )          return "Chinese Simplified (GB)";
    else if ( enc == wxFONTENCODING_CP949 )          return "Korean (Hangul)";
    else if ( enc == wxFONTENCODING_CP950 )          return "Chinese (Traditional-Big5)";
    else if ( enc == wxFONTENCODING_CP1250 )         return "Latin2 (Win)";
    else if ( enc == wxFONTENCODING_CP1251 )         return "Cyrillic (Win)";
    else if ( enc == wxFONTENCODING_CP1252 )         return "Latin1 (Win)";
    else if ( enc == wxFONTENCODING_CP1253 )         return "Greek (Win)";
    else if ( enc == wxFONTENCODING_CP1254 )         return "Turkish (Win)";
    else if ( enc == wxFONTENCODING_CP1255 )         return "Hebrew (Win)";
    else if ( enc == wxFONTENCODING_CP1256 )         return "Arabic (Win)";
    else if ( enc == wxFONTENCODING_CP1257 )         return "Baltic (Win)";
    else if ( enc == wxFONTENCODING_CP1258 )         return "Vietnamese (Win)";
    else if ( enc == wxFONTENCODING_CP1361 )         return "Korean (Johab)";
    else if ( enc == wxFONTENCODING_CP12_MAX )       return "?";
    else if ( enc == wxFONTENCODING_UTF7 )           return "UTF-7";
    else if ( enc == wxFONTENCODING_UTF8 )           return "UTF-8";
    else if ( enc == wxFONTENCODING_EUC_JP )         return "EUC Japanese";
    else if ( enc == wxFONTENCODING_UTF16BE )        return "UTF-16 BE";
    else if ( enc == wxFONTENCODING_UTF16LE )        return "UTF-16 LE";
    else if ( enc == wxFONTENCODING_UTF32BE )        return "UTF-32 BE";
    else if ( enc == wxFONTENCODING_UTF32LE )        return "UTF-32 LE";
    else if ( enc == wxFONTENCODING_MACROMAN )       return "Roman (Mac)";
    else if ( enc == wxFONTENCODING_MACJAPANESE )    return "Japanese (Mac)";
    else if ( enc == wxFONTENCODING_MACCHINESETRAD ) return "Chinese Traditional (Mac)";
    else if ( enc == wxFONTENCODING_MACKOREAN )      return "Korean (Mac)";
    else if ( enc == wxFONTENCODING_MACARABIC )      return "Arabic (Mac)";
    else if ( enc == wxFONTENCODING_MACHEBREW )      return "Hebrew (Mac)";
    else if ( enc == wxFONTENCODING_MACGREEK )       return "Greek (Mac)";
    else if ( enc == wxFONTENCODING_MACCYRILLIC )    return "Cyrillic (Mac)";
    else if ( enc == wxFONTENCODING_MACDEVANAGARI )  return "Devanagari (Mac)";
    else if ( enc == wxFONTENCODING_MACGURMUKHI )    return "Gurmukhi (Mac)";
    else if ( enc == wxFONTENCODING_MACGUJARATI )    return "Gujarati (Mac)";
    else if ( enc == wxFONTENCODING_MACORIYA )       return "Oriya (Mac)";
    else if ( enc == wxFONTENCODING_MACBENGALI )     return "Bengali (Mac)";
    else if ( enc == wxFONTENCODING_MACTAMIL )       return "Tamil (Mac)";
    else if ( enc == wxFONTENCODING_MACTELUGU )      return "Telugu (Mac)";
    else if ( enc == wxFONTENCODING_MACKANNADA )     return "Kannada (Mac)";
    else if ( enc == wxFONTENCODING_MACMALAJALAM )   return "Malajalam (Mac)";
    else if ( enc == wxFONTENCODING_MACSINHALESE )   return "Sinhalese (Mac)";
    else if ( enc == wxFONTENCODING_MACBURMESE )     return "Burmese (Mac)";
    else if ( enc == wxFONTENCODING_MACKHMER )       return "Khmer (Mac)";
    else if ( enc == wxFONTENCODING_MACTHAI )        return "Thai (Mac)";
    else if ( enc == wxFONTENCODING_MACLAOTIAN )     return "Laotian (Mac)";
    else if ( enc == wxFONTENCODING_MACGEORGIAN )    return "Georgian (Mac)";
    else if ( enc == wxFONTENCODING_MACARMENIAN )    return "Armenian (Mac)";
    else if ( enc == wxFONTENCODING_MACCHINESESIMP ) return "Chinese Imperial (Mac)";
    else if ( enc == wxFONTENCODING_MACTIBETAN )     return "Tibetan (Mac)";
    else if ( enc == wxFONTENCODING_MACMONGOLIAN )   return "Mongolian (Mac)";
    else if ( enc == wxFONTENCODING_MACETHIOPIC )    return "Ethipic (Mac)";
    else if ( enc == wxFONTENCODING_MACCENTRALEUR )  return "Central Europe (Mac)";
    else if ( enc == wxFONTENCODING_MACVIATNAMESE )  return "Vietnamese (Mac)";
    else if ( enc == wxFONTENCODING_MACARABICEXT )   return "Arabic Extended (Mac)";
    else if ( enc == wxFONTENCODING_MACSYMBOL )      return "Symbol (Mac)";
    else if ( enc == wxFONTENCODING_MACDINGBATS )    return "Dingbats (Mac)";
    else if ( enc == wxFONTENCODING_MACTURKISH )     return "Turkish (Mac)";
    else if ( enc == wxFONTENCODING_MACCROATIAN )    return "Croatian (Mac)";
    else if ( enc == wxFONTENCODING_MACICELANDIC )   return "Icelandic (Mac)";
    else if ( enc == wxFONTENCODING_MACROMANIAN )    return "Romanian (Mac)";
    else if ( enc == wxFONTENCODING_MACCELTIC )      return "Celtic (Mac)";
    else if ( enc == wxFONTENCODING_MACGAELIC )      return "Gaelic (Mac)";
    else if ( enc == wxFONTENCODING_MACKEYBOARD )    return "Default (Mac)";
    return wxString();
}

bool FontEncoding::ConvertEOLString( wxString& str, int mode )
{
    PROFILE_FUNC();

    if ( str.empty() ) return true;
    int before = GetEOLMode( str );
    if ( before == mode ) return false; // no need to convert if already the same

    // remove eol char
    std::vector<std::string> vLines;
    if ( before == wxSTC_EOL_CR )    
        vLines = Filestream::ParseString( std::string( str ), CR_CHAR );
    else if ( before == wxSTC_EOL_LF )
        vLines = Filestream::ParseString( std::string( str ), LF_CHAR );
    else if ( before == wxSTC_EOL_CRLF )
    {
        vLines = Filestream::ParseString( std::string( str ), LF_CHAR ); // parse by LF char
        for ( uint32_t i = 0; i < vLines.size() - 1; i++ ) vLines[i].pop_back(); // remove CR char0
    }
    
    // create new eol char
    str.clear();
    if ( mode == wxSTC_EOL_CR )
    {
        for ( uint32_t i = 0; i < vLines.size() - 1; i++ )
            str += vLines[i] + '\r';
    }
    else if ( mode == wxSTC_EOL_LF )
    {
        for ( uint32_t i = 0; i < vLines.size() - 1; i++ )
            str += vLines[i] + '\n';
    }
    else if ( mode == wxSTC_EOL_CRLF )
    {
        for ( uint32_t i = 0; i < vLines.size() - 1; i++ )
            str += vLines[i] + "\r\n";
    }
    str += vLines.back();

    return true;
}

int FontEncoding::GetEOLMode( const wxString& str )
{
    uint32_t strSize = str.size();
    for ( uint32_t i = 0; i < strSize; i++ )
    {
        if ( str[i] == CR_CHAR )
            if ( i + 1 != strSize && str[i + 1] == LF_CHAR ) return wxSTC_EOL_CRLF;
            else return wxSTC_EOL_CR;
        else if ( str[i] == LF_CHAR )
            return wxSTC_EOL_LF;
    }
    return wxSTC_EOL_CRLF; //default
}

wxString FontEncoding::GetEOLModeString( const wxString& str )
{
    return EOLModeString( GetEOLMode( str ) );
}

wxString FontEncoding::EOLModeString( int mode )
{
    if      ( mode == wxSTC_EOL_CR )   return "Macintosh (CR)";
    else if ( mode == wxSTC_EOL_LF )   return "Unix (LF)";
    else if ( mode == wxSTC_EOL_CRLF ) return "Windows (CR-LF)";
}

void FontEncoding::CaseConversion( wxStyledTextCtrl* stc, const StringCase& sc )
{
    wxString buf = stc->GetText();
    if ( buf.empty() ) return;
    
    void ( *func )( wxString&, uint32_t, uint32_t ) = &RandomCase;
    if      ( sc == CASE_INVERSE ) func = &InverseCase;
    else if ( sc == CASE_RANDOM )  func = &RandomCase;
    else if ( sc == CASE_UPPER )   func = &UpperCase;
    else if ( sc == CASE_LOWER )   func = &LowerCase;

    auto selections = stc->GetSelections();
    for ( int i = 0; i < selections; i++ )
    {
        auto start = stc->GetSelectionNStart( i );
        auto end = stc->GetSelectionNEnd( i );
        func( buf, start, end );
    }
    stc->SetText( buf );
}

void FontEncoding::UpperCase( wxString& str, uint32_t from, uint32_t to )
{
    std::transform( str.begin() + from, str.begin() + to, str.begin() + from, toupper );
}

void FontEncoding::LowerCase( wxString& str, uint32_t from, uint32_t to )
{
    std::transform( str.begin() + from, str.begin() + to, str.begin() + from, tolower );
}

void FontEncoding::InverseCase( wxString& str, uint32_t from, uint32_t to )
{
    std::transform( str.begin() + from, str.begin() + to, str.begin() + from, ReverseCaseChar );
}

void FontEncoding::RandomCase( wxString& str, uint32_t from, uint32_t to )
{
    std::transform( str.begin() + from, str.begin() + to, str.begin() + from, RandomCaseChar );
}

char FontEncoding::ReverseCaseChar( char c )
{
    const auto uc = static_cast<unsigned char>( c ); // Sic.
    return ::isupper( uc ) ? ::tolower( uc ) : ::toupper( uc );
}

char FontEncoding::RandomCaseChar( char c )
{
    return Util::Random::Uniform() % 2  == 0 ? ::tolower( c ) : ::toupper( c );
}