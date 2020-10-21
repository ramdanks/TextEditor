#include "TextField.h"
#include <wx/filedlg.h>
#include "LogGUI.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"

//contain only one tab or textfield, already temporary, and no content
#define IF_FIELD_UNOCCUPIED if ( mTextField.size() == 1 && mPathAbsolute[0].empty() && mTextField[0]->IsEmpty() )
#define IF_TEMPORARY_FIELD(x) if ( mPathAbsolute[x].empty() )

std::string TextField::SupportedFormat = "";
wxWindow* TextField::mParent = nullptr;
wxAuiNotebook* TextField::mTab = nullptr;
std::vector<wxStyledTextCtrl*> TextField::mTextField;
std::vector<std::string> TextField::mPathAbsolute;
std::vector<std::string> TextField::mPathTemporary;
bool TextField::Init = false;

void TextField::InitFilehandle( wxWindow* parent )
{
    mParent = parent;
    mTab = new wxAuiNotebook( parent, wxID_ANY );

    Init = true;
    SupportedFormat = "All types (*.*)|*.*";
    SupportedFormat += "|Normal text file (*.txt)|*.txt";
    SupportedFormat += "|Memoriser file (*.mtx)|*.mtx";
    SupportedFormat += "|Memoriser dictionary (*.mdt)|*.mdt";
    SupportedFormat += "|Memoriser archive (*.mac)|*.mac";
}

void TextField::FetchTempFile()
{
    Filestream::Create_Directories( "temp" );
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
    std::string tempPath;
    for ( uint32_t counter = 1; ; counter++ )
    {
        tempPath = "temp\\new" + std::to_string( counter ) + ".tmp";
        if ( !Filestream::Is_Exist( tempPath ) ) break;
    }
    Filestream::Write_Bin( nullptr, NULL, tempPath );
    AddNewTab( Filestream::getFileName( tempPath ) );

    //insert path
    mPathAbsolute.push_back( std::string() );
    mPathTemporary.push_back( tempPath );
}

void TextField::OnRenameFile()
{
    auto currentPage = mTab->GetSelection();
    IF_TEMPORARY_FIELD( currentPage ) { OnSaveFileAs(); return; }

    auto mRenameDlg = new wxTextEntryDialog( mParent, "Insert new name:", "Rename File", mTab->GetPageText( currentPage ) );
    if ( mRenameDlg->ShowModal() == wxID_OK )
    {
        uint32_t len = mTab->GetPageText( currentPage ).Len();
        std::string newName = std::string( mRenameDlg->GetValue().mb_str() );
        std::string oldPath = std::string( mPathAbsolute[currentPage].begin(), mPathAbsolute[currentPage].end() - len );

        if ( Filestream::Is_Exist( oldPath + newName ) )
        {
            auto prompt = wxMessageDialog( mParent, "File with that name already exist!\n"
                                           "Are you sure want to overwrite?", "Overwrite", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }
        Filestream::Rename_File( oldPath + newName, mPathAbsolute[currentPage] );
        mTab->SetPageText( currentPage, mRenameDlg->GetValue() );
        LOGALL( LEVEL_TRACE, "Rename file to: " + newName, LOG_FILEPATH );
    }
}

void TextField::OnOpenFile()
{
    wxFileDialog openFileDialog( mParent, _( "Open Text File" ), "", "", SupportedFormat, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;

    try
    {
        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        for ( uint32_t i = 0; i < mPathAbsolute.size(); i++ )
            if ( filepath == mPathAbsolute[i] )
            {
                mTab->ChangeSelection( i );
                return;
            }

        IF_FIELD_UNOCCUPIED
        {
            Filestream::Delete_File( mPathTemporary[0] );
            mPathAbsolute[0] = std::string( openFileDialog.GetPath().mb_str() );
            mPathTemporary[0] = std::string();
        }
        else
        {
            mPathAbsolute.push_back( filepath );
            mPathTemporary.push_back( std::string() ); //no temporary file
        }
        LOGALL( LEVEL_TRACE, std::string( "Opened Filepath: " + mPathAbsolute.back() ), LOG_FILEPATH );
        
        auto vRead = Filestream::Read_Bin( mPathAbsolute.back() );
        THROW_ERR_IF( vRead.empty(), std::string( "Cannot open file " + mPathAbsolute.back() ) );

        std::string readSize = "File size: " + std::to_string( vRead.size() ) + "(bytes)";
        LOGALL( LEVEL_TRACE, readSize, LOG_FILEPATH );

        auto format = Filestream::FileExtension( filepath );

        if ( !mTextField.back()->IsEmpty() ) AddNewTab( Filestream::getFileName( mPathAbsolute.back() ) );
        else mTab->SetPageText( mTextField.size() - 1, Filestream::getFileName( mPathAbsolute.back() ) );

        mTextField.back()->AppendText( wxString::FromUTF8( (const char*) &vRead[0], vRead.size() ) );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), ERR_FILEPATH );
    }
}

