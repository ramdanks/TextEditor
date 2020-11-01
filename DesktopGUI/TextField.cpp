#include "TextField.h"
#include <wx/filedlg.h>
#include "LogGUI.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "Config.h"
#include "Image.h"
#include "GotoFrame.h"
#include "FindFrame.h"
#include <mutex>


//translation unit for static member
wxWindow*                       TextField::mParent;
wxAuiNotebook*                  TextField::mNotebook;
std::vector<sPageData>          TextField::mPageData;
std::string                     TextField::SupportedFormat;
wxCommandEvent                  TextField::NullCmdEvent = wxCommandEvent( wxEVT_NULL );

//contain only one tab or textfield, already temporary, and no content
#define IF_FIELD_UNOCCUPIED if ( mNotebook->GetPageCount() == 1 && mPageData[0].isTemporary && mPageData[0].TextField->IsEmpty() )

void TextField::Init( wxWindow * parent )
{
    mParent = parent;
    mNotebook = new wxAuiNotebook( parent );
    GotoFrame::Init( parent );
    FindFrame::Init( parent );

    mNotebook->Bind( wxEVT_AUINOTEBOOK_END_DRAG, TextField::OnPageDrag );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CLOSE, TextField::OnPageCloseButton );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CHANGED, TextField::OnPageChanged );
    mParent->Bind( wxEVT_STC_CHANGE, TextField::OnTextChanged );

    Filestream::Create_Directories( "temp" );
    InitFilehandle();
}

void TextField::InitFilehandle()
{
    SupportedFormat = "All types (*.*)|*.*";
    SupportedFormat += "|Normal text file (*.txt)|*.txt";
    SupportedFormat += "|Memoriser file (*.mtx)|*.mtx";
    SupportedFormat += "|Memoriser dictionary (*.mdt)|*.mdt";
    SupportedFormat += "|Memoriser archive (*.mac)|*.mac";
}

void TextField::FetchTempFile()
{
    auto vTempFile = Filestream::File_List( "temp" );
    if ( vTempFile.size() == 0 )
        OnNewFile( NullCmdEvent );
    else
    {
        mPageData.reserve( vTempFile.size() );
        for ( uint32_t i = 0; i < vTempFile.size(); i++ )
        {
            mPageData.push_back( { false, true, vTempFile[i], nullptr } );
            AddNewTab( mPageData.back() );
            auto vRead = Filestream::Read_Bin( vTempFile[i] );
            if ( !vRead.empty() ) 
                mPageData.back().TextField->AppendText( wxString( &vRead[0], vRead.size() ) );
            UpdateSaveIndicator( false );
        }
    }
    LOGALL( LEVEL_TRACE, "Temporary file amount: " + std::to_string( vTempFile.size() ) );
}

void TextField::CreateTempFile( const std::string& name )
{
    Filestream::Create_Directories( "temp" );
    Filestream::Write_Bin( nullptr, NULL, "temp\\" + name );
}

void TextField::SaveTempAll()
{
    auto mutex = std::mutex();
    mutex.lock();
    for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
    {
        if ( mPageData[i].isTemporary )
        {
            std::string temp = std::string( mPageData[i].TextField->GetText().mb_str( wxConvUTF8 ) );
            Filestream::Write_Bin( temp.c_str(), temp.size(), mPageData[i].FilePath );
        }
    }
    mutex.unlock();
}

bool TextField::ExistTempFile()
{
    for ( auto i : mPageData )
        if ( i.isTemporary ) return true;
    return false;
}

bool TextField::ExistChangedFile()
{
    for ( auto i : mPageData )
        if ( i.isChanged ) return true;
    return false;
}

bool TextField::SaveToExit()
{
    //if exist non temp file and its changed then its not save to exit
    for ( auto i : mPageData )
        if ( !i.isTemporary && i.isChanged ) return false;
    return true;
}

