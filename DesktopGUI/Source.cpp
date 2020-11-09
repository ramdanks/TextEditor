#include <wx/icon.h>
#include <wx/splash.h>
#include "../Utilities/Filestream.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Err.h"
#include "Feature/Config.h"
#include "Feature/LogGUI.h"
#include "Feature/Language.h"
#include "Feature/AutoSaver.h"
#include "Feature/Image.h"
#include "Frame/AppFrame.h"
#include "TextField.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

private:
    void ShowSplashScreen();
    void HideSplashScreen();
    void DestroySplashScreen();

    AppFrame* mMainFrame;
    wxSplashScreen* mSplash;
};

wxIMPLEMENT_APP( MyApp ); //entry point program

bool MyApp::OnInit()
{
    Util::Timer Init( "Init", MS, false );
#ifndef _DIST 
    LogGUI::LGUI = new LogGUI( NULL, true );  
#else
    LogGUI::LGUI = new LogGUI( NULL, false );
#endif
    try
    {
        //initialization
        Config::FetchConfiguration();
        Image::Init();

        if ( Config::mUseSplash ) ShowSplashScreen();

        //load language from configuration
        auto successLoadLang = Language::LoadMessage( static_cast<LanguageID>( Config::mLanguageID ) );
        
        if ( successLoadLang )
        { LOGALL( LEVEL_INFO, "Language imported from configuration: " + CV_STR( MSG_LANG ) ); }
        else
        {  LOGALL( LEVEL_INFO, "Language wont import with ID: " + TO_STR( Config::mLanguageID ) ); }

        //create main frame
        mMainFrame = new AppFrame( APP_NAME, wxPoint( 200, 200 ), wxSize( 800, 600 ) );
        THROW_ERR_IFNULLPTR( mMainFrame, "Problem creating Application Frame OnInit wxApp!" );
        mMainFrame->SetIcon( wxICON( APPICON ) );

        //autosaver
        AutoSaver::Init( Config::mAutosaveInterval );
        if ( Config::mUseAutosave ) AutoSaver::Deploy();

        //finally
        if ( Config::mUseSplash ) DestroySplashScreen();
        mMainFrame->Show();
        mMainFrame->Raise();
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
    LOGALL( LEVEL_INFO, "Application Created: " + TO_STR( Init.Toc() ) + " (ms)" );
    return true;
}

int MyApp::OnExit()
{
    return 0;
}

void MyApp::ShowSplashScreen()
{
   mSplash = new wxSplashScreen( IMG_SPLASH, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                 6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                 wxBORDER_SIMPLE | wxSTAY_ON_TOP );
}

void MyApp::HideSplashScreen()
{
    mSplash->Show( false );
}

inline void MyApp::DestroySplashScreen()
{
    mSplash->Destroy();
}