#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include "../Utilities/Logging.h"

static Util::Logging* Log;
static std::string FileErr;
static std::string FileLog;

class AppFrame : public wxFrame
{
public:
    AppFrame( const wxString& title, const wxPoint& pos, const wxSize& size );
    ~AppFrame() {}

    static void InitLog( const std::string& logpath, const std::string& errpath );
    void PrintDebug( const std::string& str );
private:
    //handle main frame
    void CreateMenu();
    void OnHello( wxCommandEvent& event );
    void OnExit( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnDocumentation( wxCommandEvent& event );

    //handle debug frame
    void CreateDebugFrame();
    void OnDebugConsole( wxCommandEvent& event );
    void OnDebugExit( wxCloseEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:
    AppFrame* mAppFrame;
    std::vector<wxStyledTextCtrl*> mTextField;

    wxFrame* mDebugFrame;
    wxStyledTextCtrl* mDebugTextField;
};

enum menu
{
    ID_HELLO,
    ID_HELP,
    ID_DOCUMENTATION,
    ID_DEBUGCONSOLE,
    ID_SETONTOP
};

#define LOGFILE(l,msg,file)  Log->Log_File(l,msg,file)
#if defined( _DEBUG )
    #define LOGCONSOLE(l,msg)    PrintDebug( Log->Log_String(l,msg) )
#else
    #define LOGCONSOLE(l,msg)
#endif
#define LOGALL(l,msg,file)   LOGFILE(l,msg,file); LOGCONSOLE(l,msg)