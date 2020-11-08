#include "FontEncoding.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "Feature/LogGUI.h"
#include <wx/stc/stc.h>

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

void FontEncoding::AdjustEOLString( wxString& str, int mode )
{
    Util::Timer Tm( "Adjust EOL", MS, false );

    // find eol type
    int before = GetEOLMode( str );
    // if already the same eol type
    if ( before == mode ) return;

    // remove eol char
    std::vector<std::string> vLines;
    if ( before == wxSTC_EOL_CR )    
        vLines = Filestream::ParseString( std::string( str ), CR_CHAR );
    else if ( before == wxSTC_EOL_LF )
        vLines = Filestream::ParseString( std::string( str ), LF_CHAR );
    else if ( before == wxSTC_EOL_CRLF )
    {
        vLines = Filestream::ParseString( std::string( str ), LF_CHAR ); // parse by LF char
        for ( auto& line : vLines ) line.pop_back(); // remove CR char
    }
    
    // create new eol char
    str.clear();
    if ( mode == wxSTC_EOL_CR )
    {
        for ( const auto& line : vLines )
            str += line + '\r';   
    }
    else if ( mode == wxSTC_EOL_LF )
    {
        for ( const auto& line : vLines )
            str += line + '\n';
    }
    else if ( mode == wxSTC_EOL_CRLF )
    {
        for ( const auto& line : vLines )
            str += line + '\r' + '\n';
    }

    LOGALL( LEVEL_TRACE, "Adjust EOL time: " + TO_STR( Tm.Toc() ), +" (ms)" );
}

int FontEncoding::GetEOLMode( wxString& str )
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