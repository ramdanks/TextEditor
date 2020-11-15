#include "DictionaryFrame.h"
#include "../Feature/Language.h"
#include <wx/icon.h>

wxWindow* DictionaryFrame::mParent;
wxFrame* DictionaryFrame::mFrame;
wxComboBox* DictionaryFrame::mComboBox;
wxPanel* DictionaryFrame::mPanel;

void DictionaryFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, "Dictionary", wxDefaultPosition, wxSize( 550, 500 ),
						  wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE );
	mFrame->SetIcon( wxICON( ICON_APP ) );
	mFrame->Bind( wxEVT_CLOSE_WINDOW, &OnCloseWindow );

	mPanel = new wxPanel( mFrame, -1 );

	auto hboxAccess = new wxBoxSizer( wxHORIZONTAL );
	auto hboxOption = new wxBoxSizer( wxHORIZONTAL );
	auto hboxButton = new wxBoxSizer( wxHORIZONTAL );
	auto vbox = new wxBoxSizer( wxVERTICAL );

	mComboBox = new wxComboBox( mPanel, -1 );
	mComboBox->SetEditable( false );
	auto buttonOF = new wxButton( mPanel, -1, "Open File" );
	auto buttonRS = new wxButton( mPanel, -1, "Remove Selected" );
	auto buttonRA = new wxButton( mPanel, -1, "Remove All" );
	hboxAccess->Add( buttonOF, 0 );
	hboxAccess->Add( buttonRS, 0 );
	hboxAccess->Add( buttonRA, 0, wxRIGHT, 10 );
	hboxAccess->Add( mComboBox, 1 );

	auto buttonOK = new wxButton( mPanel, -1, "OK" );
	auto buttonCC = new wxButton( mPanel, -1, "Cancel" );
	hboxButton->Add( buttonOK, 0 );
	hboxButton->Add( buttonCC, 0 );

	auto opt1 = new wxCheckBox( mPanel, -1, "Match Case" );
	auto opt2 = new wxCheckBox( mPanel, -1, "Match Whole Word" );
	auto opt3 = new wxCheckBox( mPanel, -1, "Hide Colouring" );
	hboxOption->Add( opt1, 0 );
	hboxOption->Add( opt2, 0 );
	hboxOption->Add( opt3, 0 );

	auto embedBox = new wxStaticBox( mPanel, -1, "Embedded to This File" );
	auto removeBox = new wxStaticBox( mPanel, -1, "Drag Here to Remove" );

	vbox->Add( -1, 10 );
	vbox->Add( hboxAccess, 0, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( 20, 10 );
	vbox->Add( embedBox, 3, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( removeBox, 2, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxOption, 0, wxLEFT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxButton, 0, wxALIGN_RIGHT | wxRIGHT, 10 );
	vbox->Add( -1, 10 );

	mPanel->SetSizer( vbox );
	buttonOK->Bind( wxEVT_BUTTON, OnOK );
	buttonCC->Bind( wxEVT_BUTTON, OnClose );
}

void DictionaryFrame::ShowAndFocus()
{
	if ( mFrame == nullptr ) return;

	mFrame->SetStatusText( wxString() );
	mFrame->Show();
	mFrame->SetFocus();
}

void DictionaryFrame::UpdateComboBox( const wxArrayString& list, int selection )
{
	mComboBox->Set( list );
	mComboBox->SetSelection( selection );
}

void DictionaryFrame::OnCloseWindow( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void DictionaryFrame::OnClose( wxCommandEvent& event )
{
	mFrame->Show( false );
}

void DictionaryFrame::OnOK( wxCommandEvent& event )
{
	mFrame->Show( false );
}