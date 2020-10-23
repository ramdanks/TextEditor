#include "TextField.h"
#include <wx/filedlg.h>
#include "LogGUI.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"
#include "Config.h"

//contain only one tab or textfield, already temporary, and no content
#define IF_FIELD_UNOCCUPIED if ( mTextField.size() == 1 && mPathAbsolute[0].empty() && mTextField[0]->IsEmpty() )
#define IF_TEMPORARY_FIELD(x) if ( mPathAbsolute[x].empty() )

//translation unit for static member
std::string                         TextField::SupportedFormat = "";
wxWindow*                           TextField::mParent = nullptr;
wxAuiNotebook*                      TextField::mNotebook = nullptr;
std::vector<wxStyledTextCtrl*>      TextField::mTextField;
std::vector<std::string>            TextField::mPathAbsolute;
std::vector<std::string>            TextField::mPathTemporary;

void TextField::InitFilehandle( wxWindow* parent )
{
    mParent = parent;
    mNotebook = new wxAuiNotebook( parent, wxID_ANY );
    Filestream::Create_Directories( "temp" );

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
        OnNewFile();
    else
    {
        mTextField.reserve( mPathTemporary.size() );
        mPathAbsolute.resize( mPathTemporary.size() ); //empty path because temp doesnt have absolute path
        for ( uint32_t i = 0; i < mPathTemporary.size(); i++ )
        {
            AddNewTab( Filestream::getFileName( mPathTemporary[i] ) );
            auto vRead = Filestream::Read_Bin( mPathTemporary[i] );
            if ( !vRead.empty() ) mTextField.back()->AppendText( wxString( &vRead[0], vRead.size() ) );
        }
    }
    LOGALL( LEVEL_TRACE, "Temporary file amount: " + std::to_string( mPathTemporary.size() ), LOG_FILEPATH );
}

void TextField::CreateTempFile( const std::string& name )
{
    Filestream::Create_Directories( "temp" );
    Filestream::Write_Bin( nullptr, NULL, "temp\\" + name );
}

void TextField::OnNewFile()
{
    std::string tempName;
    for ( uint32_t counter = 1; ; counter++ )
    {
        tempName = "new" + std::to_string( counter ) + ".tmp";
        if ( !Filestream::Is_Exist( "temp\\" + tempName ) ) break;
    }
    CreateTempFile( tempName );
    AddNewTab( tempName );

    //insert path
    mPathAbsolute.push_back( std::string() );
    mPathTemporary.push_back( "temp\\" + tempName );
}

void TextField::OnRenameFile()
{
    auto currentPage = mNotebook->GetSelection();
    IF_TEMPORARY_FIELD( currentPage ) { OnSaveFileAs(); return; }

    auto mRenameDlg = new wxTextEntryDialog( mParent, "Insert new name:", "Rename File", mNotebook->GetPageText( currentPage ) );
    if ( mRenameDlg->ShowModal() == wxID_OK )
    {
        uint32_t len = mNotebook->GetPageText( currentPage ).Len();
        std::string newName = std::string( mRenameDlg->GetValue().mb_str() );
        std::string oldPath = std::string( mPathAbsolute[currentPage].begin(), mPathAbsolute[currentPage].end() - len );

        if ( Filestream::Is_Exist( oldPath + newName ) )
        {
            auto prompt = wxMessageDialog( mParent, "File with that name already exist!\n"
                                           "Are you sure want to overwrite?", "Overwrite", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }
        Filestream::Rename_File( oldPath + newName, mPathAbsolute[currentPage] );
        mNotebook->SetPageText( currentPage, mRenameDlg->GetValue() );
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
                mNotebook->ChangeSelection( i );
                return;
            }

        IF_FIELD_UNOCCUPIED
        {
            Filestream::Delete_File( mPathTemporary[0] );
            mPathAbsolute[0] = filepath;
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

        auto format = Filestream::FileExtension( filepath );

        if ( !mTextField.back()->IsEmpty() ) AddNewTab( Filestream::getFileName( filepath ) );
        else mNotebook->SetPageText( mTextField.size() - 1, Filestream::getFileName( filepath ) );
        mTextField.back()->AppendText( wxString::FromUTF8( (const char*) &vRead[0], vRead.size() ) );

        std::string readSize = "File size: " + std::to_string( vRead.size() ) + "(bytes)";
        LOGALL( LEVEL_TRACE, readSize, LOG_FILEPATH );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), ERR_FILEPATH );
    }
}

