#include "StyleFrame.h"
#include "../Feature/Language.h"

wxBEGIN_EVENT_TABLE( StyleFrame, wxFrame )
EVT_CLOSE( StyleFrame::OnClose )
wxEND_EVENT_TABLE()

StyleFrame::StyleFrame( wxWindow* parent )
	: wxFrame( parent, wxID_ANY, MSG_STYLECONFIG, wxDefaultPosition, wxSize( 600, 400 ),
			   wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW )
{
}

void StyleFrame::ShowAndFocus( bool show, bool focus )
{
	if ( show ) this->Show();
	if ( focus ) this->SetFocus();
}

void StyleFrame::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}