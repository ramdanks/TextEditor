#include "AppFrame.h"
#include "../Utilities/Filestream.h"
#include "LogGUI.h"
#include "../Utilities/Timer.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;

private:
    void InitLog();
    AppFrame* MainFrame;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    InitLog();
    this->MainFrame = new AppFrame( "Memoriser", wxPoint( 50, 50 ), wxSize( 800, 600 ) );
    this->MainFrame->Show( true );
    return true;
}

void MyApp::InitLog()
{
    Util::Timer Init;
    Filestream::Create_Directories( "log" );
    Filestream::Create_Directories( "temp" );
    std::vector<Util::LogFormat> format = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG };
    LogGUI::SetLog( format );
    
    float sec = Init.Toc();
    float ms = Init.Adjust_Time( MS, sec );
    std::string str = "Application Created: " + std::to_string( sec ) + "(sec), " + std::to_string( ms ) + "(ms)";
    LOGALL( LEVEL_INFO, str, LOG_FILEPATH );
}