#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Timer.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )

EVT_MENU( wxID_EXIT, AppFrame::OnExit )
EVT_MENU( wxID_ABOUT, AppFrame::OnAbout )
EVT_MENU( ID_DOCUMENTATION, AppFrame::OnDocumentation )
EVT_MENU( ID_DEBUGCONSOLE, AppFrame::OnDebugConsole )
EVT_MENU( ID_LOGDIR, AppFrame::OnLogDir )
EVT_MENU( ID_REPORTBUG, AppFrame::OnReportBug )
EVT_MENU( ID_TABCLOSE, AppFrame::OnTabClose )
EVT_MENU( ID_TABCLOSEALL, AppFrame::OnTabCloseAll )

EVT_MENU( ID_OPENFILE, AppFrame::OnOpenFile )
EVT_MENU( ID_SAVEFILE, AppFrame::OnSaveFile )
EVT_MENU( ID_SAVEFILEAS, AppFrame::OnSaveFileAs )
EVT_MENU( ID_SAVEFILEALL, AppFrame::OnSaveFileAll )
EVT_MENU( ID_RENAMEFILE, AppFrame::OnRenameFile )

wxEND_EVENT_TABLE()

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size )
    : wxFrame( NULL, 1, title, pos, size )
{
    this->mTab = new wxNotebook( this, wxID_ANY );
    this->fh.InitFilehandle( this, this->mTab, &this->mTextField );

    #ifdef _DEBUG
    CreateDebugFrame();
    LogGUI::SetDebugTextField( this->mDebugTextField ); //LogFile static file for logging
    #endif

    CreateMenu();
    FetchTempFile();
    this->mStatusBar = CreateStatusBar();
    this->mStatusBar->SetStatusText( "Welcome to Memoriser!" );
}

void AppFrame::AddNewTab( const std::string& name )
{
    this->mTextField.push_back( new wxStyledTextCtrl( this->mTab, wxID_ANY ) );
    mTab->AddPage( mTextField.back(), name );
    mTab->ChangeSelection( mTextField.size() - 1 );
}

void AppFrame::FetchTempFile()
{
    mTempAmount = 1;
    for ( uint32_t i = 0; i < mTempAmount; i++ )
    {
        AddNewTab( "new" );
    }
}

void AppFrame::CreateDebugFrame()
{
    this->mDebugFrame = new wxFrame( this, wxID_ANY, "Memoriser - Debug Console", wxDefaultPosition, wxSize( 600, 300 ) );
    this->mDebugFrame->Bind( wxEVT_CLOSE_WINDOW, &AppFrame::OnDebugExit, this ); //handle close window event (dont exit, just hide)
    this->mDebugTextField = new wxStyledTextCtrl( this->mDebugFrame, wxID_ANY, wxDefaultPosition );
    this->mDebugTextField->SetEditable( false );
    
    this->mDebugFrame->Show( true );
    this->mDebugTextField->Show( true );
}

void AppFrame::CreateMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append( ID_NEWFILE, "&New\tCtrl-N" );
    menuFile->Append( ID_SAVEFILE, "&Save\tCtrl-S" );
    menuFile->Append( ID_SAVEFILEAS, "&Save As\tCtrl-Alt-S" );
    menuFile->Append( ID_SAVEFILEALL, "&Save All\tCtrl-Shift-S" );
    menuFile->Append( ID_OPENFILE, "&Open\tCtrl-O" );
    menuFile->Append( ID_RENAMEFILE, "&Rename" );
    menuFile->Append( ID_TABCLOSE, "&Close\tCtrl-W" );
    menuFile->Append( ID_TABCLOSEALL, "&Close All\tCtrl-Shift-W" );
    menuFile->AppendSeparator();
    menuFile->Append( wxID_ANY, "&New Dictionary" );
    menuFile->Append( wxID_ANY, "&Open Dictionary" );
    menuFile->AppendSeparator();
    menuFile->Append( wxID_EXIT );

    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append( wxID_ANY, "Undo\tCtrl-Z" );
    menuEdit->Append( wxID_ANY, "Redo\tCtrl-Y" );
    menuEdit->AppendSeparator();
    menuEdit->Append( wxID_ANY, "Cut\tCtrl-Y" );
    menuEdit->Append( wxID_ANY, "Copy\tCtrl-C" );
    menuEdit->Append( wxID_ANY, "Paste\tCtrl-V" );
    menuEdit->Append( wxID_ANY, "Delete\tDel" );
    menuEdit->Append( wxID_ANY, "Select All\tCtrl-A" );

    wxMenu* menuSearch = new wxMenu;
    menuSearch->Append( wxID_ANY, "Find\t\tCtrl-F" );
    menuSearch->Append( wxID_ANY, "Select and Find Next\t\tCtrl-F3" );
    menuSearch->Append( wxID_ANY, "Select and Find Previous\t\tCtrl-Shift-F3" );
    menuSearch->Append( wxID_ANY, "Replace\t\tCtrl-H" );
    menuSearch->Append( wxID_ANY, "Goto\t\tCtrl-G" );

    wxMenu* menuView = new wxMenu;
    menuView->Append( wxID_ANY, "Always on Top" );
    menuView->Append( wxID_ANY, "Zoom In\tCtrl+Num +" );
    menuView->Append( wxID_ANY, "Zoom Out\tCtrl+Num -" );
    menuView->Append( wxID_ANY, "Restore Zoom\tCtrl+Num /" );
    menuView->AppendSeparator();
    menuView->Append( wxID_ANY, "Text Summary" );
    menuView->Append( wxID_ANY, "Compression Summary" );
    #ifdef _DEBUG
    menuView->AppendSeparator();
    menuView->Append( ID_DEBUGCONSOLE, "Debug Console" );
    #endif

    wxMenu* menuSettings = new wxMenu;
    menuSettings->Append( wxID_ANY, "Preferences" );
    menuSettings->Append( wxID_ANY, "Style Configuration" );

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append( ID_REPORTBUG, "Report Bug" );
    menuHelp->Append( ID_LOGDIR, "Open Log Directory" );
    menuHelp->AppendSeparator();
    menuHelp->Append( ID_DOCUMENTATION, "See Documentation" );
    menuHelp->Append( wxID_ABOUT );

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuEdit, "&Edit" );
    menuBar->Append( menuSearch, "&Search" );
    menuBar->Append( menuView, "&View" );
    menuBar->Append( menuSettings, "&Settings" );
    menuBar->Append( menuHelp, "&Help" );

    SetMenuBar( menuBar );
}

