#include "TextField.h"
#include "../Utilities/Err.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "Feature/FilehandleGUI.h"
#include "Feature/LogGUI.h"
#include "Feature/Config.h"
#include "Feature/Image.h"
#include "Frame/GotoFrame.h"
#include "Frame/FindFrame.h"
#include "Frame/DictionaryFrame.h"
#include "FontEncoding.h"
#include <wx/busyinfo.h>
#include <mutex>

//translation unit for static member
wxFrame* TextField::mParent;
wxAuiNotebook* TextField::mNotebook;
std::vector<sPageData> TextField::mPageData;
bool TextField::isGotoInit = false;
bool TextField::isFindInit = false;
bool TextField::isDictInit = false;

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
}

void TextField::MarginAutoAdjust()
{
    auto currentPage = mNotebook->GetSelection();
    auto tf = mPageData[currentPage].TextField;
    auto line = tf->GetLineCount();
    auto zoom = tf->GetZoom();
    double increment = std::floor( log( line ) / log( 10 ) );
    tf->SetMarginWidth( 0, 27 + 7 * increment + zoom * increment );
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
    LOG_ALL_FORMAT( LEVEL_TRACE, "Temporary file amount: %llu", vTempFile.size() );
}

void TextField::CreateTempFile( const std::string& name )
{
    Filestream::Create_Directories( "temp" );
    Filestream::Write_Bin( nullptr, NULL, "temp\\" + name );
}

bool TextField::AlreadyOpened( const std::string& filepath, bool focus )
{
    for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
        if ( filepath == mPageData[i].FilePath )
        {   
            LOG_CONSOLE( LEVEL_INFO, "File is already opened: " + filepath );
            if ( focus ) mNotebook->ChangeSelection( i );
            return true;
        }
    return false;
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

void TextField::OnDropFiles( wxDropFilesEvent& event )
{
    Util::Timer tm( "Drop Files", MS, false );

    wxBusyCursor busyCursor;
    wxWindowDisabler disabler;
    wxBusyInfo busyInfo( "Adding files, wait please..." );

    if ( event.GetNumberOfFiles() < 1 ) return;
    auto files = FilehandleGUI::GetDroppedFiles( event.GetFiles(), event.GetNumberOfFiles() );
    if ( files.size() == 0 ) return;

    uint64_t readSizes = 0;
    for ( int i = 0; i < files.size(); i++ )
    {
        if ( AlreadyOpened( std::string( files[i] ), true ) ) continue;

        auto filepath = std::string( files[i] );
        auto vRead = FilehandleGUI::OpenFileFormat( filepath );
        auto text = wxString( &vRead[0], vRead.size() );
        readSizes += vRead.size();

        mPageData.push_back( { false, false, filepath, nullptr } );
        AddNewTab( mPageData.back() );
        mPageData.back().TextField->SetEOLMode( FontEncoding::GetEOLMode( text ) );
        mPageData.back().TextField->SetText( text );
        UpdateSaveIndicator( true );
    }

    LOG_ALL_FORMAT( LEVEL_TRACE, "Drag n Drop Items: %d, Size: %d, Time: %f (ms)", files.size(), readSizes, tm.Toc() );
}

bool TextField::UpdateEOLString( wxStyledTextCtrl* stc, int mode )
{
    auto str = stc->GetValue();
    if ( FontEncoding::ConvertEOLString( str, mode ) )
    {
        stc->SetText( str );
        stc->SetEOLMode( mode );
        UpdateStatusEOL( stc );
        UpdateSaveIndicator( false );
        return true;
    }
    return false;
}

void TextField::UpdateStatusEOL( wxStyledTextCtrl* stc )
{
    mParent->SetStatusText( FontEncoding::EOLModeString( stc->GetEOLMode() ), 2 );
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
    // remove page data first before doing notebook deletion
    if ( !mPageData.empty() )
    {
        UpdateParentName();
        UpdateStatusEncoding( mPageData[currentPage].TextField );
        UpdateStatusEOL( mPageData[currentPage].TextField );
        UpdateStatusPos( mPageData[currentPage].TextField );
    }
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
    Util::Timer tm( "Text Summary Dialog", MS, false );

    auto currentPage = mNotebook->GetSelection();
    auto pd = mPageData[currentPage];

    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( currentPage );
    message += "\nTemporary: ";
    message += pd.isTemporary ? "Yes" : "No";
    message += "\nEOL Mode: ";
    message += FontEncoding::EOLModeString( pd.TextField->GetEOLMode() );
    message += "\nCharacters: ";
    message += TO_STR( pd.TextField->GetTextLength() );
    message += "\nLines: ";
    message += TO_STR( pd.TextField->GetLineCount() );
    message += "\nLast Created: ";
    message += Filestream::GetLastCreated( pd.FilePath );
    message += "\nLast Modified: ";
    message += Filestream::GetLastModified( pd.FilePath );
    message += "\nFilepath: ";
    message += pd.FilePath;

    auto SumDialog = wxMessageDialog( mParent, message, "Text Summary", wxOK | wxSTAY_ON_TOP );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
    SumDialog.ShowModal();
}

void TextField::OnCompSummary( wxCommandEvent& event )
{
    Util::Timer tm( "Compression Summary Dialog", MS, false );

    auto currentPage = mNotebook->GetSelection();
    auto pd = mPageData[currentPage];

    bool isCompressedFile = false;
    if ( Filestream::FileExtension( mPageData[currentPage].FilePath ) == "mtx" )
        isCompressedFile = true;
    
    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( currentPage );
    message += "\nCompressed: ";
    message += isCompressedFile ? "Yes" : "No";
    if ( isCompressedFile )
    {
        auto info = Filestream::GetCompressedInfo( pd.FilePath );
        message += "\nClusters: ";
        message += TO_STR( info.ClusterSize );
        message += "\nAddressing Size: ";
        message += TO_STR( info.AddressingSize );
        message += "\nOriginal Size: ";
        message += TO_STR( pd.TextField->GetTextLength() );
        message += "\nCompressed Size: ";
        message += TO_STR( info.CompressedSize );
        message += "\nRatio: ";
        message += TO_STR( (double) info.CompressedSize / pd.TextField->GetTextLength() );
    }

    auto SumDialog = wxMessageDialog( mParent, message, "Compression Summary", wxOK | wxSTAY_ON_TOP );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
    SumDialog.ShowModal();
}

void TextField::OnEOL_CR( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_CR );
}

