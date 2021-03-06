//#include "stdafx.h"
#include "Feature/Config.h"
#include "Feature/LogGUI.h"
#include "Feature/Language.h"
#include "Feature/Image.h"
#include "Frame/AppFrame.h"
#include "TextField.h"

#ifndef _DIST
#define DEBUG_CONSOLE true
#else
#define DEBUG_CONSOLE false
#endif

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

private:
    bool InstanceExist();
    void ShowSplashScreen();
    void HideSplashScreen();
    void DestroySplashScreen();
    bool CatchArgs();

    AppFrame* mMainFrame;
    wxSplashScreen* mSplash;
    wxSingleInstanceChecker* mInstance;
};

wxIMPLEMENT_APP( MyApp ); //entry point program

bool MyApp::OnInit()
{
    PROFILE_FUNC();

    if ( InstanceExist() )
        return false;

    Config::Init();
    LogGUI::Init( NULL, DEBUG_CONSOLE );

    try
    {
        //initialization
        Config::FetchData();
        Image::FetchData();

        if ( Config::sGeneral.UseSplash )
            ShowSplashScreen();

        //load language from configuration
        if ( !Language::LoadMessage( static_cast<LanguageID>( Config::sGeneral.LanguageID ) ) )
        {
            LOG_ALL_FORMAT( LV_INFO, "Problem importing language with ID: %d", Config::sGeneral.LanguageID ); 
        }

        //create main frame
        mMainFrame = new AppFrame( wxPoint( 200, 200 ), wxSize( 800, 600 ) );
        mMainFrame->SetIcon( wxICON( ICON_APP ) );

        //catch expect argument to text file
        if( wxGetApp().argc != 1 )
            CatchArgs();

        //finally
        if ( Config::sGeneral.UseSplash )
            DestroySplashScreen();
        mMainFrame->Show();
        mMainFrame->Raise();
    }
    catch ( Util::Err& e )
    {
        LOG_FILE( LV_FATAL, e.Seek() );
        return false;
    }
    catch ( ... )
    {
        LOG_FILE( LV_FATAL, "Unhandled Exception at OnInit wxApp!" );
        return false;
    }
    return true;
}

int MyApp::OnExit()
{
    Config::SaveConfig();
    delete mInstance;
    return 0;
}

bool MyApp::InstanceExist()
{
    mInstance = new wxSingleInstanceChecker;
    return mInstance->IsAnotherRunning();
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
        LOG_ALL( LV_WARN, "Splash screen can not created!" );
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

bool MyApp::CatchArgs()
{
    PROFILE_FUNC();

    try
    {
        bool ok;
        std::string fpath;

        if ( wxIsAbsolutePath( wxGetApp().argv[1] ) )
        {
            fpath = std::string( wxGetApp().argv[1] );
        }
        else                                         
        {
            fpath = std::string( wxGetCwd() + '\\' + wxGetApp().argv[1] );
        }

        ok = TextField::LoadFile( fpath );
        THROW_ERR_IFNOT( ok, "TextField cannot load file:" + fpath );
    }
    catch ( Util::Err& e )
    {
        LOG_ALL( LV_ERROR, e.Seek() );
        return false;
    }
    return true;
}