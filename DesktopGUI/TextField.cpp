#include "TextField.h"
#include <mutex>
#include <wx/filedlg.h>
#include <wx/process.h>
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "Feature/LogGUI.h"
#include "Feature/Config.h"
#include "Feature/Image.h"
#include "Frame/GotoFrame.h"
#include "Frame/FindFrame.h"
#include "FontEncoding.h"

//translation unit for static member
wxFrame* TextField::mParent;
wxAuiNotebook* TextField::mNotebook;
std::vector<sPageData> TextField::mPageData;
std::string TextField::SupportedFormat;
bool TextField::isGotoInit = false;
bool TextField::isFindInit = false;

wxCommandEvent NullCmdEvent = wxCommandEvent( wxEVT_NULL );
//contain only one tab or textfield, already temporary, and no content
#define IF_FIELD_UNOCCUPIED if ( mNotebook->GetPageCount() == 1 && mPageData[0].isTemporary && mPageData[0].TextField->IsEmpty() )

void TextField::Init( wxFrame * parent )
{
    mParent = parent;
    mNotebook = new wxAuiNotebook( parent );

    const int statusWidth[] = { -3, -1, 120, -1 };
    mParent->GetStatusBar()->SetFieldsCount( 4 );
    mParent->GetStatusBar()->SetStatusWidths( 4, statusWidth );

    mNotebook->Bind( wxEVT_AUINOTEBOOK_END_DRAG, TextField::OnPageDrag );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CLOSE, TextField::OnPageCloseButton );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CHANGED, TextField::OnPageChanged );
    mParent->Bind( wxEVT_STC_CHANGE, TextField::OnTextChanged );
    mParent->Bind( wxEVT_STC_ZOOM, TextField::OnSTCZoom );
    mParent->Bind( wxEVT_STC_UPDATEUI, TextField::OnUpdateUI );

    Filestream::Create_Directories( "temp" );
    InitFilehandle();
}

void TextField::MarginAutoAdjust()
{
    auto currentPage = mNotebook->GetSelection();
    auto tf = mPageData[currentPage].TextField;
    auto line = tf->GetLineCount();
    auto zoom = tf->GetZoom();
    double increment = std::floor( log( line ) / log( 10 ) );
    tf->SetMarginWidth( 0, 25 + 7 * increment + zoom * increment );
}

void TextField::InitFilehandle()
{
    SupportedFormat = Config::LoadSupportedFormat();
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
    LOGALL( LEVEL_TRACE, "Temporary file amount: " + TO_STR( vTempFile.size() ) );
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
    for ( const auto& i : mPageData )
        if ( i.isTemporary ) return true;
    return false;
}

bool TextField::ExistChangedFile()
{
    for ( const auto& i : mPageData )
        if ( i.isChanged ) return true;
    return false;
}

bool TextField::SaveToExit()
{
    //if exist non temp file and its changed then its not save to exit
    for ( const auto& i : mPageData )
        if ( !i.isTemporary && i.isChanged ) return false;
    return true;
}

void TextField::UpdateEOLString( wxStyledTextCtrl* stc, int mode )
{
    auto str = stc->GetValue();
    FontEncoding::AdjustEOLString( str, mode );
    stc->SetText( str );
    stc->SetEOLMode( mode );
    UpdateStatusEOL( stc );
}

void TextField::UpdateStatusEOL( wxStyledTextCtrl* stc )
{
    wxString status;
    auto mode = stc->GetEOLMode();
    if      ( mode == wxSTC_EOL_CR )   status = "Mac (CR)";
    else if ( mode == wxSTC_EOL_LF )   status = "Unix (LF)";
    else if ( mode == wxSTC_EOL_CRLF ) status = "Windows (CR-LF)";
    mParent->SetStatusText( status, 2 );
}

void TextField::UpdateStatusEncoding( wxStyledTextCtrl* stc )
{
    wxString status;
    status = FontEncoding::GetEncodingString( stc->GetFont().GetEncoding() );
    mParent->SetStatusText( status, 3 );
}

