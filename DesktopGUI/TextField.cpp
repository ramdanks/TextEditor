#include "TextField.h"
#include "Feature/FilehandleGUI.h"
#include "Feature/LogGUI.h"
#include "Feature/Config.h"
#include "Feature/Image.h"
#include "Feature/Language.h"
#include "Frame/DictionaryFrame.h"
#include "Frame/GotoFrame.h"
#include "Frame/FindFrame.h"
#include "Frame/EventID.h"
#include "FontEncoding.h"

//translation unit for static member
wxFrame* TextField::mParent;
wxMenu* TextField::mMenuWnd;
wxAuiNotebook* TextField::mNotebook;
std::vector<sPageData> TextField::mPageData;
std::vector<wxMenuItem*> TextField::mWindowItem;
bool TextField::isGotoInit = false;
bool TextField::isFindInit = false;
bool TextField::isDictInit = false;
std::mutex TextField::mMutex;
std::future<void> TextField::mFuture;

wxCommandEvent NullCmdEvent = wxCommandEvent( wxEVT_NULL );
//contain only one tab or textfield, already temporary, and no content
#define IF_FIELD_UNOCCUPIED if ( mNotebook->GetPageCount() == 1 && mPageData[0].isTemporary && mPageData[0].TextField->IsEmpty() )

void TextField::Init( wxFrame * parent )
{
    mParent = parent;
    int style = Config::GetNotebookStyle();
    mNotebook = new wxAuiNotebook( parent, -1, wxDefaultPosition, wxDefaultSize, style );

    const int statusWidth[] = { -3, -1, 120, -1 };
    mParent->GetStatusBar()->SetFieldsCount( 4 );
    mParent->GetStatusBar()->SetStatusWidths( 4, statusWidth );

    mNotebook->Bind( wxEVT_AUINOTEBOOK_END_DRAG, TextField::OnPageDrag );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CLOSE, TextField::OnPageCloseButton );
    mNotebook->Bind( wxEVT_AUINOTEBOOK_PAGE_CHANGED, TextField::OnPageChanged );
}

void TextField::Destroy()
{
    mNotebook = nullptr;
}

void TextField::MarginAutoAdjust()
{
    PROFILE_FUNC();
    if ( mNotebook == nullptr ) return;

    auto sel = mNotebook->GetSelection();
    auto tf = mPageData[sel].TextField;
    wxString lines = wxString::Format( wxT( "%i" ), tf->GetLineCount() );
    auto width = tf->TextWidth( wxSTC_STYLE_LINENUMBER, lines + ' ' );
    tf->SetMarginWidth( 0, width );
}

void TextField::FetchTempFile()
{
    uint32_t amount = 0;
    if ( Config::mTemp.UseTemp )
    {
        auto vTempFile = Filestream::File_List( Config::mTemp.Directory );
        amount = vTempFile.size();
        if ( amount == 0 ) OnNewFile( NullCmdEvent );
        else
        {
            mPageData.reserve( vTempFile.size() );
            for ( uint32_t i = 0; i < vTempFile.size(); i++ )
            {
                auto filename = Filestream::GetFileName( vTempFile[i] );
                mPageData.emplace_back( false, true, filename, vTempFile[i] );
                AddNewTab( mPageData.back() );
                auto vRead = Filestream::Read_Bin( vTempFile[i] );
                if ( !vRead.empty() ) 
                    mPageData.back().TextField->AppendText( wxString( &vRead[0], vRead.size() ) );
                UpdateSaveIndicator( false );
            }
        }
    }
    else
    {
        mPageData.emplace_back( false, true, "new", "" );
        AddNewTab( mPageData.back() );
        UpdateSaveIndicator( false );
    }    
    LOG_ALL_FORMAT( LV_TRACE, "Temporary file amount: %u", amount );
}

void TextField::CreateTempFile( const std::string& path )
{
    Filestream::Write_Bin( nullptr, NULL, path );
}

bool TextField::AlreadyOpened( const std::string& filepath, bool focus )
{
    for ( uint32_t i = 0; i < mNotebook->GetPageCount(); i++ )
        if ( filepath == mPageData[i].FilePath )
        {   
            LOG_DEBUG( LV_INFO, "File is already opened: " + filepath );
            if ( focus ) mNotebook->ChangeSelection( i );
            return true;
        }
    return false;
}

