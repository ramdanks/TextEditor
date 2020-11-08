#include "FindFrame.h"
#include <wx/panel.h>
#include "../../Utilities/Timer.h"
#include "../../Utilities/FindReplace.h"
#include "../Feature/Language.h"
#include "../Feature/LogGUI.h"

wxFrame* FindFrame::mFrame;
wxTextCtrl* FindFrame::mEntryReplace;
wxNotebook* FindFrame::mNotebook;
wxStyledTextCtrl* FindFrame::mTextField;
sPageAttrib* FindFrame::mFindAttrib;
sPageAttrib* FindFrame::mReplaceAttrib;

#define FIND_PAGE 0
#define REPLACE_PAGE 1

#define PANEL(x)  x->Panel
#define FENTRY(x) x->EntryFind
#define BOXMC(x)  x->BoxMatchCase
#define BOXMW(x)  x->BoxMatchWhole
#define BOXWA(x)  x->BoxWrapAround

char CharBuf[128];
#define SETBUF( msg, ... ) snprintf( CharBuf, sizeof CharBuf, msg, __VA_ARGS__ )

void FindFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, MSG_FIND, wxDefaultPosition, wxSize( 470, 260 ),
						  wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxFRAME_TOOL_WINDOW );
	mFrame->CreateStatusBar();

	mNotebook = new wxNotebook( mFrame, wxID_ANY );
	CreateFindPage();
	CreateReplacePage();

	mFrame->Bind( wxEVT_BUTTON, &OnFindNext, FIND_NEXT );
	mFrame->Bind( wxEVT_BUTTON, &OnFindPrev, FIND_PREV );
	mFrame->Bind( wxEVT_BUTTON, &OnCount, FIND_COUNT );
	mFrame->Bind( wxEVT_BUTTON, &OnReplace, FIND_REPLACE );
	mFrame->Bind( wxEVT_BUTTON, &OnReplaceAll, FIND_REPLACEALL );
	mFrame->Bind( wxEVT_BUTTON, &OnClose, FIND_CLOSE );
	mFrame->Bind( wxEVT_CLOSE_WINDOW, &OnCloseWindow );
	mNotebook->Bind( wxEVT_NOTEBOOK_PAGE_CHANGED, &OnPageChanged );
}

void FindFrame::UpdateInfo( wxStyledTextCtrl* tf, wxString filepath )
{
	mTextField = tf;
	if ( !filepath.IsEmpty() )
		mFrame->SetTitle( MSG_FIND + " - " + filepath );
}

void FindFrame::ShowAndFocus( bool find )
{
	if ( mFrame == nullptr ) return;
	find ? mNotebook->SetSelection( FIND_PAGE ) : mNotebook->SetSelection( REPLACE_PAGE );
	mFrame->SetStatusText( wxString() );
	mFrame->Show();
	mFrame->SetFocus();
}

inline wxString FindFrame::GetFindEntry()
{
	auto currentPage = mNotebook->GetSelection();
	if ( currentPage == FIND_PAGE ) return mFindAttrib->EntryFind->GetValue();
	else if ( currentPage == REPLACE_PAGE ) return mReplaceAttrib->EntryFind->GetValue();
}

void FindFrame::OnFindNext( wxCommandEvent& event )
{
	IFind( false );
}

void FindFrame::OnFindPrev( wxCommandEvent& event )
{
	IFind( true );
}

void FindFrame::OnCount( wxCommandEvent& event )
{
	Util::Timer tm( "Counting Words", MS, false );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot find empty word" );
		return;
	}

	int get = -1;
	uint32_t count = -1;
	do
	{
		get = mTextField->FindText( get + 1, mTextField->GetTextLength(), text, GetFlags() );
		count++;
	} while ( get != -1 );

	SETBUF( "Total words count: %u", count );
	mFrame->SetStatusText( CharBuf );
	
	SETBUF( "Counting (%u) words from document size (%d): %f (ms)", count, mTextField->GetTextLength(), tm.Toc() );
	LOGALL( LEVEL_TRACE, CharBuf );
}

void FindFrame::OnReplace( wxCommandEvent& event )
{
	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot replace empty word" );
		return;
	}
	if ( mEntryReplace->GetValue() == text )
	{
		mFrame->SetStatusText( "Find and Replace word already equal" );
		return;
	}
	
	if ( mTextField->GetSelectedText() != text )
	{
		auto NullCmdEvent = wxCommandEvent( wxEVT_NULL );
		OnFindNext( NullCmdEvent );
		return;
	}

	mTextField->ReplaceSelection( mEntryReplace->GetValue() );
}

