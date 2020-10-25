#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Filestream.h"
#include "TextField.h"
#include "Config.h"
#include "Language.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )

EVT_MENU(                   ID_NEWFILE,                 AppFrame::OnNewFile )
EVT_MENU(                   ID_OPENFILE,                AppFrame::OnOpenFile )
EVT_MENU(                   ID_SAVEFILE,                AppFrame::OnSaveFile )
EVT_MENU(                   ID_SAVEFILEAS,              AppFrame::OnSaveFileAs )
EVT_MENU(                   ID_SAVEFILEALL,             AppFrame::OnSaveFileAll )
EVT_MENU(                   ID_RENAMEFILE,              AppFrame::OnRenameFile )
EVT_MENU(                   ID_TABCLOSE,                AppFrame::OnPageClose )
EVT_MENU(                   ID_TABCLOSEALL,             AppFrame::OnPageCloseAll )
EVT_MENU(                   wxID_EXIT,                  AppFrame::OnClose )

EVT_MENU(                   ID_UNDO,                    AppFrame::OnUndo )
EVT_MENU(                   ID_REDO,                    AppFrame::OnRedo )
EVT_MENU(                   ID_CUT,                     AppFrame::OnCut )
EVT_MENU(                   ID_COPY,                    AppFrame::OnCopy )
EVT_MENU(                   ID_PASTE,                   AppFrame::OnPaste )
EVT_MENU(                   ID_DELETE,                  AppFrame::OnDelete )
EVT_MENU(                   ID_SELECTALL,               AppFrame::OnSelectAll )

EVT_MENU(                   ID_STAYONTOP,               AppFrame::OnStayOnTop )
EVT_MENU(                   ID_ZOOMIN,                  AppFrame::OnZoomIn )
EVT_MENU(                   ID_ZOOMOUT,                 AppFrame::OnZoomOut )
EVT_MENU(                   ID_ZOOMRESTORE,             AppFrame::OnZoomRestore )
EVT_MENU(                   ID_DEBUGCONSOLE,            AppFrame::OnDebugShow )
                                                        
EVT_MENU(                   wxID_ABOUT,                 AppFrame::OnAbout )
EVT_MENU(                   ID_DOCUMENTATION,           AppFrame::OnDocumentation )
EVT_MENU(                   ID_LOGDIR,                  AppFrame::OnLogDir )
EVT_MENU(                   ID_REPORTBUG,               AppFrame::OnReportBug )

EVT_MENU(                   ID_PREFERENCES,             AppFrame::OnPreferences )
EVT_MENU(                   ID_STYLECONFIG,             AppFrame::OnStyleConfig )                                                    
                                                        
EVT_AUINOTEBOOK_PAGE_CLOSE( wxID_ANY,                   AppFrame::OnNotebookPageClose )
EVT_CLOSE( AppFrame::OnCloseWindow )

wxEND_EVENT_TABLE()

StyleFrame* mStyleFrame;
PreferencesFrame* mPreferencesFrame;

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size, int wxAppID )
    : wxFrame( NULL, wxID_ANY, title, pos, size )
{
    TextField::InitFilehandle( this );
    TextField::FetchTempFile();
    CreateMenu();

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

void AppFrame::OnClose( wxCommandEvent& event )
{ 
    TextField::SaveTempAll();
    Config::SaveConfiguration();
    LogGUI::LGUI->HandleDestroy();
    Close( false ); 
}

void AppFrame::OnCloseWindow( wxCloseEvent & event )
{
    if ( event.CanVeto() )
    {
        TextField::SaveTempAll();
        Config::SaveConfiguration();
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
{ }

void AppFrame::OnPageClose( wxCommandEvent& event )
{ 
    TextField::OnPageClose(); 
}

void AppFrame::OnPageCloseAll( wxCommandEvent& event )
{ 
    TextField::OnPageCloseAll(); 
}

void AppFrame::OnNotebookPageClose( wxAuiNotebookEvent & evt )
{ 
    evt.Veto(); //veto event, because we handle event ourself
    TextField::OnPageClose(); 
} 

void AppFrame::OnNewFile( wxCommandEvent& event )
{ 
    TextField::OnNewFile(); 
}

void AppFrame::OnOpenFile( wxCommandEvent& event )
{ 
    TextField::OnOpenFile(); 
}

void AppFrame::OnSaveFile( wxCommandEvent& event )
{ 
    TextField::OnSaveFile(); 
}

void AppFrame::OnSaveFileAs( wxCommandEvent& event )
{ 
    TextField::OnSaveFileAs(); 
}

void AppFrame::OnSaveFileAll( wxCommandEvent& event )
{ 
    TextField::OnSaveFileAll(); 
}

void AppFrame::OnRenameFile( wxCommandEvent& event )
{ 
    TextField::OnRenameFile(); 
}

void AppFrame::OnUndo( wxCommandEvent & event )
{
    TextField::OnUndo();
}

void AppFrame::OnRedo( wxCommandEvent& event )
{
    TextField::OnRedo();
}

void AppFrame::OnCut( wxCommandEvent& event )
{
    TextField::OnCut();
}

void AppFrame::OnCopy( wxCommandEvent& event )
{
    TextField::OnCopy();
}

void AppFrame::OnPaste( wxCommandEvent& event )
{
    TextField::OnPaste();
}

void AppFrame::OnDelete( wxCommandEvent& event )
{
    TextField::OnDelete();
}

void AppFrame::OnSelectAll( wxCommandEvent& event )
{
    TextField::OnSelectAll();
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

void AppFrame::OnZoomIn( wxCommandEvent & event )
{ 
    TextField::OnZoom( true, false ); 
}

void AppFrame::OnZoomOut( wxCommandEvent& event )
{ 
    TextField::OnZoom( false, false ); 
}

void AppFrame::OnZoomRestore( wxCommandEvent& event )
{ 
    TextField::OnZoom( false, true ); 
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