void TextField::SaveTempAll()
{
    std::lock_guard<std::mutex> lock( mMutex );
    for ( uint32_t i = 0; i < mPageData.size(); i++ )
        if ( mPageData[i].isTemporary && !mPageData[i].FilePath.empty() )
        {
            std::string temp = std::string( mPageData[i].TextField->GetText().mb_str( wxConvUTF8 ) );
            Filestream::Write_Bin( temp.c_str(), temp.size(), mPageData[i].FilePath );
        }
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
    for ( size_t i = 0; i < mPageData.size(); i++  )
    {
        if ( !mPageData[i].isTemporary && mPageData[i].isChanged
             || ( mPageData[i].isTemporary && !Config::mTemp.UseTemp ) )
        {
            mNotebook->SetSelection( i );
            return false;
        }
    }
    return true;
}

int TextField::GetActivePage()
{
    if ( mNotebook == nullptr ) return -1;
    return mNotebook->GetSelection();
}

bool TextField::LoadFile( const std::string& filepath )
{
    if ( !Filestream::Exist( filepath ) ) return false;

    auto vRead = FilehandleGUI::OpenFileFormat( filepath );
    uint32_t readSizes = vRead.size();
    auto text = wxString( &vRead[0], readSizes );
    auto filename = Filestream::GetFileName( filepath );
 
    IF_FIELD_UNOCCUPIED
    {
        Filestream::Delete_File( mPageData[0].FilePath );
        mPageData[0].isChanged = false;
        mPageData[0].isTemporary = false;
        mPageData[0].FileName = filename;
        mPageData[0].FilePath = filepath;
        mPageData[0].Suggestion.clear();
    }
    else
    {
        mPageData.emplace_back( false, false, filename, filepath );
    }
    AddNewTab( mPageData.back() );

    auto stc = mPageData.back().TextField;
    stc->SetEOLMode( FontEncoding::GetEOLMode( text ) );
    stc->SetText( text );

    // update indicator
    UpdateStatusEncoding( stc );
    UpdateStatusEOL( stc );
    UpdateStatusPos( stc );
    UpdateParentName();
    UpdateSaveIndicator( true );
    UpdateMenuWindow();

    LOG_ALL_FORMAT( LV_TRACE, "File opened, size: %u (bytes)", readSizes );
    return true;
}

void TextField::OnDropFiles( wxDropFilesEvent& event )
{
    TIMER_ONLY( tm, MS, false );

    wxBusyCursor busyCursor;
    wxWindowDisabler disabler;
    wxBusyInfo busyInfo( "Adding files, wait please..." );

    if ( event.GetNumberOfFiles() < 1 ) return;
    auto files = FilehandleGUI::GetDroppedFiles( event.GetFiles(), event.GetNumberOfFiles() );
    if ( files.size() == 0 ) return;

    for ( int i = 0; i < files.size(); i++ )
    {
        auto filepath = std::string( files[i] );
        if ( !AlreadyOpened( filepath, true ) ) LoadFile( filepath );
    }

    LOG_ALL_FORMAT( LV_TRACE, "Drag n Drop Items: %d, Time: %f (ms)", files.size(), tm.toc() );
}

void TextField::UpdateMenuWindow()
{
    PROFILE_FUNC();

    auto menubar = mParent->GetMenuBar();
    int pos = menubar->FindMenu( MSG_WINDOW );
    if ( pos != wxNOT_FOUND ) menubar->Remove( pos );

    auto count = mNotebook->GetPageCount();
    mWindowItem.clear();
    mWindowItem.reserve( count );

    mMenuWnd = new wxMenu;
    for ( uint32_t i = 0; i < count; i++ )
    {
        mWindowItem.emplace_back( mMenuWnd->AppendCheckItem( ID_WINDOWSELECT + i, mNotebook->GetPageText( i ) ) );
        mParent->Bind( wxEVT_COMMAND_MENU_SELECTED, OnSelectMenuWindow, ID_WINDOWSELECT + i );
    }
    menubar->Append( mMenuWnd, MSG_WINDOW );
    mMenuWnd->Bind( wxEVT_MENU_OPEN, OnOpenMenuWindow );
}