void FindFrame::OnReplaceAll( wxCommandEvent& event )
{
	Util::Timer tm( "Replace All", MS, false );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot replace empty word" );
		return;
	}
	if ( mEntryReplace->GetValue() == text )
	{
		mFrame->SetStatusText( "Find and Replace word already equal" );
		return;
	}

	Util::sFindAttrib attrib;
	attrib.Find = std::string( FENTRY( mReplaceAttrib )->GetValue() );
	attrib.StartPos = 0;
	attrib.MatchCase = BOXMC( mReplaceAttrib )->GetValue();
	attrib.MatchWholeWord = BOXMW( mReplaceAttrib )->GetValue();
	if ( !BOXWA( mReplaceAttrib )->GetValue() )
		attrib.StartPos = mTextField->GetSelectionStart();

	std::string buffer = std::string( mTextField->GetValue() );
	auto count = Util::ReplaceAll( buffer, std::string( mEntryReplace->GetValue() ), attrib );
	if ( count == 0 )
	{
		mFrame->SetStatusText( "Cannot find that word from current page!" );
		return;
	}
	mTextField->SetText( buffer );

	SETBUF( "Total replaced words: %u", count );
	mFrame->SetStatusText( CharBuf );

	SETBUF( "Replace (%u) words from document size (%d): %f (ms)", count, mTextField->GetTextLength(), tm.Toc() );
	LOGALL( LEVEL_TRACE, CharBuf );
}

void FindFrame::IFind( bool reverse )
{
	Util::Timer tm( "Find Word", MS, false );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot find empty word" );
		return;
	}

	int atPos;
	auto flags = GetFlags();
	int selection = mTextField->GetSelectionStart();
	if ( reverse )
	{
		mTextField->SetSelectionStart( selection - 1 );
		mTextField->SearchAnchor();
		atPos = mTextField->SearchPrev( flags, text );
		if ( atPos == -1 && BOXWA( mFindAttrib )->GetValue() )
		{
			mTextField->SetSelectionStart( mTextField->GetTextLength() );
			mTextField->SearchAnchor();
			atPos = mTextField->SearchPrev( flags, text );
		}
	}
	else
	{
		mTextField->SetSelectionStart( selection + 1 );
		mTextField->SearchAnchor();
		atPos = mTextField->SearchNext( flags, text );
		if ( atPos == -1 && BOXWA( mFindAttrib )->GetValue() )
		{
			mTextField->SetSelectionStart( 0 );
			mTextField->SearchAnchor();
			atPos = mTextField->SearchNext( flags, text );
		}
	}

	// if still not found, then it's not there
	if ( atPos == -1 )
	{
		mTextField->SetSelectionStart( selection ); //reset
		mFrame->SetStatusText( "Cannot find that word from current page!" );
		return;
	}

	// finally go there
	mTextField->SetSelection( atPos, atPos + text.size() );
	mTextField->ScrollToLine( mTextField->GetCurrentLine() );
	
	SETBUF( "Word found at pos: %d, line: %d", atPos, mTextField->GetCurrentLine() + 1 );
	mFrame->SetStatusText( CharBuf );

	SETBUF( "Find single word: %f (ms)", tm.Toc() );
	LOGCONSOLE( LEVEL_TRACE, CharBuf );
}

