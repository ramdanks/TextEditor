#include <wx/process.h>
#include <wx/filedlg.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Err.h"
#include "FilehandleGUI.h"
#include "Config.h"
#include "LogGUI.h"

#if defined( __WIN32__ )
    #define CMD_COMPRESS(file)     "Compressor -c temp/out " + file + " -mt"
    #define CMD_DECOMPRESS(file)   "Compressor -d " + file + " temp/in -mt"
#elif defined ( __linux__ )
    #define CMD_COMPRESS(file)     "./Compressor -c temp/out " + file + " -mt"
    #define CMD_DECOMPRESS(file)   "./Compressor -d " + file + " temp/in -mt"
#endif

wxString FilehandleGUI::mSupportedFormat;

void FilehandleGUI::LoadSupportedFormat()
{
    mSupportedFormat = Config::LoadSupportedFormat();
}

std::string FilehandleGUI::OpenDialog( wxWindow* parent, const wxString& defaultFilename )
{
    return FileDialog( parent, "Open File", defaultFilename, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
}

std::string FilehandleGUI::SaveDialog( wxWindow* parent, const wxString& defaultFilename )
{
    return FileDialog( parent, "Save As", defaultFilename, wxFD_SAVE );
}

wxArrayString FilehandleGUI::GetDroppedFiles( wxString* dropped, uint32_t size )
{
    wxArrayString files;
    files.reserve( size );
    for ( int i = 0; i < size; i++ )
    {
        if      ( wxFileExists( dropped[i] ) ) files.push_back( dropped[i] );
        else if ( wxDirExists( dropped[i] ) )  wxDir::GetAllFiles( dropped[i], &files );      
    }
    return files;
}

std::vector<uint8_t> FilehandleGUI::OpenFileFormat( const std::string& filepath )
{
    auto format = Filestream::FileExtension( filepath );
    std::vector<uint8_t> vRead;
    if ( filepath.empty() ) return vRead;

    if ( format == "mtx" )
    {
        std::string cmd = CMD_DECOMPRESS( filepath );
        auto proc = wxProcess::Open( cmd );
        THROW_ERR_IFNULLPTR( proc, "Cannot create a process for decompressing!" );
        
        wxWindowDisabler disabler;
        wxBusyCursor busyCursor;
        wxBusyInfo busyInfo( "Decompressing files, wait please..." );
        while ( wxProcess::Exists( proc->GetPid() ) );
        vRead = Filestream::Read_Bin( "temp/in" );
        Filestream::Delete_File( "temp/in" );
    }
    else
    {
        vRead = Filestream::Read_Bin( filepath );
    }
    return vRead;
}

void FilehandleGUI::SaveFileFormat( const std::string& filepath, const char* pd, size_t size )
{
    auto format = Filestream::FileExtension( filepath );
    if ( pd == nullptr || size == 0 ) return;

    if ( format == "mtx" )
    {
        Filestream::Write_Bin( pd, size, "temp/out" );

        std::string cmd = CMD_COMPRESS( filepath );
        auto proc = wxProcess::Open( cmd );
        THROW_ERR_IFNULLPTR( proc, "Cannot create a process for compressing!" );

        wxWindowDisabler disabler;
        wxBusyCursor busyCursor;
        wxBusyInfo busyInfo( "Compressing files, wait please..." );
        while ( wxProcess::Exists( proc->GetPid() ) );
        Filestream::Delete_File( "temp/out" );
    }
    else
    {
        Filestream::Write_Bin( pd, size, filepath );
    }
}

std::string FilehandleGUI::FileDialog( wxWindow* parent, const wxString& message, const wxString& defaultFilename, int style )
{
    if ( mSupportedFormat.empty() ) LoadSupportedFormat();
    auto fd = wxFileDialog( parent, message, wxEmptyString, defaultFilename, mSupportedFormat, style );
    fd.ShowModal();
    return std::string( fd.GetPath().mb_str() );
}