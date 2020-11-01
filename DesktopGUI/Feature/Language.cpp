#include "Language.h"
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Err.h"
#include "../../Utilities/Timer.h"
#include "LogGUI.h"

//translation unit for static member
std::string                Language::mTitle;
std::vector<std::string>   Language::mMessage;

std::string Language::whatLanguage()
{
    if ( mTitle.empty() ) return EMPTY_IDENTIFIER;
    return mTitle;
}

std::string Language::getMessage( size_t index )
{
    if ( mMessage.empty() || index >= mMessage.size() ) return EMPTY_IDENTIFIER;
    return mMessage[index];
}

void Language::LoadMessageDefault()
{
    mMessage.push_back( "English-System" );
    mMessage.push_back( "Welcome to M�moriser" );
    mMessage.push_back( "M�moriser - Debug Console" );
    
    mMessage.push_back( "File" );
    mMessage.push_back( "New" );
    mMessage.push_back( "Save" );
    mMessage.push_back( "Save As" );
    mMessage.push_back( "Save All" );
    mMessage.push_back( "Open" );
    mMessage.push_back( "Rename " );
    mMessage.push_back( "Close" );
    mMessage.push_back( "Close All" );
    mMessage.push_back( "New Dictionary" );
    mMessage.push_back( "Open Dictionary" );
    mMessage.push_back( "Exit" );

    mMessage.push_back( "Edit" );
    mMessage.push_back( "Undo" );
    mMessage.push_back( "Redo" );
    mMessage.push_back( "Cut" );
    mMessage.push_back( "Copy" );
    mMessage.push_back( "Paste" );
    mMessage.push_back( "Delete" );
    mMessage.push_back( "Select All" );

    mMessage.push_back( "Search" );
    mMessage.push_back( "Find" );
    mMessage.push_back( "Select and Find Next" );
    mMessage.push_back( "Select and Find Previous" );
    mMessage.push_back( "Replace" );
    mMessage.push_back( "Goto" );

    mMessage.push_back( "View" );
    mMessage.push_back( "Always on Top" );
    mMessage.push_back( "Zoom In" );
    mMessage.push_back( "Zoom Out" );
    mMessage.push_back( "Restore Zoom" );
    mMessage.push_back( "Text Summary" );
    mMessage.push_back( "Compression Summary" );
    mMessage.push_back( "Debug Console" );

    mMessage.push_back( "Settings" );
    mMessage.push_back( "Preferences" );
    mMessage.push_back( "Style Configuration" );

    mMessage.push_back( "Help" );
    mMessage.push_back( "Report Bug" );
    mMessage.push_back( "Open Log Directory" );
    mMessage.push_back( "See Documentation" );
    mMessage.push_back( "About" );

    mTitle = mMessage[0];
}

std::string Language::IdentifyID( LanguageID id )
{
    if      ( id == ENGLISH )     return "English";
    else if ( id == BAHASA )      return "Bahasa Indonesia";
    else if ( id == SPANISH )     return "Espa�a";
    else if ( id == PORTUGUESE )  return "Portugu�s";
    else if ( id == GERMAN )      return "Deutsch";
    else if ( id == ITALIAN )     return "Italiano";
    else if ( id == FRENCH )      return "Fran�ais";
    else if ( id == DUTCH )       return "Nederlands";
}

bool Language::LoadMessage( LanguageID id )
{
    try
    {
        std::vector<uint8_t> vRead;
        if      ( id == ENGLISH )      vRead = Filestream::Read_Bin( LANG_ENGLISH_FILEPATH );
        else if ( id == BAHASA )       vRead = Filestream::Read_Bin( LANG_BAHASA_FILEPATH );
        else if ( id == SPANISH )      vRead = Filestream::Read_Bin( LANG_SPANISH_FILEPATH );
        else if ( id == PORTUGUESE )   vRead = Filestream::Read_Bin( LANG_PORTUGUESE_FILEPATH );
        else if ( id == GERMAN )       vRead = Filestream::Read_Bin( LANG_GERMAN_FILEPATH );
        else if ( id == ITALIAN )      vRead = Filestream::Read_Bin( LANG_ITALIAN_FILEPATH );
        else if ( id == FRENCH )       vRead = Filestream::Read_Bin( LANG_FRENCH_FILEPATH );
        else if ( id == DUTCH )        vRead = Filestream::Read_Bin( LANG_DUTCH_FILEPATH );
        THROW_ERR_IFEMPTY( vRead, "Problem loading a language file from LoadMessage()!" );

        mMessage = Filestream::ParseString( (char*) &vRead[0], '\n' );
        THROW_ERR_IFEMPTY( mMessage, "Problem parsing a language string from LoadMessage()!" );
        mTitle = mMessage[0];
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_WARN, e.Seek(), LOG_FILEPATH );
        LoadMessageDefault();
        return false;
    }
    catch ( ... )
    {
        LOGALL( LEVEL_WARN, "Unknown Exception when processing language file from LoadMessage()!", LOG_FILEPATH );
        LoadMessageDefault();
        return false;
    }
    return true;
}