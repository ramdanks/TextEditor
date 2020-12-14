#include "StyleFrame.h"
#include "../Feature/Language.h"
#include "../Feature/Config.h"
#include "../TextField.h"

wxBEGIN_EVENT_TABLE( StyleFrame, wxFrame )
EVT_CLOSE( StyleFrame::OnClose )
EVT_BUTTON( 0, StyleFrame::OnOK )
EVT_BUTTON( 1, StyleFrame::OnCancel )
EVT_COLOURPICKER_CHANGED( -1, StyleFrame::OnColourPickerChanged )
EVT_FONTPICKER_CHANGED( -1, StyleFrame::OnFontPickerChanged )
wxEND_EVENT_TABLE()

StyleFrame::StyleFrame( wxWindow* parent )
	: wxFrame( parent, wxID_ANY, MSG_STYLECONFIG, wxDefaultPosition, wxSize( 530, 395 ),
			   wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW )
{
	this->SetIcon( wxICON( APP_ICON ) );
	CreateContent();
	AdjustColourPicker();
}

void StyleFrame::ShowAndFocus( bool show, bool focus )
{
	AdjustColourPicker();
	UpdatePreview();
	if ( show ) this->Show();
	if ( focus ) this->SetFocus();
}

void StyleFrame::OnOK( wxCommandEvent& event )
{
	wxBusyCursor busyCursor;
	wxWindowDisabler disabler;
	wxBusyInfo busyInfo( "Applying new Style" );

	Config::SetFont( mSP.FontPicker->GetSelectedFont() );
	Config::sStyle.TextBack    = mSP.TextBack->GetColour().GetRGB();
	Config::sStyle.TextFore    = mSP.TextFore->GetColour().GetRGB();
	Config::sStyle.Caret       = mSP.Caret->GetColour().GetRGB();
	Config::sStyle.LineBack    = mSP.LineBack->GetColour().GetRGB();
	Config::sStyle.Selection   = mSP.Selection->GetColour().GetRGB();
	Config::sStyle.LinenumBack = mSP.LinenumBack->GetColour().GetRGB();
	Config::sStyle.LinenumFore = mSP.LinenumFore->GetColour().GetRGB();

	for ( auto& page : TextField::mPageData )
	{
		TextField::LoadStyle( page.TextField );
		DictionaryFrame::ResetStyling( page.FilePath );
		DictionaryFrame::StartStyling( page.FilePath, Config::GetDictionaryFlags() );
	}

	TextField::MarginAutoAdjust();
	this->Show( false );
}

void StyleFrame::OnCancel( wxCommandEvent& event )
{
	this->Show( false );
}

void StyleFrame::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}

void StyleFrame::OnColourPickerChanged( wxColourPickerEvent& event )
{
	if ( !mSP.Theme->SetStringSelection( "Custom" ) )
	{
		mSP.Theme->AppendString( "Custom" );
		mSP.Theme->SetStringSelection( "Custom" );
	}
	UpdatePreview();
}

void StyleFrame::OnFontPickerChanged( wxFontPickerEvent& event )
{
	UpdatePreview();
}

void StyleFrame::UpdatePreview()
{
	mSP.Preview->StyleSetFont( wxSTC_STYLE_DEFAULT, mSP.FontPicker->GetSelectedFont() );
	mSP.Preview->StyleSetForeground( wxSTC_STYLE_DEFAULT, mSP.TextFore->GetColour() );
	mSP.Preview->StyleSetBackground( wxSTC_STYLE_DEFAULT, mSP.TextBack->GetColour() );
	mSP.Preview->StyleClearAll();
	mSP.Preview->SetCaretLineVisible( true );
	mSP.Preview->SetCaretLineBackground( mSP.LineBack->GetColour() );
	mSP.Preview->SetCaretForeground( mSP.Caret->GetColour() );
	mSP.Preview->SetSelBackground( true, mSP.Selection->GetColour() );
	mSP.Preview->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	mSP.Preview->StyleSetBackground( wxSTC_STYLE_LINENUMBER, mSP.LinenumBack->GetColour() );
	mSP.Preview->StyleSetForeground( wxSTC_STYLE_LINENUMBER, mSP.LinenumFore->GetColour() );

	wxString lines = wxString::Format( wxT( "%i" ), mSP.Preview->GetLineCount() );
	auto width = mSP.Preview->TextWidth( wxSTC_STYLE_LINENUMBER, lines + ' ' );
	mSP.Preview->SetMarginWidth( 0, width );
}