void TextField::OnTextSummary( wxCommandEvent& event )
{
    Util::Timer timer( "Text Summary", MS, false );
    auto currentPage = mNotebook->GetSelection();

    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( currentPage ) + '\n';
    message += "Temporary: ";
    message += mPageData[currentPage].isTemporary ? "Yes\n" : "No\n";
    message += "Filepath: ";
    message += mPageData[currentPage].FilePath + '\n';
    message += "Characters: ";
    message += TO_STR( mPageData[currentPage].TextField->GetTextLength() ) + '\n';
    message += "Lines: ";
    message += TO_STR( mPageData[currentPage].TextField->GetLineCount() ) + '\n';
    message += "Last Created: ";
    message += Filestream::GetLastCreated( mPageData[currentPage].FilePath ) + '\n';
    message += "Last Modified: ";
    message += Filestream::GetLastModified( mPageData[currentPage].FilePath );

    auto SumDialog = wxMessageDialog( mParent, message, "Text Summary", wxOK | wxSTAY_ON_TOP );
    LOGALL( LEVEL_TRACE, "Text Summary Dialog Created: " + TO_STR( timer.Toc() ) + "(ms)" );
    if ( SumDialog.ShowModal() == wxID_OK || SumDialog.ShowModal() == wxID_EXIT )
        SumDialog.Close( true );
}

void TextField::OnCompSummary( wxCommandEvent& event )
{
    Util::Timer timer( "Compression Summary", MS, false );
    auto currentPage = mNotebook->GetSelection();

    bool isCompressedFile = false;
    if ( Filestream::FileExtension( mPageData[currentPage].FilePath ) == "mtx" )
        isCompressedFile = true;

    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( currentPage ) + '\n';
    message += "Compressed: ";
    message += isCompressedFile ? "Yes" : "No";
    if ( isCompressedFile )
    {
        auto info = Filestream::GetCompressedInfo( mPageData[currentPage].FilePath );
        message += "\nClusters: ";
        message += TO_STR( info.ClusterSize ) + '\n';
        message += "Addressing Size: ";
        message += TO_STR( info.AddressingSize ) + '\n';
        message += "Original Size: ";
        message += TO_STR( mPageData[currentPage].TextField->GetTextLength() ) + '\n';
        message += "Compressed Size: ";
        message += TO_STR( info.CompressedSize );
    }

    auto SumDialog = wxMessageDialog( mParent, message, "Compression Summary", wxOK | wxSTAY_ON_TOP );
    LOGALL( LEVEL_TRACE, "Compression Summary Dialog Created: " + TO_STR( timer.Toc() ) + "(ms)" );
    if ( SumDialog.ShowModal() == wxID_OK || SumDialog.ShowModal() == wxID_EXIT )
        SumDialog.Close( true );
}

void TextField::OnFind( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[currentPage].TextField, wxString::FromUTF8( mPageData[currentPage].FilePath ) );
    FindFrame::ShowAndFocus( true );
}

void TextField::OnReplace( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[currentPage].TextField, wxString::FromUTF8( mPageData[currentPage].FilePath ) );
    FindFrame::ShowAndFocus( false );
}

void TextField::OnGoto( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    GotoFrame::UpdateInfo( mPageData[currentPage].TextField );
    GotoFrame::ShowAndFocus();
}

void TextField::OnTextChanged( wxStyledTextEvent& event )
{
    UpdateSaveIndicator( false );
}

void TextField::OnNewFile( wxCommandEvent& event )
{
    std::string tempName;
    for ( uint32_t counter = 1; ; counter++ )
    {
        tempName = "new" + std::to_string( counter ) + ".tmp";
        if ( !Filestream::Is_Exist( "temp\\" + tempName ) ) break;
    }
    CreateTempFile( tempName );

    //insert path
    mPageData.push_back( { false, true, "temp\\" + tempName, nullptr } );
    AddNewTab( mPageData.back() );
    UpdateSaveIndicator( false );
    UpdateParentName();
}