void TextField::UpdateStatusPos( wxStyledTextCtrl* stc )
{
    wxString status;
    auto line = stc->GetCurrentLine();
    auto pos = stc->GetCurrentPos();
    status = wxString::Format( "Line: %d, Pos: %d", line+1, pos );
    mParent->SetStatusText( status, 1 );
}

void TextField::FindText( wxStyledTextCtrl* stc, bool next )
{
    int flags = wxSTC_FIND_WHOLEWORD | wxSTC_FIND_MATCHCASE;
    auto text = stc->GetSelectedText();
    int selection = stc->GetSelectionStart();
    int atPos = wxNOT_FOUND;
    next ? stc->SetSelectionStart( selection + 1 ) : stc->SetSelectionStart( selection - 1 );
    stc->SearchAnchor();
    next ? atPos = stc->SearchNext( flags, text ) : atPos = stc->SearchPrev( flags, text );
    if ( atPos == wxNOT_FOUND )
    {
        stc->SetSelectionStart( selection );
        return;
    }
    stc->SetSelection( atPos, atPos + text.size() );
    stc->ScrollToLine( stc->GetCurrentLine() );
}

void TextField::OnUpdateUI( wxStyledTextEvent& event )
{
    static int beforePos = 0;
    auto currentPage = mNotebook->GetSelection();
    int curPos = mPageData[currentPage].TextField->GetCurrentPos();
    if ( beforePos != curPos ) UpdateStatusPos( mPageData[currentPage].TextField );
}

void TextField::OnTextChanged( wxStyledTextEvent& event )
{
    UpdateSaveIndicator( false );
    MarginAutoAdjust();
}

void TextField::OnSTCZoom( wxStyledTextEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    if ( currentPage == -1 ) return;
    auto zoom = mPageData[currentPage].TextField->GetZoom();
    if ( zoom > Config::mZoomMax ) OnZoomOut( NullCmdEvent );
    else if ( zoom < Config::mZoomMin ) OnZoomIn( NullCmdEvent );
    MarginAutoAdjust();
}

void TextField::OnPageCloseButton( wxAuiNotebookEvent& evt )
{
    evt.Veto();
    OnPageClose( NullCmdEvent );
}

void TextField::OnPageChanged( wxAuiNotebookEvent& evt )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateParentName();
    UpdateStatusEncoding( mPageData[currentPage].TextField );
    UpdateStatusEOL( mPageData[currentPage].TextField );
    UpdateStatusPos( mPageData[currentPage].TextField );
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

void TextField::OnTextSummary( wxCommandEvent& event )
{
    Util::Timer timer( "Text Summary", MS, false );
    auto currentPage = mNotebook->GetSelection();

    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( currentPage ) + '\n';
    message += "Temporary: ";
    message += mPageData[currentPage].isTemporary ? "Yes\n" : "No\n";
    message += "Characters: ";
    message += TO_STR( mPageData[currentPage].TextField->GetTextLength() ) + '\n';
    message += "Lines: ";
    message += TO_STR( mPageData[currentPage].TextField->GetLineCount() ) + '\n';
    message += "Last Created: ";
    message += Filestream::GetLastCreated( mPageData[currentPage].FilePath ) + '\n';
    message += "Last Modified: ";
    message += Filestream::GetLastModified( mPageData[currentPage].FilePath ) + '\n';
    message += "Filepath: ";
    message += mPageData[currentPage].FilePath;

    auto SumDialog = wxMessageDialog( mParent, message, "Text Summary", wxOK | wxSTAY_ON_TOP );
    LOGALL( LEVEL_TRACE, "Text Summary Dialog Created: " + TO_STR( timer.Toc() ) + "(ms)" );
    SumDialog.ShowModal();
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
    SumDialog.ShowModal();
}

void TextField::OnEOL_CR( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_CR );
    UpdateSaveIndicator( false );
}

void TextField::OnEOL_LF( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_LF );
    UpdateSaveIndicator( false );
}

