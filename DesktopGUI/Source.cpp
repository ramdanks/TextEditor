#include <wx/wxprec.h>
#include <wx/stc/stc.h>
#include "AppFrame.h"
#include "../Utilities/Filestream.h"

#define ERR_FILEPATH	"Log/AppError.txt"
#define LOG_FILEPATH	"Log/AppLog.txt"

struct sColour
{ uint8_t r, g, b, a; };

class MyApp : public wxApp
{
public:
    bool OnInit() override;

private:
    AppFrame* MainFrame;
    wxStyledTextCtrl* TextField;
}; 

wxIMPLEMENT_APP( MyApp ); //entry point for program handled by wxWidgets

bool MyApp::OnInit()
{
    Filestream::Create_Directories( "Log" );
    AppFrame::InitLog( LOG_FILEPATH, ERR_FILEPATH );
    this->MainFrame = new AppFrame( "Memoriser", wxPoint( 50, 50 ), wxSize( 800, 600 ) );
    this->MainFrame->Show( true );
    this->TextField = new wxStyledTextCtrl( MainFrame );
    this->TextField->Show( true );
    return true;
}