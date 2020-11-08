#include "GotoFrame.h"
#include <wx/radiobut.h>
#include <wx/panel.h>
#include "wx/textctrl.h"
#include <wx/statbox.h>
#include <wx/button.h>
#include "../../Utilities/Err.h"
#include "../Feature/Language.h"
#include "../Feature/LogGUI.h"

wxWindow* GotoFrame::mParent;
wxFrame* GotoFrame::mFrame;
wxStaticText* GotoFrame::mMaxSpecifier;
wxStaticText* GotoFrame::mCurrentSpecifier;
wxRadioButton* GotoFrame::mButtonLine;
wxRadioButton* GotoFrame::mButtonPos;
sGotoInfo GotoFrame::mInfo;
wxTextCtrl* GotoFrame::mEntryGoLine;
wxStyledTextCtrl* GotoFrame::mTextField;

void GotoFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, MSG_GOTO, wxDefaultPosition, wxSize( 350, 230 ),
						  wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW );

	auto panel           = new wxPanel( mFrame );
	auto SelectBox       = new wxStaticBox( panel, wxID_ANY, "Goto By", wxPoint( 40, 10 ), wxSize( 250, 50 ) );
	mButtonLine          = new wxRadioButton( SelectBox, wxID_ANY, "Line", wxPoint( 60, 20 ), wxSize( 70, 20 ) );
	mButtonPos           = new wxRadioButton( SelectBox, wxID_ANY, "Position", wxPoint( 140, 20 ), wxSize( 70, 20 ) );

	mCurrentSpecifier    = new wxStaticText( panel, wxID_ANY, "0", wxPoint( 190, 70 ), wxSize( 70, 20 ) );
	mMaxSpecifier        = new wxStaticText( panel, wxID_ANY, "0", wxPoint( 190, 95 ), wxSize( 70, 20 ) );

	auto TextCurrentLine = new wxStaticText( panel, wxID_ANY, "You are currently at", wxPoint( 30, 70 ) );
	auto TextMaxLine     = new wxStaticText( panel, wxID_ANY, "You can't go further than", wxPoint( 30, 95 ) );
	auto TextGoLine      = new wxStaticText( panel, wxID_ANY, "You want to go to", wxPoint( 30, 120 ) );
	mEntryGoLine         = new wxTextCtrl( panel, wxID_ANY, "", wxPoint( 190, 117 ), wxSize( 100, 20 ), wxTE_PROCESS_ENTER );

	auto ButtonGo        = new wxButton( panel, 1, "Go", wxPoint( 90, 153 ), wxSize( 70, 25 ) );
	auto ButtonCancel    = new wxButton( panel, 0, "Cancel", wxPoint( 170, 153 ), wxSize( 70, 25 ) );

	new wxStaticText( panel, wxID_ANY, ":", wxPoint( 180, 70 ) );
	new wxStaticText( panel, wxID_ANY, ":", wxPoint( 180, 95 ) );
	new wxStaticText( panel, wxID_ANY, ":", wxPoint( 180, 120 ) );

	mEntryGoLine->Bind( wxEVT_TEXT_ENTER, &OnGo );
	mButtonLine->Bind( wxEVT_RADIOBUTTON, &RadioLinePressed );
	mButtonPos->Bind( wxEVT_RADIOBUTTON, &RadioPosPressed );
	mFrame->Bind( wxEVT_CLOSE_WINDOW, &OnCloseWindow );
	ButtonCancel->Bind( wxEVT_BUTTON, &OnClose, 0 );
	ButtonGo->Bind( wxEVT_BUTTON, &OnGo, 1 );
}

void GotoFrame::ShowAndFocus()
{
	if ( mFrame == nullptr ) return;

	auto NullCmdEvent = wxCommandEvent( wxEVT_NULL );
	if ( mButtonLine->GetValue() ) RadioLinePressed( NullCmdEvent );
	else if ( mButtonPos->GetValue() ) RadioPosPressed( NullCmdEvent );

	mFrame->Show();
	mFrame->SetFocus();	
}

void GotoFrame::UpdateInfo( wxStyledTextCtrl* tf )
{
	if ( tf == nullptr ) return;
	mTextField = tf;
	mInfo.mMaxLine = tf->GetLineCount();
	mInfo.mMaxPos = tf->GetTextLength();
	mInfo.mCurrentPos = tf->GetCurrentPos();
	mInfo.mCurrentLine = tf->GetCurrentLine() + 1;
}

void GotoFrame::RadioLinePressed( wxCommandEvent& event )
{
	mMaxSpecifier->SetLabel( std::to_string( mInfo.mMaxLine ) );
	mCurrentSpecifier->SetLabel( std::to_string( mInfo.mCurrentLine ) );
}

void GotoFrame::RadioPosPressed( wxCommandEvent& event )
{
	mMaxSpecifier->SetLabel( std::to_string( mInfo.mMaxPos ) );
	mCurrentSpecifier->SetLabel( std::to_string( mInfo.mCurrentPos ) );
}

void GotoFrame::OnCloseWindow( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void GotoFrame::OnClose( wxCommandEvent& event )
{
	mFrame->Show( false );
}

void GotoFrame::OnGo( wxCommandEvent& event )
{
	std::string sVal = std::string( mEntryGoLine->GetLineText( 0 ) );
	try
	{
		auto GotoVal = std::stoi( sVal );
		if ( mButtonLine->GetValue() )
		{
			//line starts at 1 until maxline
			if ( GotoVal <= mInfo.mMaxLine && GotoVal > 0 )
			{
				mTextField->GotoLine( GotoVal-1 );
				LOGCONSOLE( LEVEL_TRACE, "Goto Line: " + sVal );
			}
		}
		else if ( mButtonPos->GetValue() )
		{
			//pos starts at 0 until last char
			if ( GotoVal <= mInfo.mMaxPos && GotoVal >= 0 )
			{
				mTextField->GotoPos( GotoVal );
				LOGCONSOLE( LEVEL_TRACE, "Goto Position: " + sVal );
			}
		}
	}
	catch( ... )
	{ 
		LOGCONSOLE( LEVEL_ERROR, "Cannot Goto with Value: " + sVal );
	}
	mFrame->Show( false );
}