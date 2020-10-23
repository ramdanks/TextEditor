#include <wx/icon.h>
#include <wx/splash.h>
#include "AppFrame.h"
#include "LogGUI.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "TextField.h"
#include "Config.h"

#define SplashScreenFile "resource/splash.bmp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    void InitLog();
    AppFrame* MainFrame;

private:
    void ShowSplashScreen();
    wxSplashScreen* mSplash;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    Util::Timer Init( "Init", MS, false );

    InitLog();
    Config::FetchConfiguration();
    if ( Config::mUseSplash ) ShowSplashScreen();

    this->MainFrame = new AppFrame( "Memoriser", wxPoint( 50, 50 ), wxSize( 800, 600 ) );
    this->MainFrame->SetIcon( wxICON( APPICON ) );
    if ( mSplash != nullptr ) mSplash->Destroy();
    this->MainFrame->Show( true );

    float ms = Init.Toc();
    LOGALL( LEVEL_INFO, "Application Created: " + std::to_string( ms ) + "(ms)", LOG_FILEPATH );

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

void MyApp::ShowSplashScreen()
{
    wxBitmap bitmap;
    if ( bitmap.LoadFile( SplashScreenFile, wxBITMAP_TYPE_ANY ) )
    {
        mSplash = new wxSplashScreen( bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 
                                      6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                      wxBORDER_SIMPLE | wxSTAY_ON_TOP );
        Sleep( 2000 );
    }
}