void TextField::OnEOL_LF( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_LF );
}

void TextField::OnEOL_CRLF( wxCommandEvent& event )
{
    auto currentPage = mNotebook->GetSelection();
    UpdateEOLString( mPageData[currentPage].TextField, wxSTC_EOL_CRLF );
}

void TextField::OnUpperCase( wxCommandEvent& event )
{
    Util::Timer tm( "Upper Case", MS, false );
    auto currentPage = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[currentPage].TextField, CASE_UPPER );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
}

void TextField::OnLowerCase( wxCommandEvent& event )
{
    Util::Timer tm( "Lower Case", MS, false );
    auto currentPage = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[currentPage].TextField, CASE_LOWER );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
}

void TextField::OnInverseCase( wxCommandEvent& event )
{
    Util::Timer tm( "Inverse Case", MS, false );
    auto currentPage = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[currentPage].TextField, CASE_INVERSE );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
}

void TextField::OnRandomCase( wxCommandEvent& event )
{
    Util::Timer tm( "Random Case", MS, false );
    auto currentPage = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[currentPage].TextField, CASE_RANDOM );
    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
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
        LOG_ALL( LEVEL_TRACE, "Rename file to: " + newName );
    }
}

void TextField::OnOpenFile( wxCommandEvent& event )
{
    std::string filepath = FilehandleGUI::OpenDialog( mParent, "" );
    if ( filepath.empty() ) return;
    
    try
    {
        Util::Timer tm( "Open File", ADJUST, false );
        
        if ( AlreadyOpened( filepath, true ) ) return;
        

        auto vRead = FilehandleGUI::OpenFileFormat( filepath );
        THROW_ERR_IFEMPTY( vRead, "Cannot open file " + filepath );
        LOG_ALL( LEVEL_TRACE, "Opened Filepath: " + filepath );
        
        wxString text = wxString::FromUTF8( (char*) &vRead[0], vRead.size() );
        THROW_ERR_IFEMPTY( text, "Problem converting string to wxString OnOpenFile()!" );

        IF_FIELD_UNOCCUPIED
        {
            mPageData[0].isChanged = false;
            mPageData[0].isTemporary = false;
            mPageData[0].FilePath = filepath;
            mNotebook->SetPageText( 0, Filestream::GetFileName( filepath ) );
        }
        else
        {
            mPageData.push_back( { false, false, filepath, nullptr } );           
            AddNewTab( mPageData.back() );
        }

        auto stc = mPageData.back().TextField;
        stc->SetEOLMode( FontEncoding::GetEOLMode( text ) );
        stc->SetText( text );

        // update indicator
        UpdateStatusEncoding( stc );
        UpdateStatusEOL( stc );
        UpdateStatusPos( stc );
        UpdateParentName();
        UpdateSaveIndicator( true );

        LOG_ALL_FORMAT( LEVEL_TRACE, "Document size: %llu (chars)", vRead.size() );
        LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LEVEL_ERROR, e.Seek() );
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
             CreateTempFile( "new1.tmp" );
             mNotebook->SetPageText( 0, "new1.tmp" );
             mPageData[0].TextField->ClearAll();
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
        LOG_ALL( LEVEL_ERROR, e.Seek() );
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
            mPageData.clear();
            mNotebook->DeleteAllPages();
            mPageData.push_back( { false, true, "temp\\new1.tmp", nullptr } );
            AddNewTab( mPageData.back() );
            CreateTempFile( "new1.tmp" );
            UpdateParentName();
            UpdateSaveIndicator( false );
        }
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LEVEL_ERROR, e.Seek() );
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

        FilehandleGUI::SaveFileFormat( mPageData[currentPage].FilePath, pData.c_str(), pData.size() );
        UpdateSaveIndicator( true );
        LOG_ALL( LEVEL_INFO, "File saved: " + mPageData[currentPage].FilePath );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LEVEL_ERROR, e.Seek() );
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
    auto currentPage = mNotebook->GetSelection();
    if( currentPage == wxNOT_FOUND ) return;

    std::string filepath = std::string( FilehandleGUI::SaveDialog( mParent, mNotebook->GetPageText( currentPage ) ) );
    if ( filepath.empty() ) return;

    try
    {       
        Util::Timer tm( "Save File", ADJUST, false );

        //if file is a temporary delete temp
        if ( mPageData[currentPage].isTemporary )
            Filestream::Delete_File( mPageData[currentPage].FilePath );

        mPageData[currentPage].FilePath = filepath;
        mPageData[currentPage].isTemporary = false;
        
        std::string pData = std::string( mPageData[currentPage].TextField->GetText() );
        FilehandleGUI::SaveFileFormat( filepath, pData.c_str(), pData.size() );
        THROW_ERR_IFNOT( Filestream::Is_Exist( filepath ), "Problem saving a file, please contact developer!" );

        mNotebook->SetPageText( currentPage, Filestream::GetFileName(filepath) );
        UpdateSaveIndicator( true );
        UpdateParentName();

        LOG_ALL_FORMAT( LEVEL_TRACE, "Document size: %llu (chars)", pData.size() );
        LOG_ALL( LEVEL_INFO, "File saved as: " + filepath );
        LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LEVEL_ERROR, e.Seek() );
    }
}

