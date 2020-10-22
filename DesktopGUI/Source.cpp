#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "TextField.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    void InitLog();
    AppFrame* MainFrame;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    Util::Timer Init;
    InitLog();
    this->MainFrame = new AppFrame( "Memoriser", wxPoint( 50, 50 ), wxSize( 800, 600 ) );
    this->MainFrame->Show( true );

    float sec = Init.Toc();
    float ms = Init.Adjust_Time( MS, sec );
    std::string str = "Application Created: " + std::to_string( sec ) + "(sec), " + std::to_string( ms ) + "(ms)";
    LOGALL( LEVEL_INFO, str, LOG_FILEPATH );

    return true;
}

int MyApp::OnExit()
{
    LOGFILE( LEVEL_INFO, "============ Program Exited ============", LOG_FILEPATH );
    return 0;
}

void MyApp::InitLog()
{
    Filestream::Create_Directories( "log" );
    std::vector<Util::LogFormat> format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG };
    LogGUI::SetLog( format );
}
