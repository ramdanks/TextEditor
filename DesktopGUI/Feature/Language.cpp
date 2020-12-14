#include "Language.h"
#include "LogGUI.h"
#include "Config.h"

//translation unit for static member
std::vector<wxString> Language::mMessage;

wxString Language::GetMessage( size_t index )
{
    if ( mMessage.empty() || index >= mMessage.size() ) return EMPTY_IDENTIFIER;
    return mMessage[index];
}

void Language::LoadMessageDefault()
{
    mMessage.push_back( "English-System" );
    mMessage.push_back( "Welcome to Mémoriser" );
    mMessage.push_back( "Mémoriser - Debug Console" );
    
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
}

wxString Language::IdentifyID( LanguageID id )
{
    if      ( id == ENGLISH )     return "English";
    else if ( id == BAHASA )      return "Bahasa Indonesia";
    else if ( id == SPANISH )     return "España";
    else if ( id == PORTUGUESE )  return "Português";
    else if ( id == GERMAN )      return "Deutsch";
    else if ( id == ITALIAN )     return "Italiano";
    else if ( id == FRENCH )      return "Français";
    else if ( id == DUTCH )       return "Nederlands";
}

bool Language::LoadMessage( LanguageID id )
{
    PROFILE_FUNC();

    try
    {
        std::vector<uint8_t> vRead;
        if      ( id == ENGLISH )      vRead = Filestream::Read_Bin( Config::sAppPath + LANG_ENGLISH_FILEPATH );
        else if ( id == BAHASA )       vRead = Filestream::Read_Bin( Config::sAppPath + LANG_BAHASA_FILEPATH );
        else if ( id == SPANISH )      vRead = Filestream::Read_Bin( Config::sAppPath + LANG_SPANISH_FILEPATH );
        else if ( id == PORTUGUESE )   vRead = Filestream::Read_Bin( Config::sAppPath + LANG_PORTUGUESE_FILEPATH );
        else if ( id == GERMAN )       vRead = Filestream::Read_Bin( Config::sAppPath + LANG_GERMAN_FILEPATH );
        else if ( id == ITALIAN )      vRead = Filestream::Read_Bin( Config::sAppPath + LANG_ITALIAN_FILEPATH );
        else if ( id == FRENCH )       vRead = Filestream::Read_Bin( Config::sAppPath + LANG_FRENCH_FILEPATH );
        else if ( id == DUTCH )        vRead = Filestream::Read_Bin( Config::sAppPath + LANG_DUTCH_FILEPATH );
        
        // load file
        THROW_ERR_IFEMPTY( vRead, "Problem loading a language file from LoadMessage()!" );

        // parsing string by eol
        auto vMsg = Filestream::ParseString( (char*) &vRead[0], '\n' );
        THROW_ERR_IFEMPTY( vMsg, "Problem parsing a language string from LoadMessage()!" );

        // save and convert from utf-8
        mMessage.clear();
        for ( const auto& i : vMsg ) mMessage.push_back( wxString::FromUTF8( i ) );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_WARN, e.Seek() );
        LoadMessageDefault();
        return false;
    }
    catch ( ... )
    {
        LOG_ALL( LV_WARN, "Unknown Exception when processing language file from LoadMessage()!" );
        LoadMessageDefault();
        return false;
    }
    return true;
}