void TextField::OnEmbedDict( wxCommandEvent& event )
{
    Util::Timer tm( "Dictionary Frame", MS, false );

    if ( !isDictInit )
    {
        DictionaryFrame::Init( mParent );
        isDictInit = true;
    }

    wxArrayString list;
    list.reserve( mPageData.size() );
    for ( const auto& i : mPageData ) list.push_back( i.FilePath );

    auto currentPage = mNotebook->GetSelection();
    DictionaryFrame::UpdateComboBox( list, currentPage );
    DictionaryFrame::ShowAndFocus();

    LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
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
    pd.TextField = new wxStyledTextCtrl( mNotebook, -1, wxDefaultPosition, wxSize( 0, 0 ) );
    mNotebook->AddPage( pd.TextField, Filestream::GetFileName( pd.FilePath ), true );

    pd.TextField->SetZoom( Config::mZoomDefault );
    pd.TextField->SetScrollWidth( 1 ); //avoid large horizontal scroll width by default

    LoadStyle( pd.TextField );
    UpdateStatusEOL( pd.TextField );
    UpdateStatusPos( pd.TextField );
    UpdateStatusEncoding( pd.TextField );

    pd.TextField->DragAcceptFiles( true );
    pd.TextField->Bind( wxEVT_DROP_FILES, TextField::OnDropFiles );
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

    stc->SetMarginWidth( 0, 27 );
    stc->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    stc->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( 50, 50, 60 ) ); //linenumber back color
    stc->StyleSetForeground( wxSTC_STYLE_LINENUMBER, wxColour( 221, 151, 204 ) ); //linenumber fore color
    //stc->StyleSetSpec( wxSTC_STYLE_LINENUMBER, "fore:#dd96cc,back:#3c3c3c" ); //linenumber color with one func
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

void TextField::UpdateSaveIndicator( bool save, int index )
{
    auto currentPage = index;
    if ( index == -1 ) currentPage = mNotebook->GetSelection();

    if ( save )
    {
        mPageData[currentPage].isChanged = false;
        mNotebook->SetPageBitmap( currentPage, IMG_SAVE );
    }
    else if ( !save )
    {
        mPageData[currentPage].isChanged = true;
        mNotebook->SetPageBitmap( currentPage, IMG_UNSAVE );
    }
}