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

private:
    //handle main frame
    void CreateMenu();
    void BindMenu();
    void OnClose( wxCommandEvent& event );
    void OnCloseWindow( wxCloseEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnDocumentation( wxCommandEvent& event );
    void OnLogDir( wxCommandEvent& event );
    void OnReportBug( wxCommandEvent& event );

    void OnStayOnTop( wxCommandEvent& event );
    void OnPreferences( wxCommandEvent& event );
    void OnStyleConfig( wxCommandEvent& event );

    //handle debug frame
    void OnDebugShow( wxCommandEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:
    wxStatusBar* mStatusBar;
    StyleFrame* mStyleFrame;
    PreferencesFrame* mPreferencesFrame;

    float AutosaveTime;
};