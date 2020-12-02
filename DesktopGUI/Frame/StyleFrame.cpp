#include "StyleFrame.h"
#include "../Feature/Language.h"
#include "../Feature/Config.h"
#include "../TextField.h"
#include <wx/busyinfo.h>

wxBEGIN_EVENT_TABLE( StyleFrame, wxFrame )
EVT_CLOSE( StyleFrame::OnClose )
EVT_BUTTON( 0, StyleFrame::OnOK )
EVT_BUTTON( 1, StyleFrame::OnCancel )
EVT_COLOURPICKER_CHANGED( -1, StyleFrame::OnColourPickerChanged )
wxEND_EVENT_TABLE()

StyleFrame::StyleFrame( wxWindow* parent )
	: wxFrame( parent, wxID_ANY, MSG_STYLECONFIG, wxDefaultPosition, wxSize( 530, 380 ),
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

	Config::mTextBack    = mSP.TextBack->GetColour().GetRGB();
	Config::mTextFore    = mSP.TextFore->GetColour().GetRGB();
	Config::mCaret       = mSP.Caret->GetColour().GetRGB();
	Config::mLineBack    = mSP.LineBack->GetColour().GetRGB();
	Config::mSelection   = mSP.Selection->GetColour().GetRGB();
	Config::mLinenumBack = mSP.LinenumBack->GetColour().GetRGB();
	Config::mLinenumFore = mSP.LinenumFore->GetColour().GetRGB();

	for ( auto& page : TextField::mPageData )
		TextField::LoadStyle( page.TextField );

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

void StyleFrame::UpdatePreview()
{
	mSP.Preview->StyleSetForeground( wxSTC_STYLE_DEFAULT, mSP.TextFore->GetColour() );
	mSP.Preview->StyleSetBackground( wxSTC_STYLE_DEFAULT, mSP.TextBack->GetColour() );
	mSP.Preview->StyleClearAll();
	mSP.Preview->SetCaretLineVisible( true );
	mSP.Preview->SetCaretLineBackground( mSP.LineBack->GetColour() );
	mSP.Preview->SetCaretForeground( mSP.Caret->GetColour() );
	mSP.Preview->SetSelBackground( true, mSP.Selection->GetColour() );
	mSP.Preview->SetMarginWidth( 0, 27 );
	mSP.Preview->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	mSP.Preview->StyleSetBackground( wxSTC_STYLE_LINENUMBER, mSP.LinenumBack->GetColour() );
	mSP.Preview->StyleSetForeground( wxSTC_STYLE_LINENUMBER, mSP.LinenumFore->GetColour() );
}

void StyleFrame::AdjustColourPicker()
{
	mSP.TextBack->SetColour( wxColour( Config::mTextBack ) );
	mSP.TextFore->SetColour( wxColour( Config::mTextFore ) );
	mSP.Caret->SetColour( wxColour( Config::mCaret ) );
	mSP.LineBack->SetColour( wxColour( Config::mLineBack ) );
	mSP.Selection->SetColour( wxColour( Config::mSelection ) );
	mSP.LinenumBack->SetColour( wxColour( Config::mLinenumBack ) );
	mSP.LinenumFore->SetColour( wxColour( Config::mLinenumFore ) );
}

void StyleFrame::CreateContent()
{
	auto panel = new wxPanel( this );
	auto panelsizer = new wxBoxSizer( wxVERTICAL );

	int style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_SCROLL_BUTTONS;
	mNotebook = new wxAuiNotebook( panel, -1, wxDefaultPosition, wxDefaultSize, style );

	mFP.Panel = new wxPanel( mNotebook );
	mSP.Panel = new wxPanel( mNotebook );
	mNotebook->AddPage( mFP.Panel, "Font" );
	mNotebook->AddPage( mSP.Panel, "Style" );

	//Font Panel
	auto NameST = new wxStaticText( mFP.Panel, -1, "Font:" );
	mFP.NameTC = new wxTextCtrl( mFP.Panel, -1, wxEmptyString, wxDefaultPosition, wxSize( 150, 25 ) );
	mFP.NameLB = new wxListBox( mFP.Panel, -1, wxDefaultPosition, wxSize( 150, 200 ) );
	auto sizerName = new wxBoxSizer( wxVERTICAL );
	sizerName->Add( NameST, 0 );
	sizerName->Add( mFP.NameTC, 1 );
	sizerName->Add( mFP.NameLB, 0 );

	auto StyleST = new wxStaticText( mFP.Panel, -1, "Font Style:" );
	mFP.StyleTC = new wxTextCtrl( mFP.Panel, -1, wxEmptyString, wxDefaultPosition, wxSize( 120, 25 ) );
	mFP.StyleLB = new wxListBox( mFP.Panel, -1, wxDefaultPosition, wxSize( 120, 200 ) );
	auto sizerStyle = new wxBoxSizer( wxVERTICAL );
	sizerStyle->Add( StyleST, 0 );
	sizerStyle->Add( mFP.StyleTC, 1 );
	sizerStyle->Add( mFP.StyleLB, 0 );

	auto sizesb = new wxStaticBox( mFP.Panel, -1, "Font Size", wxDefaultPosition, wxSize( 150, 60 ) );
	mFP.SizeCB = new wxComboBox( sizesb, -1, wxEmptyString, wxPoint(18,23), wxSize( 115, 25 ) );
	mFP.SampleSB = new wxStaticBox( mFP.Panel, -1, "Samples", wxDefaultPosition, wxSize( 150, 120 ) );
	auto sizerFont = new wxBoxSizer( wxVERTICAL );
	sizerFont->Add( sizesb, 0 );
	sizerFont->Add( mFP.SampleSB, 1 );

	auto fontsizer = new wxBoxSizer( wxHORIZONTAL );
	fontsizer->Add( 20, 0 );
	fontsizer->Add( sizerName, 0, wxCENTER );
	fontsizer->Add( 20, 0 );
	fontsizer->Add( sizerStyle, 0, wxCENTER );
	fontsizer->Add( 20, 0 );
	fontsizer->Add( sizerFont, 0, wxCENTER );
	fontsizer->Add( 20, 0 );
	mFP.Panel->SetSizer( fontsizer );

	//Style Panel
	auto PickerSB = new wxStaticBox( mSP.Panel, -1, "Colour Picker", wxPoint( 10, 10 ), wxSize( 285, 250 ) );

	mSP.TextBack    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 30 ) );
	mSP.TextFore    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 60 ) );
	mSP.LinenumBack = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 90 ) );
	mSP.LinenumFore = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 120 ) );
	mSP.Selection   = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 150 ) );
	mSP.Caret       = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 180 ) );
	mSP.LineBack    = new wxColourPickerCtrl( PickerSB, -1, wxColour( 0, 0, 0 ), wxPoint( 170, 210 ) );

	new wxStaticText( PickerSB, -1, "Background",             wxPoint( 10, 30  ) );
	new wxStaticText( PickerSB, -1, "Foreground",             wxPoint( 10, 60  ) );
	new wxStaticText( PickerSB, -1, "Line Number Background", wxPoint( 10, 90  ) );
	new wxStaticText( PickerSB, -1, "Line Number Foreground", wxPoint( 10, 120 ) );
	new wxStaticText( PickerSB, -1, "Selection",              wxPoint( 10, 150 ) );
	new wxStaticText( PickerSB, -1, "Caret",                  wxPoint( 10, 180 ) );
	new wxStaticText( PickerSB, -1, "Caret Line Background",  wxPoint( 10, 210 ) );

	auto PreviewSB = new wxStaticBox( mSP.Panel, -1, "Preview", wxPoint( 310, 60 ), wxSize( 180, 200 ) );
	mSP.Preview = new wxStyledTextCtrl( PreviewSB, -1, wxPoint( 15, 22 ), wxSize( 150, 165 ) );
	mSP.Preview->AppendText( "Lorem ipsum dolor\r\n" );
	mSP.Preview->AppendText( "sit amet, consectetur\r\n" );
	mSP.Preview->AppendText( "adipiscing elit, sed\r\n" );
	mSP.Preview->AppendText( "do eiusmod tempor\r\n" );
	mSP.Preview->AppendText( "incididunt ut labore\r\n" );
	mSP.Preview->AppendText( "et dolore magna aliqua." );
	mSP.Preview->SetEditable( false );
	mSP.Preview->SetZoom( 4 );
	mSP.Theme = new wxComboBox( mSP.Panel, -1, wxEmptyString, wxPoint( 310, 30 ), wxSize( 180, 25 ) );
	new wxStaticText( mSP.Panel, 01, "Select Theme:", wxPoint( 310, 10 ) );

	//Button
	mButton.OK = new wxButton( panel, 0, "OK" );
	mButton.Cancel = new wxButton( panel, 1, "Cancel" );
	auto btnsizer = new wxBoxSizer( wxHORIZONTAL );
	btnsizer->Add( mButton.OK, 0, wxRIGHT, 5 );
	btnsizer->Add( mButton.Cancel, 0 );

	panelsizer->Add( mNotebook, 1, wxEXPAND | wxBOTTOM, 10 );
	panelsizer->Add( btnsizer, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10 );
	panel->SetSizer( panelsizer );
}