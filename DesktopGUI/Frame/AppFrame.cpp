#include "AppFrame.h"
#include "../../Utilities/Timer.h"
#include "../../Utilities/Filestream.h"
#include "../Feature/Config.h"
#include "../Feature/LogGUI.h"
#include "../Feature/Language.h"
#include "../TextField.h"
#include "EventID.h"

wxBEGIN_EVENT_TABLE( AppFrame, wxFrame )

EVT_MENU( wxID_ABOUT, AppFrame::OnAbout )
EVT_MENU( ID_DOCUMENTATION, AppFrame::OnDocumentation )
EVT_MENU( ID_LOGDIR, AppFrame::OnLogDir )
EVT_MENU( ID_NEWDICT, AppFrame::OnNewDict )
EVT_MENU( ID_OPENDICT, AppFrame::OnOpenDict )
EVT_MENU( ID_REPORTBUG, AppFrame::OnReportBug )
EVT_MENU( ID_STAYONTOP, AppFrame::OnStayOnTop )
EVT_MENU( ID_DEBUGCONSOLE, AppFrame::OnDebugShow )
EVT_MENU( ID_PREFERENCES, AppFrame::OnPreferences )
EVT_MENU( ID_STYLECONFIG, AppFrame::OnStyleConfig )
EVT_MENU( wxID_EXIT, AppFrame::OnClose )
EVT_CLOSE( AppFrame::OnCloseWindow )

wxEND_EVENT_TABLE()

AppFrame::AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size, int wxAppID )
    : wxFrame( NULL, wxAppID, title, pos, size )
{
    Util::Timer Tm( std::string(), MS, false );

    this->mStatusBar = CreateStatusBar();
    this->mStatusBar->SetStatusText( MSG_STATUSBAR );

    TextField::Init( this );
    TextField::FetchTempFile();

    CreateMenu();
    BindMenu();

    //Remove frame creation at startup reduce launch time
    //mStyleFrame = new StyleFrame( this );
    //mPreferencesFrame = new PreferencesFrame( this );   

    //new wxStaticText( mStatusBar, wxID_ANY, MSG_STATUSBAR, wxPoint(5,5) );
    LOGCONSOLE( LEVEL_TRACE, "App Frame init time: " + TO_STR( Tm.Toc() ) + " (ms)" );
}

void AppFrame::CreateMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append( ID_NEWFILE,         MSG_NEW             + "\tCtrl-N"         );
    menuFile->Append( ID_SAVEFILE,        MSG_SAVE            + "\tCtrl-S"         );
    menuFile->Append( ID_SAVEFILEAS,      MSG_SAVEAS          + "\tCtrl-Alt-S"     );
    menuFile->Append( ID_SAVEFILEALL,     MSG_SAVEALL         + "\tCtrl-Shift-S"   );
    menuFile->Append( ID_OPENFILE,        MSG_OPEN            + "\tCtrl-O"         );
    menuFile->Append( ID_RENAMEFILE,      MSG_RENAME                               );
    menuFile->Append( ID_TABCLOSE,        MSG_CLOSE           + "\tCtrl-W"         );
    menuFile->Append( ID_TABCLOSEALL,     MSG_CLOSEALL        + "\tCtrl-Shift-W"   );                                           
    menuFile->AppendSeparator();                                                   
    menuFile->Append( wxID_EXIT,          MSG_EXIT            + "\tAlt+F4"         );
                                                                                  
    wxMenu* menuEdit = new wxMenu;                                                
    menuEdit->Append( ID_UNDO,            MSG_UNDO           + "\tCtrl-Z"          );
    menuEdit->Append( ID_REDO,            MSG_REDO           + "\tCtrl-Y"          );
    menuEdit->AppendSeparator();                                                   
    menuEdit->Append( ID_CUT,             MSG_CUT            + "\tCtrl-X"          );
    menuEdit->Append( ID_COPY,            MSG_COPY           + "\tCtrl-C"          );
    menuEdit->Append( ID_PASTE,           MSG_PASTE          + "\tCtrl-V"          );
    menuEdit->Append( ID_DELETE,          MSG_DELETE         + "\tDel"             );
    menuEdit->Append( ID_SELECTALL,       MSG_SELECTALL      + "\tCtrl-A"          );
    menuEdit->AppendSeparator();

    wxMenu* subCase = new wxMenu;
    subCase->Append( -1, "Uppercase" );
    subCase->Append( -1, "Lowercase" );
    subCase->Append( -1, "Inverse-case" );
    subCase->Append( -1, "Random-case" );
    menuEdit->Append( -1, "Case Conversion", subCase );

    wxMenu* subEOL = new wxMenu;
    subEOL->Append( ID_EOL_LF, "Unix (LF)" );
    subEOL->Append( ID_EOL_CR, "Macintosh (CR)" );
    subEOL->Append( ID_EOL_CRLF, "Windows (CR-LF)" );
    menuEdit->Append( -1, "EOL Conversion", subEOL );
                                                                                   
    wxMenu* menuSearch = new wxMenu;                                               
    menuSearch->Append( ID_FIND,          MSG_FIND           + "\tCtrl-F"          );
    menuSearch->Append( ID_SELECTFNEXT,   MSG_SELECTFNEXT    + "\tCtrl-F3"         );
    menuSearch->Append( ID_SELECTFPREV,   MSG_SELECTFPREV    + "\tCtrl-Shift-F3"   );
    menuSearch->Append( ID_REPLACE,       MSG_REPLACE        + "\tCtrl-H"          );
    menuSearch->Append( ID_GOTO,          MSG_GOTO           + "\tCtrl-G"          );
                                                             
    wxMenu* menuView = new wxMenu;                           
    menuView->Append( ID_STAYONTOP, MSG_AOT, wxEmptyString, wxITEM_CHECK );
    menuView->Append( ID_ZOOMIN,          MSG_ZOOMIN         + "\tCtrl-Num +"      );
    menuView->Append( ID_ZOOMOUT,         MSG_ZOOMOUT        + "\tCtrl-Num -"      );
    menuView->Append( ID_ZOOMRESTORE,     MSG_ZOOMRESTORE    + "\tCtrl-Num /"      );
    menuView->AppendSeparator();                                                   
    menuView->Append( ID_TEXTSUM,         MSG_TEXTSUM                              );
    menuView->Append( ID_COMPSUM,         MSG_COMPRESSIONSUM                       );
