#include "PreferencesFrame.h"
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include "../Feature/LogGUI.h"
#include "../Feature/Language.h"

wxBEGIN_EVENT_TABLE( PreferencesFrame, wxFrame )
EVT_CLOSE( PreferencesFrame::OnClose )
wxEND_EVENT_TABLE()

PreferencesFrame::PreferencesFrame( wxWindow* parent )
	: wxFrame( parent, wxID_ANY, MSG_PREFERENCES, wxDefaultPosition, wxSize( 600,400 ),
			   wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW )
{
	wxArrayString LanguageList;
	LanguageList.Add( "English" );
	LanguageList.Add( "Bahasa Indonesia" );
	LanguageList.Add( "Spanish" );
	LanguageList.Add( "Portuguese" );
	LanguageList.Add( "German" );
	LanguageList.Add( "Italian" );
	LanguageList.Add( "French" );
	LanguageList.Add( "Dutch" );

	auto panel = new wxPanel( this );
	auto sbox = new wxStaticBox( panel, wxID_ANY, "Localization", wxPoint( 100, 20 ), wxSize( 196, 60 ) );
	new wxComboBox( sbox, wxID_ANY, "", wxPoint(13, 23), wxSize(170, 30) , LanguageList, wxCB_READONLY );

	auto system = new wxStaticBox( panel, wxID_ANY, "System", wxPoint( 320, 20 ), wxSize( 200, 300 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 25 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 50 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 75 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 100 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 125 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 150 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 175 ) );
	new wxCheckBox( system, wxID_ANY, "Feature", wxPoint( 15, 200 ) );
}

void PreferencesFrame::ShowAndFocus( bool show, bool focus )
{
	if ( show ) this->Show();
	if ( focus ) this->SetFocus();
}

void PreferencesFrame::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}