void TextField::OnSelectMenuWindow( wxCommandEvent& event )
{
    PROFILE_FUNC();
    auto sel = event.GetId() - ID_WINDOWSELECT;
    mNotebook->ChangeSelection( sel );
    UpdateParentName();
}

void TextField::OnOpenMenuWindow( wxMenuEvent& event )
{
    PROFILE_FUNC();
    for ( const auto& menu : mWindowItem ) menu->Check( false );
    mWindowItem[mNotebook->GetSelection()]->Check();
}

void TextField::OnOpenMenuSearch( wxMenuEvent& event )
{
    PROFILE_FUNC();

    auto page = GetActivePage();
    auto stc = mPageData[page].TextField;

    auto menubar = mParent->GetMenuBar();
    auto pos = menubar->FindMenu( MSG_SEARCH );
    auto menuFind = menubar->GetMenu( pos );

    auto start = stc->GetSelectionStart();
    auto end = stc->GetSelectionEnd();

    if ( start != end )
    {
        menuFind->Enable( ID_SELECTFNEXT, true );
        menuFind->Enable( ID_SELECTFPREV, true );
    }
    else
    {
        menuFind->Enable( ID_SELECTFNEXT, false );
        menuFind->Enable( ID_SELECTFPREV, false );
    }
}

void TextField::ShowAutoComp()
{
    // Find the word start
    auto page = GetActivePage();
    auto stc = mPageData[page].TextField;

    int currentPos = stc->GetCurrentPos();
    int wordStartPos = stc->WordStartPosition( currentPos, true );
    int lenEntered = currentPos - wordStartPos;
    if ( !stc->AutoCompActive() && lenEntered > 0 )
        stc->AutoCompShow( lenEntered, mPageData[page].Suggestion );
}

void TextField::OnAutoCompCompleted( wxStyledTextEvent& event )
{
    auto stc = (wxStyledTextCtrl*) event.GetEventObject();
    stc->AutoCompCancel();
}

bool TextField::UpdateEOLString( wxStyledTextCtrl* stc, int mode )
{
    PROFILE_FUNC();
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
    PROFILE_FUNC();
    mParent->SetStatusText( FontEncoding::EOLModeString( stc->GetEOLMode() ), 2 );
}

void TextField::UpdateStatusEncoding( wxStyledTextCtrl* stc )
{
    PROFILE_FUNC();
    wxString status;
    status = FontEncoding::GetEncodingString( stc->GetFont().GetEncoding() );
    mParent->SetStatusText( status, 3 );
}

void TextField::UpdateStatusPos( wxStyledTextCtrl* stc )
{
    PROFILE_FUNC();
    wxString status;
    auto line = stc->GetCurrentLine();
    auto pos = stc->GetCurrentPos();
    status = wxString::Format( "Line: %d, Pos: %d", line+1, pos );
    mParent->SetStatusText( status, 1 );
}

void TextField::FindText( wxStyledTextCtrl* stc, bool next )
{
    PROFILE_FUNC();

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
    PROFILE_FUNC();

    auto stc = (wxStyledTextCtrl*) event.GetEventObject();
    static int beforePos = 0;
    int curPos = stc->GetCurrentPos();
    if ( beforePos != curPos )
    {
        beforePos = curPos;
        UpdateStatusPos( stc );
    }
}

void TextField::OnTextChanged( wxStyledTextEvent& event )
{
    PROFILE_FUNC();
    if ( Config::mAutocomp.Use ) ShowAutoComp();
    UpdateSaveIndicator( false );
    MarginAutoAdjust();
}

void TextField::OnSTCZoom( wxStyledTextEvent& event )
{
    MarginAutoAdjust();
}

void TextField::OnPageCloseButton( wxAuiNotebookEvent& evt )
{
    evt.Veto();
    OnPageClose( NullCmdEvent );
}

