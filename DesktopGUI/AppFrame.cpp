#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Filestream.h"
#include "Config.h"
#include "Language.h"
#include "EventID.h"
#include "TextField.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )
                                                        
EVT_MENU( wxID_ABOUT,        AppFrame::OnAbout )
EVT_MENU( ID_DOCUMENTATION,  AppFrame::OnDocumentation )
EVT_MENU( ID_LOGDIR,         AppFrame::OnLogDir )
EVT_MENU( ID_REPORTBUG,      AppFrame::OnReportBug )
EVT_MENU( ID_STAYONTOP,      AppFrame::OnStayOnTop )
EVT_MENU( ID_DEBUGCONSOLE,   AppFrame::OnDebugShow )
EVT_MENU( ID_PREFERENCES,    AppFrame::OnPreferences )
EVT_MENU( ID_STYLECONFIG,    AppFrame::OnStyleConfig )                                                                                      
EVT_MENU( wxID_EXIT,         AppFrame::OnClose )
EVT_CLOSE(                   AppFrame::OnCloseWindow )

wxEND_EVENT_TABLE()

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size, int wxAppID )
    : wxFrame( NULL, wxID_ANY, title, pos, size )
{
    TextField::Init( this );
    TextField::FetchTempFile();

    CreateMenu();
    BindMenu();

    mStyleFrame = new StyleFrame( this );
    mPreferencesFrame = new PreferencesFrame( this );
    
    this->mStatusBar = CreateStatusBar();
    this->mStatusBar->SetStatusText( MSG_STATUSBAR );
}

void AppFrame::CreateMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append( ID_NEWFILE,         MSG_NEW             + "\tCtrl-N"             );
    menuFile->Append( ID_SAVEFILE,        MSG_SAVE            + "\tCtrl-S"             );
    menuFile->Append( ID_SAVEFILEAS,      MSG_SAVEAS          + "\tCtrl-Alt-S"         );
    menuFile->Append( ID_SAVEFILEALL,     MSG_SAVEALL         + "\tCtrl-Shift-S"       );
    menuFile->Append( ID_OPENFILE,        MSG_OPEN            + "\tCtrl-O"             );
    menuFile->Append( ID_RENAMEFILE,      MSG_RENAME                                   );
    menuFile->Append( ID_TABCLOSE,        MSG_CLOSE           + "\tCtrl-W"             );
    menuFile->Append( ID_TABCLOSEALL,     MSG_CLOSEALL        + "\tCtrl-Shift-W"       );
    menuFile->AppendSeparator();                                                       
    menuFile->Append( wxID_ANY,           MSG_NEWDICT                                  );
    menuFile->Append( wxID_ANY,           MSG_OPENDICT                                 );
    menuFile->AppendSeparator();                                                       
    menuFile->Append( wxID_EXIT,          MSG_EXIT            + "\tAlt+F4"             );
                                                                                      
    wxMenu* menuEdit = new wxMenu;                                                    
    menuEdit->Append( ID_UNDO,            MSG_UNDO           + "\tCtrl-Z"              );
    menuEdit->Append( ID_REDO,            MSG_REDO           + "\tCtrl-Y"              );
    menuEdit->AppendSeparator();                                                       
    menuEdit->Append( ID_CUT,             MSG_CUT            + "\tCtrl-X"              );
    menuEdit->Append( ID_COPY,            MSG_COPY           + "\tCtrl-C"              );
    menuEdit->Append( ID_PASTE,           MSG_PASTE          + "\tCtrl-V"              );
    menuEdit->Append( ID_DELETE,          MSG_DELETE         + "\tDel"                 );
    menuEdit->Append( ID_SELECTALL,       MSG_SELECTALL      + "\tCtrl-A"              );
                                                                                       
    wxMenu* menuSearch = new wxMenu;                                                   
    menuSearch->Append( wxID_ANY,         MSG_FIND           + "\tCtrl-F"              );
    menuSearch->Append( wxID_ANY,         MSG_SELECTFNEXT    + "\tCtrl-F3"             );
    menuSearch->Append( wxID_ANY,         MSG_SELECTFPREV    + "\tCtrl-Shift-F3"       );
    menuSearch->Append( wxID_ANY,         MSG_REPLACE        + "\tCtrl-H"              );
    menuSearch->Append( wxID_ANY,         MSG_GOTO           + "\tCtrl-G"              );
                                                             
    wxMenu* menuView = new wxMenu;                           
    menuView->Append( ID_STAYONTOP, MSG_AOT, wxEmptyString, wxITEM_CHECK );
    menuView->Append( ID_ZOOMIN,          MSG_ZOOMIN         + "\tCtrl+Num +"          );
    menuView->Append( ID_ZOOMOUT,         MSG_ZOOMOUT        + "\tCtrl+Num -"          );
    menuView->Append( ID_ZOOMRESTORE,     MSG_ZOOMRESTORE    + "\tCtrl+Num /"          );
    menuView->AppendSeparator();                                                       
    menuView->Append( wxID_ANY,           MSG_TEXTSUM                                  );
    menuView->Append( wxID_ANY,           MSG_COMPRESSIONSUM                           );
    #ifdef _DEBUG                                                                      
    menuView->AppendSeparator();                                                       
    menuView->Append( ID_DEBUGCONSOLE,    MSG_DEBUGCONSOLE                             );
    #endif                                                                            
                                                                                      
    wxMenu* menuSettings = new wxMenu;                                                
    menuSettings->Append( ID_PREFERENCES, MSG_PREFERENCES                              );
    menuSettings->Append( ID_STYLECONFIG, MSG_STYLECONFIG                              );
                                                                                      
    wxMenu* menuHelp = new wxMenu;                                                    
    menuHelp->Append( ID_REPORTBUG,       MSG_REPORTBUG                                );
    menuHelp->Append( ID_LOGDIR,          MSG_OPENLOGDIR                               );
    menuHelp->AppendSeparator();                                                       
    menuHelp->Append( ID_DOCUMENTATION,   MSG_SEEDOC                                   );
    menuHelp->Append( wxID_ABOUT,         MSG_ABOUT                                    );

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile, MSG_FILE );
    menuBar->Append( menuEdit, MSG_EDIT );
    menuBar->Append( menuSearch, MSG_SEARCH );
    menuBar->Append( menuView, MSG_VIEW );
    menuBar->Append( menuSettings, MSG_SETTINGS );
    menuBar->Append( menuHelp, MSG_HELP );

    SetMenuBar( menuBar );
}

