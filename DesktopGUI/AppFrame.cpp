#include "AppFrame.h"
#include "../Utilities/Timer.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )
EVT_MENU( wxID_EXIT, AppFrame::OnExit )
EVT_MENU( wxID_ABOUT, AppFrame::OnAbout )
EVT_MENU( ID_DOCUMENTATION, AppFrame::OnDocumentation )
EVT_MENU( ID_DEBUGCONSOLE, AppFrame::OnDebugConsole )
wxEND_EVENT_TABLE()

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size )
    : wxFrame( NULL, 1, title, pos, size )
{
    Util::Timer tm;
        
    CreateMenu();
    CreateStatusBar();
    SetStatusText( "Welcome to Memoriser!" );
    #ifdef _DEBUG
    CreateDebugFrame();
    #endif

    if ( Log != nullptr )
    {
        auto sec = tm.Toc();
        auto ms = tm.Adjust_Time( MS, sec );
        std::string str = "Main Frame Created: " + std::to_string( sec ) + "(sec), " + std::to_string( ms ) + "(ms)";
        LOGALL( LEVEL_INFO, str, FileLog );
    }
}

void AppFrame::InitLog( const std::string& logpath, const std::string& errpath )
{
    std::vector<Util::LogFormat> format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG };
    Log = new Util::Logging( format );
    FileLog = logpath;
    FileErr = errpath;
}

void AppFrame::PrintDebug( const std::string& str )
{
    this->mDebugTextField->SetEditable( true );
    this->mDebugTextField->AppendText( str );
    this->mDebugTextField->SetEditable( false );
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
    menuFile->Append( wxID_ANY, "&New\tCtrl-N" );
    menuFile->Append( wxID_ANY, "&Save\tCtrl-S" );
    menuFile->Append( wxID_ANY, "&Save As\tCtrl-Alt-S" );
    menuFile->Append( wxID_ANY, "&Save All\tCtrl-Shift-S" );
    menuFile->Append( wxID_ANY, "&Open\tCtrl-O" );
    menuFile->Append( wxID_ANY, "&Rename" );
    menuFile->Append( wxID_ANY, "&Close\tCtrl-W" );
    menuFile->Append( wxID_ANY, "&Close All\tCtrl-Shift-W" );
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

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append( wxID_ABOUT );
    menuHelp->Append( ID_DOCUMENTATION, "See Documentation" );

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

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuEdit, "&Edit" );
    menuBar->Append( menuSearch, "&Search" );
    menuBar->Append( menuView, "&View" );
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

void AppFrame::OnDebugConsole( wxCommandEvent& event )
{
    this->mDebugFrame->Show( true );
    this->mDebugFrame->Raise();
}

void AppFrame::OnDebugExit( wxCloseEvent& event )
{
    this->mDebugFrame->Show( false ); //hide instead of closing
}