#ifndef _DIST                                                                  
    menuView->AppendSeparator();                                                   
    menuView->Append( ID_DEBUGCONSOLE,    MSG_DEBUGCONSOLE                         );
#endif                                                                        
                                                                                  
    wxMenu* menuSettings = new wxMenu;                                            
    menuSettings->Append( ID_PREFERENCES, MSG_PREFERENCES                          );
    menuSettings->Append( ID_STYLECONFIG, MSG_STYLECONFIG                          );

    wxMenu* menuDictionary = new wxMenu;
    menuDictionary->Append( -1, "Dictionary" );
    menuDictionary->Append( -1, "Embed" );
                                                                                  
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append( ID_REPORTBUG,       MSG_REPORTBUG                            );
    menuHelp->Append( ID_LOGDIR,          MSG_OPENLOGDIR                           );
    menuHelp->AppendSeparator();                                                   
    menuHelp->Append( ID_DOCUMENTATION,   MSG_SEEDOC                               );
    menuHelp->Append( wxID_ABOUT,         MSG_ABOUT                                );

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append( menuFile, MSG_FILE );
    menuBar->Append( menuEdit, MSG_EDIT );
    menuBar->Append( menuSearch, MSG_SEARCH );
    menuBar->Append( menuView, MSG_VIEW );
    menuBar->Append( menuSettings, MSG_SETTINGS );
    menuBar->Append( menuDictionary, "Dictionary" );
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
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnTextSummary, ID_TEXTSUM );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnCompSummary, ID_COMPSUM );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnGoto, ID_GOTO );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnFind, ID_FIND );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnReplace, ID_REPLACE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnFindNext, ID_SELECTFNEXT );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnFindPrev, ID_SELECTFPREV );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnEOL_CR, ID_EOL_CR );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnEOL_LF, ID_EOL_LF );
    Bind( wxEVT_COMMAND_MENU_SELECTED, TextField::OnEOL_CRLF, ID_EOL_CRLF );
}                                                            

void AppFrame::OnClose( wxCommandEvent& event )
{ 
    TextField::SaveTempAll();
    if ( LogGUI::LGUI != nullptr ) LogGUI::LGUI->Destroy();
    Close(); 
}

void AppFrame::OnCloseWindow( wxCloseEvent & event )
{
    if ( event.CanVeto() )
    {
        TextField::SaveTempAll();
        if ( !TextField::SaveToExit() )
        {
            auto prompt = wxMessageDialog( this, "There is non-temporary file which has been modified.\n"
                                           "Please save your work before exiting, any changes will be ignored.\n"
                                           "You sure want to Continue ?",
                                           "Close Window", wxYES_NO );
            if ( prompt.ShowModal() == wxID_NO )
            {
                event.Veto();
                return;
            }
        }
    }
    LOGALL( LEVEL_INFO, "==(PROGRAM EXITED)==" );
    if ( mStyleFrame != nullptr ) mStyleFrame->Destroy();
    if ( mPreferencesFrame != nullptr ) mPreferencesFrame->Destroy(); 
    if ( LogGUI::LGUI != nullptr ) LogGUI::LGUI->Destroy();
    Destroy();
}

void AppFrame::OnAbout( wxCommandEvent& event )
{
    char buf[64];
    snprintf( buf, sizeof buf, "%s version %s.%s", APP_NAME, MAJOR_VERSION, CHILD_VERSION );
    auto message = wxMessageDialog( this, buf, APP_NAME, wxSTAY_ON_TOP | wxOK );
    if ( message.ShowModal() == wxID_OK ) return;
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
    system( "explorer log" );
#elif defined( __APPLE__ )  
    system( "open log" );
#elif defined ( __linux__ ) 
    system( "nautilus log" );
#endif
}

void AppFrame::OnReportBug( wxCommandEvent& event )
{
}

void AppFrame::OnNewDict( wxCommandEvent& event )
{
    if ( mGridField == nullptr )  mGridField = new GridField( this );
    mGridField->Show();
    mGridField->OnNewDict();
}

void AppFrame::OnOpenDict( wxCommandEvent& event )
{
    if ( mGridField == nullptr )  mGridField = new GridField( this );
    mGridField->Show();
    mGridField->OnOpenDict();
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
    if ( mPreferencesFrame == nullptr ) mPreferencesFrame = new PreferencesFrame( this );
    mPreferencesFrame->ShowAndFocus( true, true );
}

void AppFrame::OnStyleConfig( wxCommandEvent& event )
{
    if ( mStyleFrame == nullptr ) mStyleFrame = new StyleFrame( this );
    mStyleFrame->ShowAndFocus( true, true );
}

void AppFrame::OnDebugShow( wxCommandEvent& event )
{
    LogGUI::LGUI->Show( true );
    LogGUI::LGUI->Raise();
}