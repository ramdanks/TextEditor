#include <wx/icon.h>
#include <wx/splash.h>
#include "TextField.h"
#include "Config.h"
#include "AutoSaver.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Err.h"
#include "AppFrame.h"
#include "LogGUI.h"
#include "Language.h"

#define APPNAME "Mémoriser"
#define SLASHSCR_FILEPATH "resource/splash.bmp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    AppFrame* MainFrame;

private:
    inline void InitRoutine();
    inline void ShowSplashScreen();
    inline void HideSplashScreen();
    inline void DestroySplashScreen();
    wxSplashScreen* mSplash;
    AutoSaver* mSaver;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    Util::Timer Init( "Init", MS, false );
    try
    {
        //initialization
        LogGUI::LGUI = new LogGUI( NULL );
        Config::FetchConfiguration();
        if ( Config::mUseSplash ) ShowSplashScreen();

        InitRoutine();

        //finally
        if ( Config::mUseSplash ) DestroySplashScreen();
        this->MainFrame->Show();
        this->MainFrame->Raise();
    }
    catch ( Util::Err& e )
    {
        LOGFILE( LEVEL_FATAL, e.Seek() );
        return false;
    }
    catch ( ... )
    {
        LOGFILE( LEVEL_FATAL, "Unhandled Exception at OnInit wxApp!" );
        return false;
    }
    LOGALL( LEVEL_INFO, "Application Created: " + TO_STR( Init.Toc() ) + "(ms)" );
    return true;
}

int MyApp::OnExit()
{
    return 0;
}

void MyApp::InitRoutine()
{
    //load language from configuration
    auto successLoadLang = Language::LoadMessage( static_cast<LanguageID>( Config::mLanguageID ) );
    if ( successLoadLang )
    {
        LOGALL( LEVEL_INFO, "Language imported from configuration: " + MSG_LANG );
    }
    else
    {
        LOGALL( LEVEL_INFO, "Language wont import with ID: " + TO_STR( Config::mLanguageID ) );
    }

    //create main frame
    MainFrame = new AppFrame( APPNAME, wxPoint( 200, 200 ), wxSize( 800, 600 ) );
    THROW_ERR_IFNULLPTR( MainFrame, "Problem creating Application Frame OnInit wxApp!" );
    MainFrame->SetIcon( wxICON( APPICON ) );

    //launching autosave feature
    mSaver = new AutoSaver;
    auto sucessDeploy = mSaver->Deploy( Config::mAutosaveInterval );
    if ( !sucessDeploy )
    {
        LOGALL( LEVEL_WARN, "Thread wont deploy, disabling AutoSave feature!" );
    }
}

void MyApp::ShowSplashScreen()
{
    wxBitmap bitmap;
    if ( bitmap.LoadFile( SLASHSCR_FILEPATH, wxBITMAP_TYPE_ANY ) )
    {
        mSplash = new wxSplashScreen( bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                      6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                      wxBORDER_SIMPLE | wxSTAY_ON_TOP );
        //std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    }
}

void MyApp::HideSplashScreen()
{
    mSplash->Show( false );
}

inline void MyApp::DestroySplashScreen()
{
    mSplash->Destroy();
}
