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
    Util::Timer tm( "Application Init", ADJUST, false );
#ifndef _DIST 
    LogGUI::sLogGUI = new LogGUI( NULL, true ); 
#else
    LogGUI::sLogGUI = new LogGUI( NULL, false );
#endif
    try
    {
        //initialization
        Config::FetchData();
        Image::FetchData();

        if ( Config::mUseSplash )
            ShowSplashScreen();

        //load language from configuration
        if ( Language::LoadMessage( static_cast<LanguageID>( Config::mLanguageID ) ) )
        {
            LOG_ALL( LEVEL_INFO, "Imported language from config: " + CV_STR( MSG_LANG ) );
        }
        else
        {  
            LOG_ALL_FORMAT( LEVEL_INFO, "Problem importing language with ID: %d", Config::mLanguageID ); 
        }

        //create main frame
        mMainFrame = new AppFrame( APP_NAME, wxPoint( 200, 200 ), wxSize( 800, 600 ) );
        THROW_ERR_IFNULLPTR( mMainFrame, "Problem creating Application Frame OnInit wxApp!" );
        mMainFrame->SetIcon( wxICON( ICON_APP ) );

        //autosaver
        if ( Config::mUseAutosave )
            AutoSaver::Deploy( Config::mAutosaveInterval );

        //finally
        if ( mSplash != nullptr )
            DestroySplashScreen();
        mMainFrame->Show();
        mMainFrame->Raise();
    }
    catch ( Util::Err& e )
    {
        LOG_FILE( LEVEL_FATAL, e.Seek() );
        return false;
    }
    catch ( ... )
    {
        LOG_FILE( LEVEL_FATAL, "Unhandled Exception at OnInit wxApp!" );
        return false;
    }
    LOG_ALL( LEVEL_INFO, tm.Toc_String() );
    return true;
}

int MyApp::OnExit()
{
    return 0;
}

void MyApp::ShowSplashScreen()
{
    try
    {
       mSplash = new wxSplashScreen( IMG_SPLASH, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                     6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                     wxBORDER_SIMPLE | wxSTAY_ON_TOP );
    }
    catch ( ... )
    {
        LOG_ALL( LEVEL_WARN, "Splash screen can not created!" );
    }
}

void MyApp::HideSplashScreen()
{
    mSplash->Show( false );
}

void MyApp::DestroySplashScreen()
{
    mSplash->Destroy();
}