void TextField::OnEOL_CRLF( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_CRLF );
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

    wxTextEntryDialog RenameDlg( mParent, "Insert new name:", "Rename File", mNotebook->GetPageText( currentPage ) );
    if ( RenameDlg.ShowModal() == wxID_OK )
    {
        uint32_t len = mNotebook->GetPageText( currentPage ).Len();
        std::string newName = std::string( RenameDlg.GetValue().mb_str() );
        std::string oldPath = std::string( mPageData[currentPage].FilePath.begin(), mPageData[currentPage].FilePath.end() - len );

        if ( Filestream::Is_Exist( oldPath + newName ) )
        {
            auto prompt = wxMessageDialog( mParent, "File with that name already exist!\n"
                                           "Are you sure want to overwrite?", "Overwrite", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }
        Filestream::Rename_File( oldPath + newName, mPageData[currentPage].FilePath );
        mPageData[currentPage].FilePath = oldPath + newName;

        mNotebook->SetPageText( currentPage, RenameDlg.GetValue() );
        UpdateParentName();
        LOGALL( LEVEL_TRACE, "Rename file to: " + newName );
    }
}

void TextField::OnOpenFile( wxCommandEvent& event )
{
    wxFileDialog FileDialog( mParent, "Open File", wxEmptyString, wxEmptyString, 
                             SupportedFormat, wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if ( FileDialog.ShowModal() == wxID_CANCEL ) return;
    
    try
    {
        Util::Timer Tm( "Open File", MS, false );

        std::string filepath = std::string( FileDialog.GetPath().mb_str() );
        for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
            if ( filepath == mPageData[i].FilePath )
            {
                LOGCONSOLE( LEVEL_INFO, "File is already opened: " + filepath );
                mNotebook->ChangeSelection( i );
                return;
            }

        auto vRead = FormatOpen( filepath );
        THROW_ERR_IFEMPTY( vRead, "Cannot open file " + filepath );
        LOGALL( LEVEL_TRACE, "Opened Filepath: " + filepath );

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

        auto stc = mPageData.back().TextField;
        wxString str = wxString::FromUTF8( (char*) &vRead[0], vRead.size() );
        stc->SetEOLMode( FontEncoding::GetEOLMode( str ) );
        stc->AppendText( str );

        // update indicator
        UpdateStatusEncoding( stc );
        UpdateStatusEOL( stc );
        UpdateStatusPos( stc );
        UpdateParentName();
        UpdateSaveIndicator( true );

        std::string readSize = "Document size: " + std::to_string( vRead.size() ) + " (bytes)";
        LOGALL( LEVEL_TRACE, readSize );
        LOGALL( LEVEL_TRACE, "Opening file time: " + TO_STR( Tm.Toc() ) + " (ms)" );
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
            wxMessageDialog prompt( mParent, "Any changes to the file will be ignored.\n"
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
        wxMessageDialog prompt( mParent, "Any changes to the file will be ignored.\n"
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

void TextField::OnSaveFile( wxCommandEvent& event )
{
    try
    {
        auto currentPage = mNotebook->GetSelection();
        THROW_ERR_IF( currentPage == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        if ( !mPageData[currentPage].isChanged ) return;
        if ( mPageData[currentPage].isTemporary )
        {
            OnSaveFileAs( NullCmdEvent );
            return;
        }
        // get string from current textfield
        std::string pData = std::string( mPageData[currentPage].TextField->GetText() );

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

        wxFileDialog FileDialog( mParent, "Save As", wxEmptyString, mNotebook->GetPageText( currentPage ), 
                                 SupportedFormat, wxFD_SAVE );
        if ( FileDialog.ShowModal() == wxID_CANCEL ) return;

        std::string pData = std::string( mPageData[currentPage].TextField->GetText() );

        //if file is a temporary delete temp
        if ( mPageData[currentPage].isTemporary )
            Filestream::Delete_File( mPageData[currentPage].FilePath );

        std::string filepath = std::string( FileDialog.GetPath().mb_str() );
        mPageData[currentPage].FilePath = filepath;
        mPageData[currentPage].isTemporary = false;
     
        FormatSave( pData, filepath );
        THROW_ERR_IFNOT( Filestream::Is_Exist( filepath ), "Problem saving a file, please contact developer!" );

        mNotebook->SetPageText( currentPage, FileDialog.GetFilename() );
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
    mPageData[currentPage].TextField->Redo();
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

void TextField::OnFind( wxCommandEvent& event )
{
    if ( !isFindInit )
    {
        FindFrame::Init( mParent );
        isFindInit = true;
    }
    auto currentPage = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[currentPage].TextField, wxString::FromUTF8( mPageData[currentPage].FilePath ) );
    FindFrame::ShowAndFocus( true );
}

void TextField::OnReplace( wxCommandEvent& event )
{
    if ( !isFindInit )
    {
        FindFrame::Init( mParent );
        isFindInit = true;
    }
    auto currentPage = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[currentPage].TextField, wxString::FromUTF8( mPageData[currentPage].FilePath ) );
    FindFrame::ShowAndFocus( false );
}

void TextField::OnGoto( wxCommandEvent& event )
{
    if ( !isGotoInit )
    {
        GotoFrame::Init( mParent );
        isGotoInit = true;
    }
    auto currentPage = mNotebook->GetSelection();
    GotoFrame::UpdateInfo( mPageData[currentPage].TextField );
    GotoFrame::ShowAndFocus();
}

void TextField::OnFindNext( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    FindText( mPageData[currentPage].TextField, true );
}

void TextField::OnFindPrev( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    FindText( mPageData[currentPage].TextField, false );
}

void TextField::AddNewTab( sPageData& pd )
{
    pd.TextField = new wxStyledTextCtrl( mNotebook );
    pd.TextField->SetZoom( Config::mZoomDefault );
    pd.TextField->SetScrollWidth( 1 ); //avoid large horizontal scroll width by default
    LoadStyle( pd.TextField );
    UpdateStatusEOL( pd.TextField );
    UpdateStatusPos( pd.TextField );
    UpdateStatusEncoding( pd.TextField );
    mNotebook->AddPage( pd.TextField, Filestream::getFileName( pd.FilePath ), true );
}

void TextField::LoadStyle( wxStyledTextCtrl* stc )
{
    stc->StyleSetForeground( wxSTC_STYLE_DEFAULT, *wxWHITE ); //foreground such as text
    stc->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( 35, 35, 35 ) ); //background for field

    stc->StyleClearAll();
    stc->SetCaretLineVisible( true ); //set caret line background visible
    stc->SetCaretLineBackground( wxColour( 70, 70, 70 ) ); //caret line background
    stc->SetCaretForeground( wxColour( 221, 151, 204 ) ); //caret colour

    stc->SetSelBackground( true, wxColour( 58, 119, 201 ) ); //background of selected text

    stc->SetMarginWidth( 0, 25 );
    stc->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    stc->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color
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

        auto proc = wxProcess::Open( cmd );
        if ( proc != nullptr )
        {           
            wxDialog dialog( mParent, -1, "Compress", wxDefaultPosition, wxSize( 200, 50 ), wxSTAY_ON_TOP | wxCAPTION );
            dialog.Show();
            while ( wxProcess::Exists( proc->GetPid() ) );
            Filestream::Delete_File( "temp/out" );
        }
        else
        {
            LOGALL( LEVEL_ERROR, "Cannot create process id from FormatSave()!" );
        }
    }
    else
    {
        Filestream::Write_Bin( sData.c_str(), sData.size(), filepath );
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

        auto proc = wxProcess::Open( cmd );
        if ( proc != nullptr )
        {
            wxDialog dialog( mParent, -1, "Decompress", wxDefaultPosition, wxSize( 200, 50 ), wxSTAY_ON_TOP | wxCAPTION );
            dialog.Show();
            while ( wxProcess::Exists( proc->GetPid() ) );
            vRead = Filestream::Read_Bin( "temp/in" );
            Filestream::Delete_File( "temp/in" );
        }
        else
        {
            LOGALL( LEVEL_ERROR, "Cannot create process id from FormatOpen()!" );
        }
    }
    else
    {
       vRead = Filestream::Read_Bin( filepath );
    }
    return vRead;
}

void TextField::UpdateParentName()
{
    auto currentPage = mNotebook->GetSelection();
    wxString title = APP_NAME;
    if ( !mPageData[currentPage].isTemporary )
    {
        title += " - ";
        title += mPageData[currentPage].FilePath;
    }
    mParent->SetTitle( title );
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