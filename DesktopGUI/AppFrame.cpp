#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Filestream.h"
#include "TextField.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )

EVT_MENU( wxID_EXIT, AppFrame::OnClose )
EVT_MENU( wxID_ABOUT, AppFrame::OnAbout )
EVT_MENU( ID_DOCUMENTATION, AppFrame::OnDocumentation )
EVT_MENU( ID_DEBUGCONSOLE, AppFrame::OnDebugConsole )
EVT_MENU( ID_LOGDIR, AppFrame::OnLogDir )
EVT_MENU( ID_REPORTBUG, AppFrame::OnReportBug )
EVT_MENU( ID_TABCLOSE, AppFrame::OnPageClose )
EVT_MENU( ID_TABCLOSEALL, AppFrame::OnPageCloseAll )
EVT_MENU( ID_NEWFILE, AppFrame::OnNewFile )
EVT_MENU( ID_OPENFILE, AppFrame::OnOpenFile )
EVT_MENU( ID_SAVEFILE, AppFrame::OnSaveFile )
EVT_MENU( ID_SAVEFILEAS, AppFrame::OnSaveFileAs )
EVT_MENU( ID_SAVEFILEALL, AppFrame::OnSaveFileAll )
EVT_MENU( ID_RENAMEFILE, AppFrame::OnRenameFile )

EVT_AUINOTEBOOK_PAGE_CLOSE( wxID_ANY, AppFrame::OnNotebookPageClose )
EVT_CLOSE( AppFrame::OnCloseWindow )

wxEND_EVENT_TABLE()

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size, int wxAppID )
    : wxFrame( NULL, 1, title, pos, size )
{
    #ifdef _DEBUG
    CreateDebugFrame(); //create debug frame for logging on GUI
    #endif

    TextField::InitFilehandle( this );
    TextField::FetchTempFile();
    CreateMenu();
    
    this->mStatusBar = CreateStatusBar();
    this->mStatusBar->SetStatusText( "Welcome to Memoriser!" );
}

void AppFrame::CreateDebugFrame()
{
    mDebugFrame = new wxFrame( this, wxID_ANY, "Memoriser - Debug Console", wxDefaultPosition, wxSize( 600, 300 ) );
    mDebugFrame->Bind( wxEVT_CLOSE_WINDOW, &AppFrame::OnDebugExit, this ); //handle close window event (dont exit, just hide)
    mDebugTextField = new wxStyledTextCtrl( this->mDebugFrame, wxID_ANY, wxDefaultPosition );
    mDebugTextField->SetEditable( false );
    LogGUI::SetDebugTextField( this->mDebugTextField ); //LogFile static file for logging
    
    mDebugFrame->Show( true );
    mDebugTextField->Show( true );
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

void AppFrame::OnClose( wxCommandEvent& event )
{ TextField::SaveTempAll(); Close( false ); }

void AppFrame::OnCloseWindow( wxCloseEvent & event )
{
    if ( event.CanVeto() )
    {
        TextField::SaveTempAll();
        if ( TextField::ExistAbsoluteFile() )
        {
            auto prompt = wxMessageDialog( this, "A non temporary file detected in current Notebook.\n"
                                           "Please save your work, any changes will be ignored.\n"
                                           "Sure want to Continue ?",
                                           "Close Window", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO )
            {
                event.Veto();
                return;
            }
        }
    }
    Destroy();
}

void AppFrame::OnAbout( wxCommandEvent& event )
{ wxLogMessage( "Memoriser Ver 0.2" ); }

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
{ }

void AppFrame::OnPageClose( wxCommandEvent& event )
{ TextField::OnPageClose(); }

void AppFrame::OnPageCloseAll( wxCommandEvent& event )
{ TextField::OnPageCloseAll(); }

void AppFrame::OnNotebookPageClose( wxAuiNotebookEvent & evt )
{ evt.Veto(); TextField::OnPageClose(); } //veto event, because we handle event ourself

void AppFrame::OnNewFile( wxCommandEvent& event )
{ TextField::OnNewFile(); }

void AppFrame::OnOpenFile( wxCommandEvent& event )
{ TextField::OnOpenFile(); }

void AppFrame::OnSaveFile( wxCommandEvent& event )
{ TextField::OnSaveFile(); }

void AppFrame::OnSaveFileAs( wxCommandEvent& event )
{ TextField::OnSaveFileAs(); }

void AppFrame::OnSaveFileAll( wxCommandEvent& event )
{ TextField::OnSaveFileAll(); }

void AppFrame::OnRenameFile( wxCommandEvent& event )
{ TextField::OnRenameFile(); }

void AppFrame::OnDebugConsole( wxCommandEvent& event )
{
    mDebugFrame->Show( true );
    mDebugFrame->Raise();
}

void AppFrame::OnDebugExit( wxCloseEvent& event )
{ mDebugFrame->Show( false ); } //hide instead of closing