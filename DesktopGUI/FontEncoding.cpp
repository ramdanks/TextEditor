#include "FontEncoding.h"
#include "Feature/LogGUI.h"

wxString FontEncoding::GetEncodingString( const wxFontEncoding& enc )
{
    switch (enc)
    {
        case wxFONTENCODING_SYSTEM:           return "System";
        case wxFONTENCODING_DEFAULT:          return "Default";
        case wxFONTENCODING_ISO8859_1:        return "West European (Latin1)";
        case wxFONTENCODING_ISO8859_2:        return "Centrar and East European (Latin2)";
        case wxFONTENCODING_ISO8859_3:        return "Esperanto (Latin3)";
        case wxFONTENCODING_ISO8859_4:        return "Baltic (Latin4)";
        case wxFONTENCODING_ISO8859_5:        return "Cyrillic";
        case wxFONTENCODING_ISO8859_6:        return "Arabic";
        case wxFONTENCODING_ISO8859_7:        return "Greek";
        case wxFONTENCODING_ISO8859_8:        return "Hebrew";
        case wxFONTENCODING_ISO8859_9:        return "Turkish (Latin5)";
        case wxFONTENCODING_ISO8859_10:       return "Baltic (Latin6)";
        case wxFONTENCODING_ISO8859_11:       return "Thai";
        case wxFONTENCODING_ISO8859_12:       return "?";
        case wxFONTENCODING_ISO8859_13:       return "Baltic (Latin7)";
        case wxFONTENCODING_ISO8859_14:       return "Latin8";
        case wxFONTENCODING_ISO8859_15:       return "Latin9 or Latin0";
        case wxFONTENCODING_ISO8859_MAX:      return "Cyrillic (Charsets)";
        case wxFONTENCODING_KOI8:             return "Russian (KOI8)";
        case wxFONTENCODING_KOI8_U:           return "Ukrainian (KOI8)";
        case wxFONTENCODING_ALTERNATIVE:      return "MS-DOS CP866";
        case wxFONTENCODING_BULGARIAN:        return "Bulgaria (Linux)";
        case wxFONTENCODING_CP437:            return "MS-DOS CP437";
        case wxFONTENCODING_CP850:            return "MS-DOS CP850";
        case wxFONTENCODING_CP852:            return "MS-DOS CP852";
        case wxFONTENCODING_CP855:            return "MS-DOS CP855";
        case wxFONTENCODING_CP866:            return "MS-DOS CP866";
        case wxFONTENCODING_CP874:            return "Thai (Win)";
        case wxFONTENCODING_CP932:            return "Japanese (Shift-JIS)";
        case wxFONTENCODING_CP936:            return "Chinese Simplified (GB)";
        case wxFONTENCODING_CP949:            return "Korean (Hangul)";
        case wxFONTENCODING_CP950:            return "Chinese (Traditional-Big5)";
        case wxFONTENCODING_CP1250:           return "Latin2 (Win)";
        case wxFONTENCODING_CP1251:           return "Cyrillic (Win)";
        case wxFONTENCODING_CP1252:           return "Latin1 (Win)";
        case wxFONTENCODING_CP1253:           return "Greek (Win)";
        case wxFONTENCODING_CP1254:           return "Turkish (Win)";
        case wxFONTENCODING_CP1255:           return "Hebrew (Win)";
        case wxFONTENCODING_CP1256:           return "Arabic (Win)";
        case wxFONTENCODING_CP1257:           return "Baltic (Win)";
        case wxFONTENCODING_CP1258:           return "Vietnamese (Win)";
        case wxFONTENCODING_CP1361:           return "Korean (Johab)";
        case wxFONTENCODING_CP12_MAX:         return "?";
        case wxFONTENCODING_UTF7:             return "UTF-7";
        case wxFONTENCODING_UTF8:             return "UTF-8";
        case wxFONTENCODING_EUC_JP:           return "EUC Japanese";
        case wxFONTENCODING_UTF16BE:          return "UTF-16 BE";
        case wxFONTENCODING_UTF16LE:          return "UTF-16 LE";
        case wxFONTENCODING_UTF32BE:          return "UTF-32 BE";
        case wxFONTENCODING_UTF32LE:          return "UTF-32 LE";
        case wxFONTENCODING_MACROMAN:         return "Roman (Mac)";
        case wxFONTENCODING_MACJAPANESE:      return "Japanese (Mac)";
        case wxFONTENCODING_MACCHINESETRAD:   return "Chinese Traditional (Mac)";
        case wxFONTENCODING_MACKOREAN:        return "Korean (Mac)";
        case wxFONTENCODING_MACARABIC:        return "Arabic (Mac)";
        case wxFONTENCODING_MACHEBREW:        return "Hebrew (Mac)";
        case wxFONTENCODING_MACGREEK:         return "Greek (Mac)";
        case wxFONTENCODING_MACCYRILLIC:      return "Cyrillic (Mac)";
        case wxFONTENCODING_MACDEVANAGARI:    return "Devanagari (Mac)";
        case wxFONTENCODING_MACGURMUKHI:      return "Gurmukhi (Mac)";
        case wxFONTENCODING_MACGUJARATI:      return "Gujarati (Mac)";
        case wxFONTENCODING_MACORIYA:         return "Oriya (Mac)";
        case wxFONTENCODING_MACBENGALI:       return "Bengali (Mac)";
        case wxFONTENCODING_MACTAMIL:         return "Tamil (Mac)";
        case wxFONTENCODING_MACTELUGU:        return "Telugu (Mac)";
        case wxFONTENCODING_MACKANNADA:       return "Kannada (Mac)";
        case wxFONTENCODING_MACMALAJALAM:     return "Malajalam (Mac)";
        case wxFONTENCODING_MACSINHALESE:     return "Sinhalese (Mac)";
        case wxFONTENCODING_MACBURMESE:       return "Burmese (Mac)";
        case wxFONTENCODING_MACKHMER:         return "Khmer (Mac)";
        case wxFONTENCODING_MACTHAI:          return "Thai (Mac)";
        case wxFONTENCODING_MACLAOTIAN:       return "Laotian (Mac)";
        case wxFONTENCODING_MACGEORGIAN:      return "Georgian (Mac)";
        case wxFONTENCODING_MACARMENIAN:      return "Armenian (Mac)";
        case wxFONTENCODING_MACCHINESESIMP:   return "Chinese Imperial (Mac)";
        case wxFONTENCODING_MACTIBETAN :      return "Tibetan (Mac)";
        case wxFONTENCODING_MACMONGOLIAN:     return "Mongolian (Mac)";
        case wxFONTENCODING_MACETHIOPIC:      return "Ethipic (Mac)";
        case wxFONTENCODING_MACCENTRALEUR:    return "Central Europe (Mac)";
        case wxFONTENCODING_MACVIATNAMESE:    return "Vietnamese (Mac)";
        case wxFONTENCODING_MACARABICEXT:     return "Arabic Extended (Mac)";
        case wxFONTENCODING_MACSYMBOL:        return "Symbol (Mac)";
        case wxFONTENCODING_MACDINGBATS:      return "Dingbats (Mac)";
        case wxFONTENCODING_MACTURKISH:       return "Turkish (Mac)";
        case wxFONTENCODING_MACCROATIAN:      return "Croatian (Mac)";
        case wxFONTENCODING_MACICELANDIC:     return "Icelandic (Mac)";
        case wxFONTENCODING_MACROMANIAN:      return "Romanian (Mac)";
        case wxFONTENCODING_MACCELTIC:        return "Celtic (Mac)";
        case wxFONTENCODING_MACGAELIC:        return "Gaelic (Mac)";
        case wxFONTENCODING_MACKEYBOARD:      return "Default (Mac)";
        default:                              return wxString();
    }
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