void FindFrame::CreateFindPage()
{
	mFindAttrib = new sPageAttrib;
	PANEL( mFindAttrib ) = new wxPanel( mNotebook );
	FENTRY( mFindAttrib ) = new wxTextCtrl( PANEL( mFindAttrib ), wxID_ANY, "", wxPoint( 120, 15 ), wxSize( 215, 20 ) );

	new wxStaticText( PANEL( mFindAttrib ), wxID_ANY, "Find what", wxPoint( 30, 15 ) );
	new wxStaticText( PANEL( mFindAttrib ), wxID_ANY, ":", wxPoint( 110, 15 ) );

	BOXMW( mFindAttrib ) = new wxCheckBox( PANEL( mFindAttrib ), wxID_ANY, "Match whole word", wxPoint( 30, 90 ) );
	BOXMC( mFindAttrib ) = new wxCheckBox( PANEL( mFindAttrib ), wxID_ANY, "Match case", wxPoint( 30, 115 ) );
	BOXWA( mFindAttrib ) = new wxCheckBox( PANEL( mFindAttrib ), wxID_ANY, "Wrap around", wxPoint( 30, 140 ) );

	new wxButton( PANEL(mFindAttrib), FIND_NEXT, "Find Next", wxPoint( 350, 15 ), wxSize( 80, 25 ) );
	new wxButton( PANEL(mFindAttrib), FIND_PREV, "Find Prev", wxPoint( 350, 45 ), wxSize( 80, 25 ) );
	new wxButton( PANEL(mFindAttrib), FIND_COUNT, "Count", wxPoint( 350, 75 ), wxSize( 80, 25 ) );
	new wxButton( PANEL(mFindAttrib), FIND_CLOSE, "Close", wxPoint( 350, 105 ), wxSize( 80, 25 ) );

	mNotebook->AddPage( PANEL( mFindAttrib ), MSG_FIND );
}

void FindFrame::CreateReplacePage()
{
	mReplaceAttrib = new sPageAttrib;
	PANEL( mReplaceAttrib ) = new wxPanel( mNotebook );
	FENTRY( mReplaceAttrib ) = new wxTextCtrl( PANEL( mReplaceAttrib ), wxID_ANY, "", wxPoint( 120, 15 ), wxSize( 215, 20 ) );
	mEntryReplace = new wxTextCtrl( PANEL( mReplaceAttrib ), wxID_ANY, "", wxPoint( 120, 45 ), wxSize( 215, 20 ) );

	new wxStaticText( PANEL( mReplaceAttrib ), wxID_ANY, "Find what", wxPoint( 30, 15 ) );
	new wxStaticText( PANEL( mReplaceAttrib ), wxID_ANY, "Replace with", wxPoint( 30, 45 ) );
	new wxStaticText( PANEL( mReplaceAttrib ), wxID_ANY, ":", wxPoint( 110, 15 ) );
	new wxStaticText( PANEL( mReplaceAttrib ), wxID_ANY, ":", wxPoint( 110, 45 ) );

	BOXMW( mReplaceAttrib ) = new wxCheckBox( PANEL( mReplaceAttrib ), wxID_ANY, "Match whole word", wxPoint( 30, 90 ) );
	BOXMC( mReplaceAttrib ) = new wxCheckBox( PANEL( mReplaceAttrib ), wxID_ANY, "Match case", wxPoint( 30, 115 ) );
	BOXWA( mReplaceAttrib ) = new wxCheckBox( PANEL( mReplaceAttrib ), wxID_ANY, "Wrap around", wxPoint( 30, 140 ) );

	new wxButton( PANEL( mReplaceAttrib ), FIND_NEXT, "Find Next", wxPoint( 350, 15 ), wxSize( 80, 25 ) );
	new wxButton( PANEL( mReplaceAttrib ), FIND_PREV, "Find Prev", wxPoint( 350, 45 ), wxSize( 80, 25 ) );
	new wxButton( PANEL( mReplaceAttrib ), FIND_REPLACE, "Replace", wxPoint( 350, 75 ), wxSize( 80, 25 ) );
	new wxButton( PANEL( mReplaceAttrib ), FIND_REPLACEALL, "Replace All", wxPoint( 350, 105 ), wxSize( 80, 25 ) );
	new wxButton( PANEL( mReplaceAttrib ), FIND_CLOSE, "Close", wxPoint( 350, 135 ), wxSize( 80, 25 ) );

	mNotebook->AddPage( PANEL( mReplaceAttrib ), MSG_REPLACE );
}

void FindFrame::OnPageChanged( wxBookCtrlEvent& event )
{
	auto currentPage = mNotebook->GetSelection();
}

void FindFrame::OnCloseWindow( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void FindFrame::OnClose( wxCommandEvent& event )
{
	mFrame->Show( false );
}

int FindFrame::GetFlags()
{
	auto currentPage = mNotebook->GetSelection();
	sPageAttrib* where = mReplaceAttrib;
	if      ( currentPage == FIND_PAGE )    where = mFindAttrib;

	int flags = 0;
	if ( BOXMW( where )->GetValue() ) flags = flags | wxSTC_FIND_WHOLEWORD;
	if ( BOXMC( where )->GetValue() ) flags = flags | wxSTC_FIND_MATCHCASE;
	return flags;
}