void StyleFrame::AdjustColourPicker()
{
	mSP.TextBack->SetColour   ( wxColour( Config::sStyle.TextBack    ) );
	mSP.TextFore->SetColour   ( wxColour( Config::sStyle.TextFore    ) );
	mSP.Caret->SetColour      ( wxColour( Config::sStyle.Caret       ) );
	mSP.LineBack->SetColour   ( wxColour( Config::sStyle.LineBack    ) );
	mSP.Selection->SetColour  ( wxColour( Config::sStyle.Selection   ) );
	mSP.LinenumBack->SetColour( wxColour( Config::sStyle.LinenumBack ) );
	mSP.LinenumFore->SetColour( wxColour( Config::sStyle.LinenumFore ) );
}

void StyleFrame::CreateContent()
{
	auto panel = new wxPanel( this );
	auto panelsizer = new wxBoxSizer( wxVERTICAL );

	//Style Panel
	mSP.Panel = new wxPanel( panel );

	new wxStaticText( mSP.Panel, -1, "Select Font:", wxPoint( 10, 10 ) );
	mSP.FontPicker = new wxFontPickerCtrl( mSP.Panel, -1, wxNullFont, wxPoint( 10, 30 ), wxSize( 150, 25 ) );
	mSP.FontPicker->SetSelectedFont( Config::BuildFont() );

	new wxStaticText( mSP.Panel, 01, "Select Theme:", wxPoint( 180, 10 ) );
	mSP.Theme = new wxComboBox( mSP.Panel, -1, wxEmptyString, wxPoint( 180, 30 ), wxSize( 150, 25 ) );

	auto PickerSB = new wxStaticBox( mSP.Panel, -1, "Colour Picker", wxPoint( 10, 65 ), wxSize( 240, 245 ) );

	mSP.TextBack    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 25 ) );
	mSP.TextFore    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 55 ) );
	mSP.LinenumBack = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 85 ) );
	mSP.LinenumFore = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 115 ) );
	mSP.Selection   = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 145 ) );
	mSP.Caret       = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 175 ) );
	mSP.LineBack    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 130, 205 ) );

	new wxStaticText( PickerSB, -1, "Background",        wxPoint( 10, 25  ) );
	new wxStaticText( PickerSB, -1, "Foreground",        wxPoint( 10, 55  ) );
	new wxStaticText( PickerSB, -1, "Margin Background", wxPoint( 10, 85  ) );
	new wxStaticText( PickerSB, -1, "Margin Foreground", wxPoint( 10, 115 ) );
	new wxStaticText( PickerSB, -1, "Selection",         wxPoint( 10, 145 ) );
	new wxStaticText( PickerSB, -1, "Caret",             wxPoint( 10, 175 ) );
	new wxStaticText( PickerSB, -1, "Caret Background",  wxPoint( 10, 205 ) );

	auto PreviewSB = new wxStaticBox( mSP.Panel, -1, "Preview", wxPoint( 265, 65 ), wxSize( 225, 245 ) );
	mSP.Preview = new wxStyledTextCtrl( PreviewSB, -1, wxPoint( 15, 22 ), wxSize( 195, 205 ) );
	mSP.Preview->AppendText( "Lorem ipsum dolor\r\n" );
	mSP.Preview->AppendText( "sit amet, consectetur\r\n" );
	mSP.Preview->AppendText( "adipiscing elit, sed\r\n" );
	mSP.Preview->AppendText( "do eiusmod tempor\r\n" );
	mSP.Preview->AppendText( "incididunt ut labore\r\n" );
	mSP.Preview->AppendText( "et dolore magna aliqua." );
	mSP.Preview->SetEditable( false );
	mSP.Preview->SetZoom( 4 );

	//Button
	mButton.OK = new wxButton( panel, 0, "OK" );
	mButton.Cancel = new wxButton( panel, 1, "Cancel" );
	auto btnsizer = new wxBoxSizer( wxHORIZONTAL );
	btnsizer->Add( mButton.OK, 0, wxRIGHT, 5 );
	btnsizer->Add( mButton.Cancel, 0 );

	panelsizer->Add( mSP.Panel, 1, wxEXPAND | wxBOTTOM, 10 );
	panelsizer->Add( btnsizer, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10 );
	panel->SetSizer( panelsizer );
}