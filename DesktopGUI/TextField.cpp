#include "TextField.h"
#include <wx/filedlg.h>
#include "LogGUI.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"

std::string TextField::SupportedFormat = "";
wxWindow* TextField::mParent = nullptr;
wxNotebook* TextField::mTab = nullptr;
std::vector<wxStyledTextCtrl*> TextField::mTextField = {};
std::vector<std::string> TextField::mPathAbsolute = {};
std::vector<std::string> TextField::mPathTemporary = {};
bool TextField::Init = false;

void TextField::InitFilehandle( wxWindow* parent )
{
    mParent = parent;
    mTab = new wxNotebook( parent, wxID_ANY );

    Init = true;
    SupportedFormat = "All types (*.*)|*.*";
    SupportedFormat += "|Normal text file (*.txt)|*.txt";
    SupportedFormat += "|Memoriser file (*.mtx)|*.mtx";
    SupportedFormat += "|Memoriser dictionary (*.mdt)|*.mdt";
    SupportedFormat += "|Memoriser archive (*.mac)|*.mac";
}

void TextField::FetchTempFile()
{
    mPathTemporary = Filestream::File_List( "temp" );
    if ( mPathTemporary.size() == 0 )
    {
        mPathAbsolute.push_back( std::string() );
        mPathTemporary.push_back( "temp/new1.tmp" );
        CreateTempFile( mPathTemporary.back() );
        AddNewTab( "new1" );
    }
    else
    {
        mTextField.reserve( mPathTemporary.size() );
        mPathAbsolute.resize( mPathTemporary.size() ); //empty path because temp doesnt have absolute path
        for ( uint32_t i = 0; i < mPathTemporary.size(); i++ )
        {
            AddNewTab( Filestream::getFileName( mPathTemporary[i] ) );
            auto vRead = Filestream::Read_Bin( mPathTemporary[i] );
            if ( !vRead.empty() ) mTextField.back()->AppendText( &vRead[0] );
        }
    }
    LOGALL( LEVEL_TRACE, "Temporary file amount: " + std::to_string( mPathTemporary.size() ), LOG_FILEPATH );
}

void TextField::CreateTempFile( const std::string& name )
{
    Filestream::Create_Directories( "temp" );
    Filestream::Write_Bin( nullptr, NULL, name );
}

void TextField::OnNewFile()
{
    //dont create a new file if the last tab is a temporary file and existing filed is empty
    if ( !mTextField.empty() && mPathAbsolute.back().empty() && mTextField.back()->IsEmpty() ) return;

    std::string tempPath;
    for ( uint32_t counter = 1; ; counter++ )
    {
        tempPath = "temp/new" + std::to_string( counter ) + ".tmp";
        if ( !Filestream::Is_Exist( tempPath ) ) break;
    }
    Filestream::Write_Bin( nullptr, NULL, tempPath );
    AddNewTab( Filestream::getFileName( tempPath, true, '/' ) );

    //insert path
    mPathAbsolute.push_back( std::string() );
    mPathTemporary.push_back( tempPath );
}

void TextField::OnRenameFile()
{
}

void TextField::OnOpenFile()
{
    wxFileDialog openFileDialog( mParent, _( "Open Text File" ), "", "", SupportedFormat, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;

    try
    {
        mPathAbsolute.push_back( std::string( openFileDialog.GetPath().mb_str() ) );
        mPathTemporary.push_back( std::string() ); //no temporary file
        LOGALL( LEVEL_TRACE, std::string( "Opened Filepath: " + mPathAbsolute.back() ), LOG_FILEPATH );
        
        auto vRead = Filestream::Read_Bin( mPathAbsolute.back() );
        THROW_ERR_IF( vRead.empty(), std::string( "Cannot open file " + mPathAbsolute.back() ) );

        std::string readSize = "File size: " + std::to_string( vRead.size() ) + "(bytes)";
        LOGALL( LEVEL_TRACE, readSize, LOG_FILEPATH );

        auto format = Filestream::FileExtension( std::string( openFileDialog.GetPath().mb_str() ) );

        if ( !mTextField.back()->IsEmpty() ) AddNewTab( Filestream::getFileName( mPathAbsolute.back() ) );
        else mTab->SetPageText( mTextField.size() - 1, Filestream::getFileName( mPathAbsolute.back() ) );

        mTextField.back()->AppendText( wxString( &vRead[0] ) );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), ERR_FILEPATH );
    }
}

void TextField::OnTabClose()
{
    //we dont need to close if existing tab is only one, already temporary, and zero content
    if ( mTextField.size() == 1 && mPathAbsolute[0].empty() && mTextField[0]->IsEmpty() ) return;

    try
    {
        auto currentPage = mTab->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );
    
        auto prompt = new wxMessageDialog( mParent, "Are you sure want to close this file? Any changes will be ignored, please save your work!", "Close Window", wxYES_NO );
        if ( prompt->ShowModal() == wxID_YES )
        {
            //if its a temporary file, delete the temp file
            //we dont want to delete an absolute file
            if ( mPathAbsolute[currentPage].empty() )
                Filestream::Delete_File( mPathTemporary[currentPage] );

            mTab->DeletePage( currentPage );
            mTextField.erase( mTextField.begin() + currentPage );
            mPathTemporary.erase( mPathTemporary.begin() + currentPage );
            mPathAbsolute.erase( mPathAbsolute.begin() + currentPage );
            if ( mTextField.size() < 1 ) OnNewFile();
            
            prompt->Destroy();
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnTabCloseAll()
{
    //we dont need to close if existing tab is only one, already temporary, and zero content
    if ( mTextField.size() == 1 && mPathAbsolute[0].empty() && mTextField[0]->IsEmpty() ) return;

    try
    {
        auto prompt = new wxMessageDialog( mParent, "Are you sure want to close all file? Any changes will be ignored, please save your work!", "Close All Window", wxYES_NO );
        if ( prompt->ShowModal() == wxID_YES )
        {
            for ( uint32_t i = 0; i < mTextField.size(); i++ )
            {
                //if it's a temporary file, delete it!
                if ( mPathAbsolute[i].empty() )
                    Filestream::Delete_File( mPathTemporary[i] );
                
            }
            mTab->DeleteAllPages();
            mTextField.clear();
            mPathTemporary.clear();
            mPathAbsolute.clear();
            OnNewFile();
            prompt->Destroy();
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnClose()
{
    //for ( uint32_t i = 0; i < mFilePathList.size(); i++ )
    //{
    //    std::string temp = std::string( mTextField[i]->GetText().mb_str() );
    //    Filestream::Write_Bin( temp.c_str(), temp.size(), mFilePathList[i] );
    //    LOGALL( LEVEL_TRACE, "Saving temporary file: " + mFilePathList[i], LOG_FILEPATH );
    //}
}

void TextField::OnSaveFile()
{
    wxFileDialog openFileDialog( mParent, _( "Save File" ), "", "", SupportedFormat, wxFD_SAVE );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;
}

void TextField::OnSaveFileAll()
{

}

void TextField::OnSaveFileAs()
{
    wxFileDialog openFileDialog( mParent, _( "Save As" ), "", "", SupportedFormat, wxFD_SAVE );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;
}

void TextField::AddNewTab( const std::string& name )
{
    mTextField.push_back( new wxStyledTextCtrl( mTab, wxID_ANY ) );
    mTab->AddPage( mTextField.back(), name );
    mTab->ChangeSelection( mTextField.size() - 1 );
}
