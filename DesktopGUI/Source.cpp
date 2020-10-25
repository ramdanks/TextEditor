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
    void InitRoutine();
    void ShowSplashScreen();
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
        if ( mSplash != nullptr ) mSplash->Destroy();
        this->MainFrame->Show();
        this->MainFrame->Raise();
        this->MainFrame->SetFocus();
        LOGALL( LEVEL_INFO, "Application Created: " + TO_STR( Init.Toc() ) + "(ms)" );
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
    this->MainFrame = new AppFrame( APPNAME, wxPoint( 200, 200 ), wxSize( 800, 600 ) );
    THROW_ERR_IFNULLPTR( MainFrame, "Problem creating Application Frame OnInit wxApp!" );
    this->MainFrame->SetIcon( wxICON( APPICON ) );

    //launching autosave feature
    mSaver = new AutoSaver( Config::mAutosaveInterval );
    auto sucessDeploy = mSaver->Deploy();
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