void TextField::OnPageClose()
{
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        auto prompt = wxMessageDialog( mParent, "Any changes to the file will be ignored.\n"
                                      "Sure want to Continue ?", "Close Page", wxYES_NO );
        if ( prompt.ShowModal() == wxID_YES )
        {
            if ( mPathAbsolute[currentPage].empty() ) //delete temp file on close
                Filestream::Delete_File( mPathTemporary[currentPage] );

            if ( mNotebook->GetPageCount() == 1 )
            {
                ClearPage( 0, "new1.tmp" );
                CreateTempFile( "new1.tmp" );
            }
            else
            {
                mNotebook->DeletePage( currentPage );
                mTextField.erase( mTextField.begin() + currentPage );
                mPathTemporary.erase( mPathTemporary.begin() + currentPage );
                mPathAbsolute.erase( mPathAbsolute.begin() + currentPage );
            }
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::OnPageCloseAll()
{
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto prompt = wxMessageDialog( mParent, "Any changes to the file will be ignored.\n"
                                       "This action also delete all temporary file. Sure want to Continue ?",
                                       "Close All Window", wxYES_NO );
        if ( prompt.ShowModal() == wxID_YES )
        {
            for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
            {
                if ( mPathAbsolute[i].empty() ) //delete temp file
                    Filestream::Delete_File( mPathTemporary[i] );
            }
            mNotebook->DeleteAllPages();
            mTextField.clear();
            mPathTemporary.clear();
            mPathAbsolute.clear();

            mPathTemporary.push_back( "temp\\new1.tmp" );
            mPathAbsolute.push_back( std::string() );
            AddNewTab( "new1.tmp" );
            CreateTempFile( "new1.tmp" );
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
    }
}

void TextField::SaveTempAll()
{
    for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
    {
        std::string temp = std::string( mTextField[i]->GetText().mb_str( wxConvUTF8 ) );
        Filestream::Write_Bin( temp.c_str(), temp.size(), mPathTemporary[i] );
    }
}

bool TextField::ExistAbsoluteFile()
{
    for ( auto i : mPathAbsolute )
        if ( !i.empty() ) return true;
    return false;
}

void TextField::OnSaveFile()
{
    try
    {
        auto currentPage = mNotebook->GetSelection();
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
        mNotebook->SetSelection( i );
        IF_TEMPORARY_FIELD( i ) OnSaveFileAs();   
        else                    OnSaveFile();
    }
}

bool TextField::OnSaveFileAs()
{
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        wxFileDialog openFileDialog( mParent, _( "Save As" ), "", mNotebook->GetPageText( currentPage ), SupportedFormat, wxFD_SAVE );
        if ( openFileDialog.ShowModal() == wxID_CANCEL ) return false;

        std::string pData = std::string( mTextField[currentPage]->GetText().mb_str( wxConvUTF8 ) );
        THROW_ERR_IF( pData.empty(), "Cannot extract text from Text Field!" );

        //if file is a temporary delete temp
        if ( mPathAbsolute[currentPage].empty() )
            Filestream::Delete_File( mPathTemporary[currentPage] );

        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        mPathAbsolute[currentPage] = filepath;
        mPathTemporary[currentPage] = std::string();
        
        Filestream::Write_Bin( pData.c_str(), pData.size(), filepath );    
        THROW_ERR_IFNOT( Filestream::Is_Exist( filepath ), "Problem saving a file, please contact developer!" );
        mNotebook->SetPageText( currentPage, openFileDialog.GetFilename() );

        LOGALL( LEVEL_INFO, "File saved as: " + filepath, LOG_FILEPATH );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek(), LOG_FILEPATH );
        return false;
    }
    return true;
}

void TextField::OnZoom( bool zoomIn, bool reset )
{
    auto currentPage = mNotebook->GetSelection();
    if ( reset )
        mTextField[currentPage]->SetZoom( Config::mZoomDefault );

    if ( zoomIn )
    {
        if ( mTextField[currentPage]->GetZoom() >= Config::mZoomMax ) return;
        mTextField[currentPage]->ZoomIn();
    }
    else
    {
        if ( mTextField[currentPage]->GetZoom() <= Config::mZoomMin ) return;
        mTextField[currentPage]->ZoomOut();
    }
}

void TextField::AddNewTab( const std::string& name )
{
    mTextField.push_back( new wxStyledTextCtrl( mNotebook, wxID_ANY ) );
    mNotebook->AddPage( mTextField.back(), name, true );
}

void TextField::ClearPage( size_t page, const std::string& name )
{
    if ( !name.empty() ) mNotebook->SetPageText( page, name );
    mTextField[page]->ClearAll();
}