void AppFrame::OnExit( wxCommandEvent& event )
{
    Close( true );
}

void AppFrame::OnAbout( wxCommandEvent& event )
{
    wxLogMessage( "Memoriser Ver 0.1" );
}

void AppFrame::OnDocumentation( wxCommandEvent& event )
{
#if defined( _WIN32 )       
    system( "start https://github.com/ramdanks/TextEditor" );
#elif defined( unix )       
    system( "wget https://github.com/ramdanks/TextEditor" );
#elif defined( __APPLE__ )  
    system( "open https://github.com/ramdanks/TextEditor" );
#elif defined ( __linux__ ) 
    system( "xdg-open https://github.com/ramdanks/TextEditor" );
#endif
}

void AppFrame::OnLogDir( wxCommandEvent& event )
{
    #if defined( _WIN32 )       
    system( "explorer Log" );
    #elif defined( __APPLE__ )  
    system( "open Log" );
    #elif defined ( __linux__ ) 
    system( "nautilus Log" );
    #endif
}

void AppFrame::OnReportBug( wxCommandEvent& event )
{
}

void AppFrame::OnTabClose( wxCommandEvent& event )
{
    auto currentPage = this->mTab->GetSelection();
    if ( !this->mTextField[currentPage]->IsEmpty() )
    {
        auto prompt = new wxMessageDialog( this, "Are you sure want to close this file? Any changes will be ignored, please save your work!", "Close Window", wxYES_NO );
        if ( prompt->ShowModal() == wxID_YES )
        {
            this->mTab->DeletePage( currentPage );
            this->mTextField.erase( mTextField.begin() + currentPage );
            if ( this->mTextField.size() < 1 ) AddNewTab( "new" ); 
        }
        prompt->Destroy();
    } 
}

void AppFrame::OnTabCloseAll( wxCommandEvent& event )
{
    if ( this->mTextField.size() == 1 && this->mTextField[0]->IsEmpty() ) return;

    auto prompt = new wxMessageDialog( this, "Are you sure want to close all file? Any changes will be ignored, please save your work!", "Close All Window", wxYES_NO );
    if ( prompt->ShowModal() == wxID_YES )
    {
        this->mTab->DeleteAllPages();
        this->mTextField.clear();
        if ( this->mTextField.size() < 1 ) AddNewTab( "new" );
    }
    prompt->Destroy();
}

void AppFrame::OnNewFile( wxCommandEvent& event )
{
    if ( !this->mTextField.back()->IsEmpty() ) AddNewTab( "new" );
}

void AppFrame::OnOpenFile( wxCommandEvent& event )
{ fh.OnOpenFile(); }

void AppFrame::OnSaveFile( wxCommandEvent& event )
{ fh.OnSaveFile(); }

void AppFrame::OnSaveFileAs( wxCommandEvent& event )
{ fh.OnSaveFileAs(); }

void AppFrame::OnSaveFileAll( wxCommandEvent& event )
{ fh.OnSaveFileAll(); }

void AppFrame::OnRenameFile( wxCommandEvent& event )
{ fh.OnRenameFile(); }

void AppFrame::OnDebugConsole( wxCommandEvent& event )
{
    this->mDebugFrame->Show( true );
    this->mDebugFrame->Raise();
}

void AppFrame::OnDebugExit( wxCloseEvent& event )
{
    this->mDebugFrame->Show( false ); //hide instead of closing
}