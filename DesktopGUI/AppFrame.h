#pragma once
#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
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
    void InitFileSupport();
    void PrintDebug( const std::string& str );
    void AddNewTab( const std::string& name );
    void FetchTempFile();

private:
    //handle main frame
    void CreateMenu();
    void OnHello( wxCommandEvent& event );
    void OnExit( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );
    void OnDocumentation( wxCommandEvent& event );
    void OnLogDir( wxCommandEvent& event );
    void OnReportBug( wxCommandEvent& event );
    void OnOpenFile( wxCommandEvent& event );
    void OnSaveFile( wxCommandEvent& event );
    void OnTabClose( wxCommandEvent& event );
    void OnTabCloseAll( wxCommandEvent& event );
    void OnNewFile( wxCommandEvent& event );

    //handle debug frame
    void CreateDebugFrame();
    void OnDebugConsole( wxCommandEvent& event );
    void OnDebugExit( wxCloseEvent& event );

    //wxWidgets handle event (every class inherit from wxFrame should apply)
    wxDECLARE_EVENT_TABLE();

private:
    AppFrame* mAppFrame;
    wxStatusBar* mStatusBar;
    wxNotebook* mTab;
    std::vector<wxStyledTextCtrl*> mTextField;
    std::vector<std::string> SupportedFormat;
    
    uint32_t mTempAmount;

    wxFrame* mDebugFrame;
    wxStyledTextCtrl* mDebugTextField;
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
    ID_SAVEFILE,
    ID_TABCLOSE,
    ID_TABCLOSEALL,
    ID_NEWFILE
};

#define LOGFILE(l,msg,file)  Log->Log_File(l,msg,file)
#if defined( _DEBUG )
    #define LOGCONSOLE(l,msg)    PrintDebug( Log->Log_String(l,msg) )
#else
    #define LOGCONSOLE(l,msg)
#endif
#define LOGALL(l,msg,file)   LOGFILE(l,msg,file); LOGCONSOLE(l,msg)