void TextField::OnPageChanged( wxAuiNotebookEvent& evt )
{
    PROFILE_FUNC();
    auto sel = mNotebook->GetSelection();
    // remove page data first before doing notebook deletion
    if ( !mPageData.empty() )
    {
        UpdateParentName();
        UpdateStatusEncoding( mPageData[sel].TextField );
        UpdateStatusEOL( mPageData[sel].TextField );
        UpdateStatusPos( mPageData[sel].TextField );
    }
}

void TextField::OnPageDrag( wxAuiNotebookEvent& evt )
{
    PROFILE_FUNC();

    auto sel = evt.GetSelection();
    auto currentWindow = mNotebook->GetPage( sel );

    if ( currentWindow != mPageData[sel].TextField )
    {
        //find the origin index of page that being dragged
        uint32_t i = 0;
        for ( i = 0; i < mPageData.size(); i++ )
            if ( currentWindow == mPageData[i].TextField ) break;

        //original page data
        sPageData temp = mPageData[i];
        mPageData.erase( mPageData.begin() + i );
        mPageData.insert( mPageData.begin() + sel, temp );
        UpdateMenuWindow();
    }
    mNotebook->SetSelection( sel );
}

void TextField::OnTextSummary( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    auto pd = mPageData[sel];

    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( sel );
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
    SumDialog.ShowModal();
}

void TextField::OnCompSummary( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    auto pd = mPageData[sel];

    bool isCompressedFile = false;
    if ( Filestream::FileExtension( mPageData[sel].FilePath ) == "mtx" )
        isCompressedFile = true;
    
    wxString message;
    message += "Page Name: ";
    message += mNotebook->GetPageText( sel );
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
    SumDialog.ShowModal();
}

void TextField::OnEOL_CR( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    UpdateEOLString( mPageData[sel].TextField, wxSTC_EOL_CR );
}

void TextField::OnEOL_LF( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    UpdateEOLString( mPageData[sel].TextField, wxSTC_EOL_LF );
}

void TextField::OnEOL_CRLF( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    UpdateEOLString( mPageData[sel].TextField, wxSTC_EOL_CRLF );
}

void TextField::OnUpperCase( wxCommandEvent& event )
{
    PROFILE_FUNC();
    auto sel = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[sel].TextField, CASE_UPPER );
}

void TextField::OnLowerCase( wxCommandEvent& event )
{
    PROFILE_FUNC();
    auto sel = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[sel].TextField, CASE_LOWER );
}

void TextField::OnInverseCase( wxCommandEvent& event )
{
    PROFILE_FUNC();
    auto sel = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[sel].TextField, CASE_INVERSE );
}

void TextField::OnRandomCase( wxCommandEvent& event )
{
    PROFILE_FUNC();
    auto sel = mNotebook->GetSelection();
    FontEncoding::CaseConversion( mPageData[sel].TextField, CASE_RANDOM );
}

void TextField::OnNewFile( wxCommandEvent& event )
{
    PROFILE_FUNC();

    if ( Config::mTemp.UseTemp )
    {
        char filepath[1024];
        char filename[32];
        for ( uint32_t counter = 1; ; counter++ )
        {
            snprintf( filename, sizeof filename, "new%d.tmp", counter );
            snprintf( filepath, sizeof filepath, "%s\\%s", Config::mTemp.Directory.c_str(), filename );
            if ( !Filestream::Exist( filepath ) ) break;
        }
        CreateTempFile( filepath );
        mPageData.emplace_back( false, true, std::move(filename), std::move(filepath) );
    }
    else
    {
        mPageData.emplace_back( false, true, "new", "" );
    }
    AddNewTab( mPageData.back() );
    UpdateSaveIndicator( false );
    UpdateParentName();
}