void TextField::OnRenameFile( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();

    //if its a temporary file redirect to save as
    if ( mPageData[currentPage].isTemporary ) 
    { 
        OnSaveFileAs( NullCmdEvent ); 
        return; 
    }

    auto mRenameDlg = new wxTextEntryDialog( mParent, "Insert new name:", "Rename File", mNotebook->GetPageText( currentPage ) );
    if ( mRenameDlg->ShowModal() == wxID_OK )
    {
        uint32_t len = mNotebook->GetPageText( currentPage ).Len();
        std::string newName = std::string( mRenameDlg->GetValue().mb_str() );
        std::string oldPath = std::string( mPageData[currentPage].FilePath.begin(), mPageData[currentPage].FilePath.end() - len );

        if ( Filestream::Is_Exist( oldPath + newName ) )
        {
            auto prompt = wxMessageDialog( mParent, "File with that name already exist!\n"
                                           "Are you sure want to overwrite?", "Overwrite", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }
        Filestream::Rename_File( oldPath + newName, mPageData[currentPage].FilePath );
        mPageData[currentPage].FilePath = oldPath + newName;

        mNotebook->SetPageText( currentPage, mRenameDlg->GetValue() );
        UpdateParentName();
        LOGALL( LEVEL_TRACE, "Rename file to: " + newName );
    }
}

void TextField::OnOpenFile( wxCommandEvent& event )
{
    wxFileDialog openFileDialog( mParent, _( "Open Text File" ), "", "", SupportedFormat, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;

    try
    {
        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
            if ( filepath == mPageData[i].FilePath )
            {
                LOGCONSOLE( LEVEL_INFO, "File is already opened: " + filepath );
                mNotebook->ChangeSelection( i );
                return;
            }

        auto vRead = FormatOpen( filepath );
        THROW_ERR_IF( vRead.empty(), std::string( "Cannot open file " + filepath ) );
        LOGALL( LEVEL_TRACE, std::string( "Opened Filepath: " + filepath ) );

        IF_FIELD_UNOCCUPIED
        {
            mPageData[0].isChanged = false;
            mPageData[0].isTemporary = false;
            mPageData[0].FilePath = filepath;
            mNotebook->SetPageText( 0, Filestream::getFileName( filepath ) );
        }
        else
        {
            mPageData.push_back( { false, false, filepath, nullptr } );           
            AddNewTab( mPageData.back() );
        }

        mPageData.back().TextField->AppendText( wxString::FromUTF8( (char*) &vRead[0], vRead.size() ) );
        UpdateSaveIndicator( true );
        UpdateParentName();

        std::string readSize = "File size: " + std::to_string( vRead.size() ) + "(bytes)";
        LOGALL( LEVEL_TRACE, readSize );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnPageClose( wxCommandEvent& evt )
{
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        //if that page is changed then ask first
        if ( mPageData[currentPage].isChanged )
        {
            auto prompt = wxMessageDialog( mParent, "Any changes to the file will be ignored.\n"
                                           "Sure want to Continue ?", "Close Page", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }

         if ( mPageData[currentPage].isTemporary ) //delete temp file on close
             Filestream::Delete_File( mPageData[currentPage].FilePath );

         if ( mNotebook->GetPageCount() == 1 )
         {
             mNotebook->SetPageText( 0, "new1.tmp" );
             mPageData[0].TextField->ClearAll();
             CreateTempFile( "new1.tmp" );
             mPageData[0].isTemporary = true;
             mPageData[0].FilePath = "temp\\new1.tmp";
         }
         else
         {
             mNotebook->DeletePage( currentPage );
             mPageData.erase( mPageData.begin() + currentPage );
         }
         UpdateParentName();     
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnPageCloseAll( wxCommandEvent& event )
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
                if ( mPageData[i].isTemporary ) //delete temp file
                    Filestream::Delete_File( mPageData[i].FilePath );
            }
            mNotebook->DeleteAllPages();
            mPageData.clear();
            mPageData.push_back( { false, true, "temp\\new1.tmp", nullptr } );
            AddNewTab( mPageData.back() );
            UpdateSaveIndicator( false );
            CreateTempFile( "new1.tmp" );
            UpdateParentName();
        }
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnPageCloseButton( wxAuiNotebookEvent& evt )
{
    evt.Veto();
    OnPageClose( NullCmdEvent );
}

void TextField::OnPageChanged( wxAuiNotebookEvent& evt )
{
    UpdateParentName();
}

void TextField::OnPageDrag( wxAuiNotebookEvent& evt )
{
    auto currentPage = evt.GetSelection();
    auto currentWindow = mNotebook->GetPage( currentPage );

    if ( currentWindow != mPageData[currentPage].TextField )
    {
        //find the origin index of page that being dragged
        uint32_t i = 0; 
        for ( i = 0; i < mPageData.size(); i++ )
            if ( currentWindow == mPageData[i].TextField ) break;
        
        //original page data
        sPageData temp = mPageData[i];
        mPageData.erase( mPageData.begin() + i );
        mPageData.insert( mPageData.begin() + currentPage, temp );
    }
    mNotebook->SetSelection( currentPage );
}

void TextField::OnSaveFile( wxCommandEvent& event )
{
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        //if nothing changed then return
        if ( !mPageData[currentPage].isChanged ) return;
        //if a temporary file, save as
        if ( mPageData[currentPage].isTemporary )
        {
            OnSaveFileAs( NullCmdEvent );
            return;
        }
        // get string from current textfield
        std::string pData = std::string( mPageData[currentPage].TextField->GetText().mb_str( wxConvUTF8 ) );

        FormatSave( pData, mPageData[currentPage].FilePath );
        UpdateSaveIndicator( true );
        LOGALL( LEVEL_INFO, "File saved: " + mPageData[currentPage].FilePath );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnSaveFileAll( wxCommandEvent& event )
{
    for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
    {     
        mNotebook->SetSelection( i );
        if ( mPageData[i].isTemporary )  OnSaveFileAs( NullCmdEvent );
        else                             OnSaveFile( NullCmdEvent );
    }
}

void TextField::OnSaveFileAs( wxCommandEvent& event )
{
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        wxFileDialog openFileDialog( mParent, _( "Save As" ), "", mNotebook->GetPageText( currentPage ), SupportedFormat, wxFD_SAVE );
        if ( openFileDialog.ShowModal() == wxID_CANCEL ) return;

        std::string pData = std::string( mPageData[currentPage].TextField->GetText().mb_str( wxConvUTF8 ) );
        THROW_ERR_IFEMPTY( pData, "Cannot extract text from Text Field!" );

        //if file is a temporary delete temp
        if ( mPageData[currentPage].isTemporary )
            Filestream::Delete_File( mPageData[currentPage].FilePath );

        std::string filepath = std::string( openFileDialog.GetPath().mb_str() );
        mPageData[currentPage].FilePath = filepath;
        mPageData[currentPage].isTemporary = false;
     
        FormatSave( pData, filepath );
        THROW_ERR_IFNOT( Filestream::Is_Exist( filepath ), "Problem saving a file, please contact developer!" );

        mNotebook->SetPageText( currentPage, openFileDialog.GetFilename() );
        UpdateSaveIndicator( true );
        UpdateParentName();
        LOGALL( LEVEL_INFO, "File saved as: " + filepath );
    }
    catch ( Util::Err& e )
    {
        LOGALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnUndo( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->Undo();
}

void TextField::OnRedo( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->Undo();
}

void TextField::OnCut( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->Cut();
}

void TextField::OnCopy( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->Copy();
}

void TextField::OnPaste( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->Paste();
}

void TextField::OnDelete( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->DeleteBack();
}

void TextField::OnSelectAll( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->SelectAll();
}

void TextField::OnZoomIn( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    if ( mPageData[currentPage].TextField->GetZoom() >= Config::mZoomMax ) return;
    mPageData[currentPage].TextField->ZoomIn();
}

void TextField::OnZoomOut( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    if ( mPageData[currentPage].TextField->GetZoom() <= Config::mZoomMin ) return;
    mPageData[currentPage].TextField->ZoomOut();
}

void TextField::OnZoomRestore( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    mPageData[currentPage].TextField->SetZoom( Config::mZoomDefault );
}

void TextField::OnSTCZoom( wxStyledTextEvent& event )
{
}

void TextField::AddNewTab( sPageData& pd )
{
    pd.TextField = new wxStyledTextCtrl( mNotebook, wxID_ANY );
    pd.TextField->SetZoom( Config::mZoomDefault );
    LoadStyle( pd.TextField );
    mNotebook->AddPage( pd.TextField, Filestream::getFileName( pd.FilePath ), true );
}

void TextField::LoadStyle( wxStyledTextCtrl* stc )
{
    stc->StyleSetForeground( wxSTC_STYLE_DEFAULT, *wxWHITE ); //foreground such as text
    stc->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 30, 30, 30 ) ); //background for field

    stc->StyleClearAll();
    stc->SetCaretLineVisible( true ); //set caret line background visible
    stc->SetCaretLineBackground( wxColour( 70, 70, 70 ) ); //caret line background
    stc->SetCaretForeground( wxColour( 221, 151, 204 ) ); //caret colour

    stc->SetSelBackground( true, wxColour( 58, 119, 201 ) ); //background of selected text

    stc->SetMarginWidth( 0, 35 );
    stc->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    stc->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 60, 60, 60 ) ); //linenumber back color
    stc->StyleSetForeground( wxSTC_STYLE_LINENUMBER, wxColour( 221, 151, 204 ) ); //linenumber fore color
    //stc->StyleSetSpec( wxSTC_STYLE_LINENUMBER, "fore:#dd96cc,back:#3c3c3c" ); //linenumber color with one func
}

void TextField::FormatSave( const std::string& sData, const std::string& filepath )
{
    auto format = Filestream::FileExtension( filepath );
    if ( format == "mtx" )
    {
        Filestream::Write_Bin( sData.c_str(), sData.size(), "temp/out" );
        #if defined( __WIN32__ )
        std::string cmd = "Compressor -c temp/out " + filepath + " -mt";
        #elif defined ( __linux__ )
        std::string cmd = "./Compressor -c temp/out " + filepath + " -mt";
        #endif
        system( cmd.c_str() );
        Filestream::Delete_File( "temp/out" );
    }
    else
    {
        Filestream::Write_Bin( sData.c_str(), sData.size(), filepath );
    }
}

void TextField::UpdateParentName()
{
    auto currentPage = mNotebook->GetSelection();
    if ( mPageData[currentPage].isTemporary )
    {
        mParent->SetLabel( "Mémoriser" );
        return;
    }
    else
    {
        if ( !mPageData[currentPage].isTemporary )
            mParent->SetLabel( "Mémoriser - " + mPageData[currentPage].FilePath );
        else
            mParent->SetLabel( "Mémoriser" );
    }
}

void TextField::UpdateSaveIndicator( bool save )
{
    auto currentPage = mNotebook->GetSelection();
    if ( save && mPageData[currentPage].isChanged )
    {
        mPageData[currentPage].isChanged = false;
        mNotebook->SetPageBitmap( currentPage, IMG_SAVE );
    }
    else if ( !save && !mPageData[currentPage].isChanged )
    {
        mPageData[currentPage].isChanged = true;
        mNotebook->SetPageBitmap( currentPage, IMG_UNSAVE );
    }
}

std::vector<uint8_t> TextField::FormatOpen( const std::string& filepath )
{
    auto format = Filestream::FileExtension( filepath );
    std::vector<uint8_t> vRead;
    if ( format == "mtx" )
    {
        #if defined( __WIN32__ )
        std::string cmd = "Compressor -d " + filepath + " temp/in -mt";
        #elif defined ( __linux__ )
        std::string cmd = "./Compressor -d " + filepath + " temp/in -mt";
        #endif
        system( cmd.c_str() );
        vRead = Filestream::Read_Bin( "temp/in" );
        Filestream::Delete_File( "temp/in" );
    }
    else
    {
       vRead = Filestream::Read_Bin( filepath );
    }
    return vRead;
}