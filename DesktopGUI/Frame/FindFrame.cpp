#include "FindFrame.h"
#include "../Feature/Language.h"
#include "../Feature/LogGUI.h"
#include "../../Utilities/FindReplace.h"
#include "../TextField.h"

wxFrame* FindFrame::mFrame;
wxTextCtrl* FindFrame::mEntryReplace;
wxNotebook* FindFrame::mNotebook;
sPageAttrib* FindFrame::mFindAttrib;
sPageAttrib* FindFrame::mReplaceAttrib;
char FindFrame::buf[256];

#define FIND_PAGE 0
#define REPLACE_PAGE 1

#define PANEL(x)  x->Panel
#define FENTRY(x) x->EntryFind
#define BOXMC(x)  x->BoxMatchCase
#define BOXMW(x)  x->BoxMatchWhole
#define BOXWA(x)  x->BoxWrapAround

#define SETBUF( msg, ... ) snprintf( buf, sizeof buf, msg, __VA_ARGS__ )

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

void FindFrame::ShowAndFocus( bool find )
{
	if ( mFrame )
	{
		if ( find ) mNotebook->SetSelection( FIND_PAGE );
		else        mNotebook->SetSelection( REPLACE_PAGE );
		mFrame->SetStatusText( wxString() );
		mFrame->Show();
		mFrame->SetFocus();
	}
}

inline wxString FindFrame::GetFindEntry()
{
	auto currentPage = mNotebook->GetSelection();
	if ( currentPage == FIND_PAGE ) return mFindAttrib->EntryFind->GetValue();
	else if ( currentPage == REPLACE_PAGE ) return mReplaceAttrib->EntryFind->GetValue();
}

void FindFrame::OnFindNext( wxCommandEvent& event )
{
	IFind( false, GetFindEntry() );
}

void FindFrame::OnFindPrev( wxCommandEvent& event )
{
	IFind( true, GetFindEntry() );
}

void FindFrame::OnCount( wxCommandEvent& event )
{
	TIMER_ONLY( tm, MS, false );

	auto ac = TextField::GetActivePage();
	auto stc = TextField::GetSTC( ac );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot Find: No Entry" );
		return;
	}

	int get = -1;
	uint32_t count = -1;
	do
	{
		get = stc->FindText( get + 1, stc->GetTextLength(), text, GetFlags() );
		count++;
	} while ( get != -1 );

	SETBUF( "Total words count: %u", count );
	mFrame->SetStatusText( buf );

	LOG_ALL_FORMAT( LV_TRACE, "Counting (%u) words from document size (%d): %.2f (ms)", count, stc->GetTextLength(), tm.toc() );
}

void FindFrame::OnReplace( wxCommandEvent& event )
{
	auto ac = TextField::GetActivePage();
	auto stc = TextField::GetSTC( ac );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot replace empty word" );
		return;
	}

	auto search = mReplaceAttrib->EntryFind->GetValue();
	if ( IFind( false, search ) != wxNOT_FOUND )
		stc->ReplaceSelection( mEntryReplace->GetValue() );
}

void FindFrame::OnReplaceAll( wxCommandEvent& event )
{
	TIMER_ONLY( tm, MS, false );

	auto ac = TextField::GetActivePage();
	auto stc = TextField::GetSTC( ac );

	auto text = GetFindEntry();
	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot Find: No Entry" );
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
		attrib.StartPos = stc->GetSelectionStart();

	std::string buffer = std::string( stc->GetValue() );
	auto count = Util::ReplaceAll( buffer, std::string( mEntryReplace->GetValue() ), attrib );
	if ( count == 0 )
	{
		if ( IsWrapAround() ) mFrame->SetStatusText( "Cannot Find: Not Exists." );
		else                  mFrame->SetStatusText( "Cannot Find: End of Line Reached." );
		return;
	}
	stc->SetText( buffer );

	SETBUF( "Total replaced words: %u", count );
	mFrame->SetStatusText( buf );

	LOG_ALL_FORMAT( LV_TRACE, "Replace (%u) words from document size (%d): %.2f (ms)", count, stc->GetTextLength(), tm.toc() );
}

int FindFrame::IFind( bool reverse, const wxString& text )
{
	TIMER_ONLY( tm, MS, false );

	if ( text.IsEmpty() )
	{
		mFrame->SetStatusText( "Cannot Find: No Entry" );
		return wxNOT_FOUND;
	}

	auto ac = TextField::GetActivePage();
	auto stc = TextField::GetSTC( ac );

	int atPos;
	const int flags = GetFlags();
	const int selection = stc->GetSelectionStart();
	if ( reverse )
	{
		stc->SearchAnchor();
		atPos = stc->SearchPrev( flags, text );
		if ( atPos == wxNOT_FOUND && IsWrapAround() )
		{
			stc->SetSelectionStart( stc->GetTextLength() );
			stc->SearchAnchor();
			atPos = stc->SearchPrev( flags, text );
		}
	}
	else
	{
		stc->SetSelectionStart( selection + 1 );
		stc->SearchAnchor();
		atPos = stc->SearchNext( flags, text );
		if ( atPos == wxNOT_FOUND && IsWrapAround() )
		{
			stc->SetSelectionStart( 0 );
			stc->SearchAnchor();
			atPos = stc->SearchNext( flags, text );
		}
	}

	// if still not found, then it's not there
	if ( atPos == wxNOT_FOUND )
	{
		stc->SetSelectionStart( selection ); //reset
		if ( IsWrapAround() ) mFrame->SetStatusText( "Cannot Find: Not Exists." );
		else                  mFrame->SetStatusText( "Cannot Find: End of Line Reached." );
		return wxNOT_FOUND;
	}

	// finally go there
	stc->SetSelection( atPos, atPos + text.size() );
	stc->ScrollToLine( stc->GetCurrentLine() );
	
	SETBUF( "Word found at pos: %d, line: %d", atPos, stc->GetCurrentLine() + 1 );
	mFrame->SetStatusText( buf );

	LOG_DEBUG_FORMAT( LV_TRACE, "Find word: %.2f (ms)", tm.toc() );
	return atPos;
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

bool FindFrame::IsMatchWhole()
{
	auto sel = mNotebook->GetSelection();
	if ( sel == wxNOT_FOUND ) return 0;
	if ( sel == FIND_PAGE ) return mFindAttrib->BoxMatchWhole->GetValue();
	return mReplaceAttrib->BoxMatchWhole->GetValue();
}

bool FindFrame::IsMatchCase()
{
	auto sel = mNotebook->GetSelection();
	if ( sel == wxNOT_FOUND ) return 0;
	if ( sel == FIND_PAGE ) return mFindAttrib->BoxMatchCase->GetValue();
	return mReplaceAttrib->BoxMatchCase->GetValue();
}

bool FindFrame::IsWrapAround()
{
	auto sel = mNotebook->GetSelection();
	if ( sel == wxNOT_FOUND ) return 0;
	if ( sel == FIND_PAGE ) return mFindAttrib->BoxWrapAround->GetValue();
	return mReplaceAttrib->BoxWrapAround->GetValue();
}

int FindFrame::GetFlags()
{
	auto sel = mNotebook->GetSelection();
	if ( sel == wxNOT_FOUND ) return 0;

	sPageAttrib* where = nullptr;
	if ( sel == FIND_PAGE ) where = mFindAttrib;
	else where = mReplaceAttrib;

	int flags = 0;
	if ( BOXMW( where )->GetValue() ) flags |= wxSTC_FIND_WHOLEWORD;
	if ( BOXMC( where )->GetValue() ) flags |= wxSTC_FIND_MATCHCASE;
	return flags;
}