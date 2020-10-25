#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include <wx/aui/auibook.h>
#include "StyleFrame.h"
#include "PreferencesFrame.h"

#define APP_VER "Memoriser Ver 0.4"

class AppFrame : public wxFrame
{
public:
    AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size, int wxAppID = 0 );
    ~AppFrame() {}

private:
    //handle main frame
    void CreateMenu();
    void OnNewFile( wxCommandEvent& event );
    void OnClose( wxCommandEvent& event );
    void OnCloseWindow( wxCloseEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnDocumentation( wxCommandEvent& event );
    void OnLogDir( wxCommandEvent& event );
    void OnReportBug( wxCommandEvent& event );
    void OnPageClose( wxCommandEvent& event );
    void OnPageCloseAll( wxCommandEvent& event );
    void OnNotebookPageClose( wxAuiNotebookEvent& evt );

    void OnOpenFile( wxCommandEvent& event );
    void OnSaveFile( wxCommandEvent& event );
    void OnSaveFileAs( wxCommandEvent& event );
    void OnSaveFileAll ( wxCommandEvent& event );
    void OnRenameFile( wxCommandEvent& event );

    void OnUndo( wxCommandEvent& event );
    void OnRedo( wxCommandEvent& event );
    void OnCut( wxCommandEvent& event );
    void OnCopy( wxCommandEvent& event );
    void OnPaste( wxCommandEvent& event );
    void OnDelete ( wxCommandEvent& event );
    void OnSelectAll( wxCommandEvent& event );

    void OnStayOnTop( wxCommandEvent& event );
    void OnZoomIn( wxCommandEvent& event );
    void OnZoomOut( wxCommandEvent& event );
    void OnZoomRestore( wxCommandEvent& event );

    void OnPreferences( wxCommandEvent& event );
    void OnStyleConfig( wxCommandEvent& event );

    //handle debug frame
    void OnDebugShow( wxCommandEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:  
    wxStatusBar* mStatusBar;

    float AutosaveTime;
};

enum menu
{
    ID_HELLO,
    ID_HELP,
    ID_DOCUMENTATION,
    ID_DEBUGCONSOLE,
    ID_SETONTOP,
    ID_LOGDIR,
    ID_REPORTBUG,
    ID_OPENFILE,
    ID_RENAMEFILE,
    ID_SAVEFILE,
    ID_SAVEFILEAS,
    ID_SAVEFILEALL,
    ID_TABCLOSE,
    ID_TABCLOSEALL,
    ID_NEWFILE,
    ID_STAYONTOP,
    ID_ZOOMIN,
    ID_ZOOMOUT,
    ID_ZOOMRESTORE,
    ID_UNDO,
    ID_REDO,
    ID_CUT,
    ID_COPY,
    ID_PASTE,
    ID_DELETE,
    ID_SELECTALL,
    ID_PREFERENCES,
    ID_STYLECONFIG
};