void TextField::OnRenameFile( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();

    //if its a temporary file redirect to save as
    if ( mPageData[sel].isTemporary ) 
    { 
        OnSaveFileAs( NullCmdEvent ); 
        return; 
    }

    wxTextEntryDialog RenameDlg( mParent, "Insert new name:", "Rename File", mNotebook->GetPageText( sel ) );
    if ( RenameDlg.ShowModal() == wxID_OK )
    {
        uint32_t len = mNotebook->GetPageText( sel ).Len();
        uint32_t pathSize = mPageData[sel].FilePath.size() - mPageData[sel].FileName.size();

        std::string newName = std::string( RenameDlg.GetValue().mb_str() );
        std::string newPath = mPageData[sel].FilePath.substr( 0, pathSize ) + newName;

        if ( Filestream::Exist( newPath ) )
        {
            auto prompt = wxMessageDialog( mParent, "File with that name already exist!\n"
                                           "Are you sure want to overwrite?", "Overwrite", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }
        Filestream::Rename_File( newPath, mPageData[sel].FilePath );
        DictionaryFrame::ChangeListKey( mPageData[sel].FilePath, newPath );

        mPageData[sel].FileName = newName;
        mPageData[sel].FilePath = newPath;

        mNotebook->SetPageText( sel, RenameDlg.GetValue() );
        UpdateParentName();

        LOG_ALL( LV_TRACE, "Rename file to: " + newPath );
    }
}

void TextField::OnOpenFile( wxCommandEvent& event )
{
    std::string filepath = FilehandleGUI::OpenDialog( mParent, "" );
    if ( filepath.empty() ) return;
    if ( !AlreadyOpened( filepath, true ) ) LoadFile( filepath );
}

void TextField::OnPageClose( wxCommandEvent& evt )
{
    IF_FIELD_UNOCCUPIED return;
    try
    {
        auto sel = mNotebook->GetSelection();
        THROW_ERR_IF( sel == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        //destroy dictionary list after closing a pages
        DictionaryFrame::EraseList( mPageData[sel].FilePath );

        //if that page is changed then ask first
        if ( mPageData[sel].isChanged )
        {
            wxMessageDialog prompt( mParent, "Any changes to the file will be ignored.\n"
                                    "Sure want to Continue ?", "Close Page", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO ) return;
        }

         if ( mPageData[sel].isTemporary ) //delete temp file on close
             Filestream::Delete_File( mPageData[sel].FilePath );

         if ( mNotebook->GetPageCount() == 1 )
         {
             if ( Config::mTemp.UseTemp )
             {
                 std::string tempPath = Config::mTemp.Directory + "\\new1.tmp";
                 CreateTempFile( tempPath );
                 mPageData[0].FileName = "new1.tmp";
                 mPageData[0].FilePath = std::move( tempPath );
             }
             else
             {
                 mPageData[0].FileName = "new";
                 mPageData[0].FilePath.clear();
             }
             mNotebook->SetPageText( 0, mPageData[0].FileName );
             mPageData[0].TextField->ClearAll();
             mPageData[0].isTemporary = true;
         }
         else
         {
             mNotebook->DeletePage( sel );
             mPageData.erase( mPageData.begin() + sel );
         }
         UpdateMenuWindow();
         UpdateParentName();
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
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
            if ( Config::mTemp.UseTemp )
            {
                CreateTempFile( "new1.tmp" );
                auto filepath = Config::mTemp.Directory + "\\new1.tmp";
                mPageData.emplace_back( false, true, "new1.tmp", std::move(filepath) );
            }
            else mPageData.emplace_back( false, true, "new", "" );
            AddNewTab( mPageData.back() );
            UpdateParentName();
            UpdateSaveIndicator( false );
        }
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
    }
}

void TextField::OnSaveFile( wxCommandEvent& event )
{
    try
    {
        auto sel = mNotebook->GetSelection();
        THROW_ERR_IF( sel == wxNOT_FOUND, "Cannot found selected tab. problems during closing tab!" );

        if ( !mPageData[sel].isChanged ) return;
        if ( mPageData[sel].isTemporary )
        {
            OnSaveFileAs( NullCmdEvent );
            return;
        }
        // get string from current textfield
        std::string pData = std::string( mPageData[sel].TextField->GetText() );

        FilehandleGUI::SaveFileFormat( mPageData[sel].FilePath, pData.c_str(), pData.size() );
        UpdateSaveIndicator( true );
        LOG_ALL( LV_INFO, "File saved: " + mPageData[sel].FilePath );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
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
    auto sel = mNotebook->GetSelection();
    if( sel == wxNOT_FOUND ) return;

    std::string filepath = std::string( FilehandleGUI::SaveDialog( mParent, mNotebook->GetPageText( sel ) ) );
    if ( filepath.empty() ) return;

    try
    {       
        //if file is a temporary delete temp
        if ( mPageData[sel].isTemporary )
            Filestream::Delete_File( mPageData[sel].FilePath );
        
        std::string pData = std::string( mPageData[sel].TextField->GetText() );
        FilehandleGUI::SaveFileFormat( filepath, pData.c_str(), pData.size() );
        THROW_ERR_IFNOT( Filestream::Exist( filepath.c_str() ), "Problem saving to file!" );

        DictionaryFrame::ChangeListKey( mPageData[sel].FilePath, filepath );

        mPageData[sel].FileName = Filestream::GetFileName( filepath );
        mPageData[sel].FilePath = filepath;
        mPageData[sel].isTemporary = false;

        UpdateParentName();
        UpdateSaveIndicator( true );
        mNotebook->SetPageText( sel, mPageData[sel].FileName );

        LOG_ALL( LV_INFO, "File saved as: " + filepath );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
    }
}

void TextField::OnEmbedDict( wxCommandEvent& event )
{
    PROFILE_FUNC();

    if ( !isDictInit )
    {
        DictionaryFrame::Init( mParent );
        isDictInit = true;
    }

    wxArrayString list;
    list.reserve( mPageData.size() );
    for ( const auto& i : mPageData ) list.push_back( i.FilePath );

    auto sel = mNotebook->GetSelection();
    DictionaryFrame::UpdateComboBox( list, mPageData[sel].FilePath );
    DictionaryFrame::ShowAndFocus();
}

void TextField::OnUndo( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->Undo();
}

void TextField::OnRedo( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->Redo();
}

void TextField::OnCut( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->Cut();
}

void TextField::OnCopy( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->Copy();
}

void TextField::OnPaste( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->Paste();
}

void TextField::OnDelete( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->DeleteBack();
}

void TextField::OnSelectAll( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->SelectAll();
}

void TextField::OnZoomIn( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->ZoomIn();
}

void TextField::OnZoomOut( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->ZoomOut();
}

void TextField::OnZoomRestore( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    mPageData[sel].TextField->SetZoom( Config::mGeneral.ZoomDefault );
}

void TextField::OnFind( wxCommandEvent& event )
{
    if ( !isFindInit )
    {
        FindFrame::Init( mParent );
        isFindInit = true;
    }
    auto sel = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[sel].TextField, wxString::FromUTF8( mPageData[sel].FilePath ) );
    FindFrame::ShowAndFocus( true );
}

void TextField::OnReplace( wxCommandEvent& event )
{
    if ( !isFindInit )
    {
        FindFrame::Init( mParent );
        isFindInit = true;
    }
    auto sel = mNotebook->GetSelection();
    FindFrame::UpdateInfo( mPageData[sel].TextField, wxString::FromUTF8( mPageData[sel].FilePath ) );
    FindFrame::ShowAndFocus( false );
}

void TextField::OnGoto( wxCommandEvent& event )
{
    if ( !isGotoInit )
    {
        GotoFrame::Init( mParent );
        isGotoInit = true;
    }
    auto sel = mNotebook->GetSelection();
    GotoFrame::UpdateInfo( mPageData[sel].TextField );
    GotoFrame::ShowAndFocus();
}

void TextField::OnFindNext( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    FindText( mPageData[sel].TextField, true );
}

void TextField::OnFindPrev( wxCommandEvent& event )
{
    auto sel = mNotebook->GetSelection();
    FindText( mPageData[sel].TextField, false );
}

void TextField::AddNewTab( sPageData& pd )
{
    PROFILE_FUNC();

    if ( !pd.TextField )
    {
        pd.TextField = new wxStyledTextCtrl( mNotebook, ID_TEXTFIELDCTRL, wxDefaultPosition, wxSize( 0, 0 ) );
        mNotebook->AddPage( pd.TextField, pd.FileName, true );
    }
    else mNotebook->SetPageText( mNotebook->GetSelection(), pd.FileName );

    pd.TextField->SetZoom( Config::mGeneral.ZoomDefault );
    pd.TextField->SetScrollWidth( 1 ); //avoid large horizontal scroll width by default

    LoadStyle( pd.TextField );
    UpdateStatusEOL( pd.TextField );
    UpdateStatusPos( pd.TextField );
    UpdateStatusEncoding( pd.TextField );

    pd.TextField->DragAcceptFiles( true );
    pd.TextField->AutoCompSetAutoHide( true );
    pd.TextField->AutoCompSetIgnoreCase( true );
    pd.TextField->AutoCompSetMaxWidth( 32 );
    pd.TextField->AutoCompSetSeparator( '|' );
    pd.TextField->Bind( wxEVT_DROP_FILES, OnDropFiles );
    pd.TextField->Bind( wxEVT_STC_UPDATEUI, OnUpdateUI );
    pd.TextField->Bind( wxEVT_STC_CHANGE, OnTextChanged );
    pd.TextField->Bind( wxEVT_STC_ZOOM, OnSTCZoom );
    pd.TextField->Bind( wxEVT_STC_AUTOCOMP_COMPLETED, OnAutoCompCompleted );

    UpdateMenuWindow();
    MarginAutoAdjust();

    try
    {
        if ( Config::mDictionary.UseGlobal )
        {
            if ( Config::mDictionary.ApplyOn == DICT_ALL_DOCS
                 || ( pd.isTemporary && Config::mDictionary.ApplyOn == DICT_TMP_DOCS )
                 || ( !pd.isTemporary && Config::mDictionary.ApplyOn == DICT_OPN_DOCS ) )
            {
                DictionaryFrame::LoadFromDir( pd.FilePath, Config::mDictionary.Directory );
                DictionaryFrame::ResetStyling( pd.FilePath );
                DictionaryFrame::CreateSuggestion( pd.FilePath );
            }
        }
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
    }
}

void TextField::LoadStyle( wxStyledTextCtrl* stc )
{
    PROFILE_FUNC();
    stc->StyleSetFont( wxSTC_STYLE_DEFAULT, Config::BuildFont() );
    stc->StyleSetForeground( wxSTC_STYLE_DEFAULT, wxColour( Config::mStyle.TextFore ) ); //foreground such as text
    stc->StyleSetBackground( wxSTC_STYLE_DEFAULT, wxColour( Config::mStyle.TextBack ) ); //background for field
    stc->StyleClearAll();
    stc->SetCaretLineVisible( true ); //set caret line background visible
    stc->SetCaretLineBackground( wxColour( Config::mStyle.LineBack ) ); //caret line background
    stc->SetCaretForeground( wxColour( Config::mStyle.Caret ) ); //caret colour
    stc->SetSelBackground( true, wxColour( Config::mStyle.Selection ) ); //background of selected text
    stc->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    stc->StyleSetBackground( wxSTC_STYLE_LINENUMBER, wxColour( Config::mStyle.LinenumBack ) ); //linenumber back color
    stc->StyleSetForeground( wxSTC_STYLE_LINENUMBER, wxColour( Config::mStyle.LinenumFore ) ); //linenumber fore color
    //stc->StyleSetSpec( wxSTC_STYLE_LINENUMBER, "fore:#dd96cc,back:#3c3c3c" ); //linenumber color with one func
}

void TextField::UpdateParentName()
{
    PROFILE_FUNC();

    auto sel = mNotebook->GetSelection();
    wxString title = APP_NAME;
    if ( !mPageData[sel].isTemporary )
    {
        title += " - ";
        title += mPageData[sel].FilePath;
    }
    mParent->SetTitle( title );
}

void TextField::UpdateSaveIndicator( bool save, int index )
{
    PROFILE_FUNC();

    if ( mNotebook == nullptr ) return;
    auto sel = index;
    if ( index == -1 ) sel = mNotebook->GetSelection();

    if ( save )
    {
        mPageData[sel].isChanged = false;
        mNotebook->SetPageBitmap( sel, IMG_SAVE );
    }
    else if ( !save )
    {
        mPageData[sel].isChanged = true;
        mNotebook->SetPageBitmap( sel, IMG_UNSAVE );
    }
}