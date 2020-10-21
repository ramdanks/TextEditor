#include "Filehandle.h"
#include <wx/filedlg.h>
#include "LogGUI.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"

void Filehandle::InitFilehandle( wxWindow* parent, wxNotebook* tab, std::vector<wxStyledTextCtrl*>* vTextField )
{
    this->mParent = parent;
    this->mTab = tab;
    this->vTextField = vTextField;

    this->Init = true;
    std::vector<std::string> FormatList;
    this->SupportedFormat = "All types (*.*)|*.*";
    FormatList.push_back( "Normal text file (*.txt)|*.txt" );
    FormatList.push_back( "Memoriser file (*.mtx)|*.mtx" );
    FormatList.push_back( "Memoriser dictionary (*.mdt)|*.mdt" );
    FormatList.push_back( "Memoriser archive (*.mac)|*.mac" );
    for ( auto i : FormatList ) this->SupportedFormat += "|" + i;
}

void Filehandle::OnRenameFile()
{
}

void Filehandle::OnOpenFile()
{
    wxFileDialog openFileDialog( this->mParent, _( "Open Text File" ), "", "", this->SupportedFormat, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;

    try
    {
        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        LOGALL( LEVEL_TRACE, std::string( "Opened Filepath: " + filepath ), LOG_FILEPATH );

        auto vRead = Filestream::Read_Bin( filepath );
        THROW_ERR_IF( vRead.empty(), std::string( "Cannot open file " + filepath ) );

        std::string readSize = "File size: " + std::to_string( vRead.size() ) + "(bytes)";
        LOGALL( LEVEL_TRACE, readSize, LOG_FILEPATH );

        auto format = Filestream::FileExtension( std::string( openFileDialog.GetPath().mb_str() ) );

        if ( !this->vTextField->back()->IsEmpty() ) AddNewTab( Filestream::getFileName( filepath ) );
        else this->mTab->SetPageText( this->vTextField->size() - 1, Filestream::getFileName( filepath ) );

        this->vTextField->back()->AppendText( wxString( &vRead[0] ) );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), ERR_FILEPATH );
    }
}

void Filehandle::OnSaveFile()
{
    wxFileDialog openFileDialog( this->mParent, _( "Save Text File" ), "", "", this->SupportedFormat, wxFD_SAVE );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;
}

void Filehandle::OnSaveFileAll()
{

}

void Filehandle::OnSaveFileAs()
{
    wxFileDialog openFileDialog( this->mParent, _( "Save Text File" ), "", "", this->SupportedFormat, wxFD_SAVE );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;
}

void Filehandle::AddNewTab( const std::string& name )
{
    this->vTextField->push_back( new wxStyledTextCtrl( this->mTab, wxID_ANY ) );
    mTab->AddPage( vTextField->back(), name );
    mTab->ChangeSelection( vTextField->size() - 1 );
}
