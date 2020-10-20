#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include "AppFrame.h"
#include "../Utilities/Filestream.h"

#define ERR_FILEPATH	"log/AppError.txt"
#define LOG_FILEPATH	"log/AppLog.txt"

struct sColour
{ uint8_t r, g, b, a; };

class MyApp : public wxApp
{
public:
    bool OnInit() override;

private:
    AppFrame* MainFrame;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    Filestream::Create_Directories( "log" );
    Filestream::Create_Directories( "temp" );
    AppFrame::InitLog( LOG_FILEPATH, ERR_FILEPATH );
    this->MainFrame = new AppFrame( "Memoriser", wxPoint( 50, 50 ), wxSize( 800, 600 ) );
    this->MainFrame->Show( true );
    return true;
}