void AppFrame::BindMenu()
{
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnNewFile, ID_NEWFILE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnSaveFile, ID_SAVEFILE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnSaveFileAs, ID_SAVEFILEAS );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnSaveFileAll, ID_SAVEFILEALL );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnOpenFile, ID_OPENFILE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnRenameFile, ID_RENAMEFILE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnPageClose, ID_TABCLOSE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnPageCloseAll, ID_TABCLOSEALL );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnUndo, ID_UNDO );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnRedo, ID_REDO );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnCut, ID_CUT );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnCopy, ID_COPY );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnPaste, ID_PASTE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnDelete, ID_DELETE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnSelectAll, ID_SELECTALL );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnZoomIn, ID_ZOOMIN );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnZoomOut, ID_ZOOMOUT );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnZoomRestore, ID_ZOOMRESTORE );
    Bind( wxEVT_STC_ZOOM, TextField::OnSTCZoom );
}                                                            

void AppFrame::OnClose( wxCommandEvent& event )
{ 
    TextField::SaveTempAll();
    LogGUI::LGUI->HandleDestroy();
    Close( false ); 
}

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
    LOGALL( LEVEL_INFO, "==(PROGRAM EXITED)==" );
    LogGUI::LGUI->Destroy();
    mStyleFrame->Destroy();
    mPreferencesFrame->Destroy();
    Destroy();
}

void AppFrame::OnAbout( wxCommandEvent& event )
{
    wxLogMessage( APP_VER ); 
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

void AppFrame::OnStayOnTop( wxCommandEvent& event )
{
    static bool AlwaysOnTop = false;
    if ( AlwaysOnTop )
    {
        this->SetWindowStyleFlag( wxDEFAULT_FRAME_STYLE );
        AlwaysOnTop = false;
    }
    else
    {
        this->SetWindowStyleFlag( wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP );
        AlwaysOnTop = true;
    }
}

void AppFrame::OnPreferences( wxCommandEvent& event )
{
    mPreferencesFrame->ShowAndFocus( true, true );
}

void AppFrame::OnStyleConfig( wxCommandEvent& event )
{
    mStyleFrame->ShowAndFocus( true, true );
}

void AppFrame::OnDebugShow( wxCommandEvent& event )
{
    LogGUI::LGUI->Show( true );
}