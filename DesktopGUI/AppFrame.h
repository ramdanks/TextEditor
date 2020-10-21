#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include <wx/aui/auibook.h>

class AppFrame : public wxFrame
{
public:
    AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size );
    ~AppFrame() {}

private:
    //handle main frame
    void CreateMenu();
    void OnNewFile( wxCommandEvent& event );
    void OnExit( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnDocumentation( wxCommandEvent& event );
    void OnLogDir( wxCommandEvent& event );
    void OnReportBug( wxCommandEvent& event );
    void OnTabClose( wxCommandEvent& event );
    void OnTabCloseAll( wxCommandEvent& event );
    void OnNotebookPageClose( wxAuiNotebookEvent& evt );

    void OnOpenFile( wxCommandEvent& event );
    void OnSaveFile( wxCommandEvent& event );
    void OnSaveFileAs( wxCommandEvent& event );
    void OnSaveFileAll ( wxCommandEvent& event );
    void OnRenameFile( wxCommandEvent& event );

    //handle debug frame
    void CreateDebugFrame();
    void OnDebugConsole( wxCommandEvent& event );
    void OnDebugExit( wxCloseEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:
    wxFrame* mDebugFrame;
    wxStyledTextCtrl* mDebugTextField;  
    AppFrame* mAppFrame;
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
    ID_NEWFILE
};