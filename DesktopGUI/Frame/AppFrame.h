#pragma once
#include "../GridField.h"
#include "PreferencesFrame.h"
#include "StyleFrame.h"
#include "ShareFrame.h"

class AppFrame : public wxFrame
{
public:
    AppFrame( const wxPoint& pos, const wxSize& size );

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
    void OnShare( wxCommandEvent& event );

    //handle debug frame
    void OnDebugShow( wxCommandEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:
    GridField* mGridField;
    wxStatusBar* mStatusBar;
    StyleFrame* mStyleFrame;
    bool isPrefInit;
    bool isShareInit;
};