void TextField::OnTabClose()
{
    //we dont need to close if existing tab is only one, already temporary, and zero content
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto currentPage = mTab->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        auto prompt = wxMessageDialog( mParent, "Are you sure want to close this notebook page?\n"
                                      "Any changes will be ignored, please save your work!", "Close Page", wxYES_NO );
        if ( prompt.ShowModal() == wxID_YES )
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
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnTabClose( wxAuiNotebookEvent& evt )
{
    //we dont need to close if existing tab is only one, already temporary, and zero content
    IF_FIELD_UNOCCUPIED { evt.Veto(); return; }
    try
    {
        auto currentPage = mTab->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );
        
        auto prompt = wxMessageDialog( mParent, "Are you sure want to close this notebook page?\n" 
                                      "Any changes will be ignored, please save your work!", "Close Page", wxYES_NO );
        if ( prompt.ShowModal() == wxID_YES )
        {
            //if its a temporary file, delete the temp file
            //we dont want to delete an absolute file
            if ( mPathAbsolute[currentPage].empty() )
                Filestream::Delete_File( mPathTemporary[currentPage] );

            mTextField.erase( mTextField.begin() + currentPage );
            mPathTemporary.erase( mPathTemporary.begin() + currentPage );
            mPathAbsolute.erase( mPathAbsolute.begin() + currentPage );
            if ( mTextField.size() < 1 ) OnNewFile();
        }
        else evt.Veto();
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnTabCloseAll()
{
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto prompt = wxMessageDialog( mParent, "Are you sure want to close all file?\n" 
                                           "Any changes will be ignored, please save your work!","Close All Window", wxYES_NO );
        if ( prompt.ShowModal() == wxID_YES )
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
    try
    {
        auto currentPage = mTab->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        //if a temporary file, save as
        IF_TEMPORARY_FIELD( currentPage ) { OnSaveFileAs(); return; }

        std::string pData = std::string( mTextField[currentPage]->GetText().mb_str( wxConvUTF8 ) );
        Filestream::Write_Bin( pData.c_str(), pData.size(), mPathAbsolute[currentPage] );
        LOGALL( LEVEL_INFO, "File saved: " + mPathAbsolute[currentPage], LOG_FILEPATH );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnSaveFileAll()
{
    for ( uint32_t i = 0; i < mTextField.size(); i++ )
    {     
        mTab->SetSelection( i );
        IF_TEMPORARY_FIELD( i ) OnSaveFileAs();   
        else                    OnSaveFile();
    }
}

bool TextField::OnSaveFileAs()
{
    try
    {
        auto currentPage = mTab->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        wxFileDialog openFileDialog( mParent, _( "Save As" ), "", mTab->GetPageText( currentPage ), SupportedFormat, wxFD_SAVE );
        if ( openFileDialog.ShowModal() == wxID_CANCEL ) return false;

        std::string pData = std::string( mTextField[currentPage]->GetText().mb_str( wxConvUTF8 ) );
        THROW_ERR_IF( pData.empty(), "Cannot extract text from Text Field!" );

        //if file is a temporary delete temp
        if ( mPathAbsolute[currentPage].empty() )
            Filestream::Delete_File( mPathTemporary[currentPage] );

        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        mPathAbsolute[currentPage] = filepath;
        mPathTemporary[currentPage] = std::string();
        mTab->SetPageText( currentPage, openFileDialog.GetFilename() );
        
        Filestream::Write_Bin( pData.c_str(), pData.size(), filepath );    

        THROW_ERR_IFNOT( Filestream::Is_Exist( filepath ), "Problem saving a file, please contact developer!" );
        LOGALL( LEVEL_INFO, "File saved as: " + filepath, LOG_FILEPATH );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
        return false;
    }
    return true;
}

void TextField::AddNewTab( const std::string& name )
{
    mTextField.push_back( new wxStyledTextCtrl( mTab, wxID_ANY ) );
    mTab->AddPage( mTextField.back(), name );
    mTab->ChangeSelection( mTextField.size() - 1 );
}
