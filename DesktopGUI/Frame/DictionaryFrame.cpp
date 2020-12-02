#include "DictionaryFrame.h"
#include "../Feature/FilehandleGUI.h"
#include "../Feature/Language.h"
#include "../Feature/LogGUI.h"
#include "../../Utilities/Timer.h"
#include "../../Utilities/Err.h"
#include <wx/busyinfo.h>
#include <wx/colordlg.h>
#include <wx/clrpicker.h>
#include "../TextField.h"
#include <mutex>
#include <algorithm>

wxWindow* DictionaryFrame::mParent;
wxFrame* DictionaryFrame::mFrame;
wxComboBox* DictionaryFrame::mComboBox;
wxPanel* DictionaryFrame::mPanel;
wxListBox* DictionaryFrame::mListBox;
wxStyledTextCtrl* DictionaryFrame::mDefaultSTC;
wxStyledTextCtrl* DictionaryFrame::mDictionarySTC;
PagesDictionaryList DictionaryFrame::mData;

wxCheckBox* MatchCase;
wxCheckBox* MatchWhole;
wxCheckBox* UniformClr;

#define FOREGROUND_DEF_CLR(stc) stc->StyleGetForeground( wxSTC_STYLE_DEFAULT )

void DictionaryFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, "Dictionary", wxDefaultPosition, wxSize( 600, 400 ),
						  wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE );
	mFrame->SetIcon( wxICON( ICON_APP ) );
	mFrame->Bind( wxEVT_CLOSE_WINDOW, &OnCloseWindow );

	mPanel = new wxPanel( mFrame, -1 );

	auto hboxSelect = new wxBoxSizer( wxHORIZONTAL );
	auto hboxOption = new wxBoxSizer( wxHORIZONTAL );
	auto hboxPrompt = new wxBoxSizer( wxHORIZONTAL );
	auto hboxDict = new wxBoxSizer( wxHORIZONTAL );
	auto vboxInfo = new wxBoxSizer( wxVERTICAL );
	auto vbox = new wxBoxSizer( wxVERTICAL );

	auto text = new wxStaticText( mPanel, -1, "Selected Pages" );
	mComboBox = new wxComboBox( mPanel, -1 );
	mComboBox->SetEditable( false );
	mComboBox->Bind( wxEVT_COMBOBOX, OnUpdateCombo );
	hboxSelect->Add( text, 0, wxRIGHT, 10 );
	hboxSelect->Add( mComboBox, 1 );

	mDefaultSTC = new wxStyledTextCtrl( mPanel, wxID_NEW, wxDefaultPosition, wxSize( 150, 30 ), wxSUNKEN_BORDER );
	mDictionarySTC = new wxStyledTextCtrl( mPanel, wxID_NEW, wxDefaultPosition, wxSize( 150, 30 ), wxSUNKEN_BORDER );
	mDefaultSTC->AppendText( "System Colouring..." );
	mDictionarySTC->AppendText( "Dictionary Colouring..." );
	mDefaultSTC->SetScrollWidth( 1 );
	mDictionarySTC->SetScrollWidth( 1 );
	mDefaultSTC->SetEditable( false );
	mDictionarySTC->SetEditable( false );

	auto buttonOF = new wxButton( mPanel, -1, "Open File", wxDefaultPosition, wxSize( 150, 30 ) );
	buttonOF->Bind( wxEVT_BUTTON, OnOpenFile );
	auto buttonRS = new wxButton( mPanel, -1, "Remove Selected", wxDefaultPosition, wxSize( 150, 30 ) );
	buttonRS->Bind( wxEVT_BUTTON, OnRemoveSelected );
	auto buttonRA = new wxButton( mPanel, -1, "Remove All", wxDefaultPosition, wxSize( 150, 30 ) );
	buttonRA->Bind( wxEVT_BUTTON, OnRemoveAll );
	auto buttonSC = new wxButton( mPanel, -1, "Set Colour", wxDefaultPosition, wxSize( 150, 30 ) );
	buttonSC->Bind( wxEVT_BUTTON, OnSetColour );
	auto buttonSU = new wxButton( mPanel, -1, "Summary", wxDefaultPosition, wxSize( 150, 30 ) );
	buttonSU->Bind( wxEVT_BUTTON, OnSummary );

	vboxInfo->Add( mDefaultSTC, 0 );
	vboxInfo->Add( mDictionarySTC, 0 );
	vboxInfo->Add( buttonOF, 0 );
	vboxInfo->Add( buttonRS, 0 );
	vboxInfo->Add( buttonRA, 0 );
	vboxInfo->Add( buttonSC, 0 );
	vboxInfo->Add( buttonSU, 0 );

	auto buttonOK = new wxButton( mPanel, -1, "OK" );
	buttonOK->Bind( wxEVT_BUTTON, OnOK );
	auto buttonCC = new wxButton( mPanel, -1, "Cancel" );
	buttonCC->Bind( wxEVT_BUTTON, OnClose );
	hboxPrompt->Add( buttonOK, 0 );
	hboxPrompt->Add( buttonCC, 0 );

	MatchCase  = new wxCheckBox( mPanel, -1, "Match Case" );
	MatchWhole = new wxCheckBox( mPanel, -1, "Match Whole Word" );
	UniformClr = new wxCheckBox( mPanel, -1, "Uniform Colour" );
	hboxOption->Add( MatchCase, 0 );
	hboxOption->Add( MatchWhole, 0 );
	hboxOption->Add( UniformClr, 0 );

	mListBox = new wxListBox( mPanel, -1 );
	mListBox->DragAcceptFiles( true );
	mListBox->Bind( wxEVT_DROP_FILES, OnDropFiles );
	mListBox->Bind( wxEVT_LISTBOX, OnUpdateListbox );
	hboxDict->Add( mListBox, 1, wxEXPAND | wxRIGHT, 10 );
	hboxDict->Add( vboxInfo, 0, wxEXPAND );

	vbox->Add( -1, 10 );
	vbox->Add( hboxSelect, 0, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxDict, 1, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxOption, 0, wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxPrompt, 0, wxALIGN_RIGHT | wxRIGHT, 10 );
	vbox->Add( -1, 10 );

	mPanel->SetSizer( vbox );
	buttonOK->Bind( wxEVT_BUTTON, OnOK );
	buttonCC->Bind( wxEVT_BUTTON, OnClose );
}

void DictionaryFrame::ShowAndFocus()
{
	if ( mFrame == nullptr ) return;

	mFrame->Show();
	mFrame->SetFocus();
}

void DictionaryFrame::UpdateComboBox( const wxArrayString& list, const wxString& textPath )
{
	mComboBox->Set( list );
	mComboBox->SetStringSelection( textPath );
	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;

	UpdatePreview( textPath, FOREGROUND_DEF_CLR(stc) );
	UpdateList( textPath );
}

bool DictionaryFrame::ExistList( const wxString& filepath )
{
	return mData.find( filepath ) != mData.end();
}

void DictionaryFrame::AddNewDict( const wxString& textPath )
{
	mData[textPath] = std::vector<DictionaryData>();
}

void DictionaryFrame::ClearList( const wxString& textPath )
{
	//this will retain an element in vector
	auto it = mData.find( textPath );
	if ( it == mData.end() ) return;
	it->second.clear();
}
 
void DictionaryFrame::EraseList( const wxString& textPath )
{
	//erase list will remove an element in vector
	auto it = mData.find( textPath );
	if ( it == mData.end() ) return;
	mData.erase( it );
}

void DictionaryFrame::EraseDict( const wxString& textPath, const wxString& dictPath )
{
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) return;
	for ( uint32_t i = 0; i < list->size(); i++ )
		if ( ( *list )[i].Filepath == dictPath )
			list->erase( list->begin() + i );
}

bool DictionaryFrame::AlreadyLoaded( const wxString& textPath, const wxString& dictPath )
{
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) return false;

	for ( const auto& dict : *list )
		if ( dict.Filepath == dictPath ) return true;
	return false;
}

bool DictionaryFrame::LoadFromFile( const wxString& textPath, const wxString& dictPath )
{
	Dictionary dict;
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) AddNewDict( textPath );

	if ( dict.Load( std::string( dictPath ) ) )
	{
		char* p;
		unsigned long colRGB = strtol( dict.GetColour().c_str() + 1, &p, 16 );
		wxColour clr( colRGB );
		mData[textPath].push_back( { dict, dictPath, clr } );
		mListBox->AppendString( dictPath );
		return true;
	}
	return false;
}

void DictionaryFrame::OnDropFiles( wxDropFilesEvent& event )
{
	Util::Timer tm( "Drop Files", MS, false );
	try
	{
		wxBusyCursor busyCursor;
		wxWindowDisabler disabler;
		wxBusyInfo busyInfo( "Adding files, wait please..." );

		if ( event.GetNumberOfFiles() < 1 ) return;
		auto files = FilehandleGUI::GetDroppedFiles( event.GetFiles(), event.GetNumberOfFiles() );
		THROW_ERR_IFEMPTY( files, "Dropped Files is Empty on Dictionary!" );

		auto textPath = mComboBox->GetStringSelection();
		THROW_ERR_IFEMPTY( textPath, "Invalid Selection on Dictionary!" );

		uint64_t readSizes = 0;
		for ( int i = 0; i < files.size(); i++ )
		{
			if ( AlreadyLoaded( textPath, files[i] ) )
			{
				LOG_DEBUG( LEVEL_INFO, "Dictionary already loaded: " + CV_STR( files[i] ) );
			}
			else
			{
				if ( !LoadFromFile( textPath, files[i] ) )
					LOG_DEBUG( LEVEL_WARN, "Rejecting Dictionary File: " + CV_STR( files[i] ) );
			}
		}
	}
	catch ( Util::Err& e )
	{
		LOG_ALL( LEVEL_ERROR, e.Seek() );
		return;
	}
	LOG_ALL_FORMAT( LEVEL_TRACE, "Drag n Drop Dictionary: %d, Time: %f (ms)", event.GetNumberOfFiles(), tm.Toc() );
}

void DictionaryFrame::UpdatePreview( const wxString& textPath, const wxColour& clr )
{
	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;

	auto background = stc->StyleGetBackground( wxSTC_STYLE_DEFAULT );
	auto foreground = stc->StyleGetForeground( wxSTC_STYLE_DEFAULT );

	auto lineBack = stc->StyleGetBackground( wxSTC_STYLE_LINENUMBER );
	auto lineFore = stc->StyleGetForeground( wxSTC_STYLE_LINENUMBER );

	stc = mDefaultSTC;
	for ( int i = 0; i < 2; i++ )
	{
		if ( i == 1 )
		{
			stc = mDictionarySTC;
			foreground = clr;
		}

		stc->StyleSetForeground( wxSTC_STYLE_DEFAULT, foreground ); //foreground such as text
		stc->StyleSetBackground( wxSTC_STYLE_DEFAULT, background ); //background for field

		stc->StyleClearAll();
		stc->SetCaretLineVisible( true ); //set caret line background visible
		stc->SetCaretLineBackground( wxColour( 70, 70, 70 ) ); //caret line background
		stc->SetCaretForeground( wxColour( 221, 151, 204 ) ); //caret colour

		stc->SetSelBackground( true, wxColour( 58, 119, 201 ) ); //background of selected text

		stc->StyleSetBackground( wxSTC_STYLE_LINENUMBER, lineBack ); //linenumber back color
		stc->StyleSetForeground( wxSTC_STYLE_LINENUMBER, lineFore ); //linenumber fore color
	}
}

void DictionaryFrame::OnUpdateCombo( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	UpdateList( textPath );

	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;
	UpdatePreview( textPath, FOREGROUND_DEF_CLR( stc ) );
}

void DictionaryFrame::OnUpdateListbox( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	auto dictPath = mListBox->GetStringSelection();
	auto dict = RequestDictionary( textPath, dictPath );
	if ( dict == nullptr ) return;

	UpdatePreview( textPath, dict->TextColour );
}

void DictionaryFrame::UpdateList( const wxString& textPath )
{
	mListBox->Clear();
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) return;

	wxArrayString strList;
	for ( const auto& dict : *list )
		strList.Add( wxString( dict.Filepath ) );
	mListBox->Append( strList );
}

void DictionaryFrame::OnOpenFile( wxCommandEvent& event )
{
	auto opened = FilehandleGUI::OpenDialog( mParent, "" );
	auto textPath = mComboBox->GetStringSelection();
	if ( AlreadyLoaded( textPath, opened ) )
	{
		LOG_DEBUG( LEVEL_INFO, "Dictionary already loaded: " + opened );
		return;
	}
	LoadFromFile( textPath, opened );
}

void DictionaryFrame::OnRemoveSelected( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	auto dicPath = mListBox->GetStringSelection();

	mListBox->Delete( mListBox->GetSelection() );
	EraseDict( textPath, dicPath );

	ResetStyling( textPath );
	StartStyling( textPath );

	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;
	UpdatePreview( textPath, FOREGROUND_DEF_CLR( stc ) );
}

void DictionaryFrame::OnRemoveAll( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	mListBox->Clear();
	ClearList( textPath );

	ResetStyling( textPath );
	StartStyling( textPath );

	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;
	UpdatePreview( textPath, FOREGROUND_DEF_CLR( stc ) );
}

void DictionaryFrame::OnSetColour( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	auto dictPath = mListBox->GetStringSelection();
	auto dict = RequestDictionary( textPath, dictPath );
	if ( dict == nullptr ) return;

	auto dlg = wxColourDialog( mFrame );
	if ( dlg.ShowModal() == wxID_OK )
	{
		auto colour = dlg.GetColourData().GetColour();
		dict->TextColour = colour;
		UpdatePreview( textPath, colour );
	}
}

void DictionaryFrame::OnSummary( wxCommandEvent& event )
{
	Util::Timer tm( "Dictionary Summary", ADJUST, false );

	auto textPath = mComboBox->GetStringSelection();
	auto dictPath = mListBox->GetStringSelection();
	auto dict = RequestDictionary( textPath, dictPath );
	if ( dict == nullptr ) return;

	wxString message;
	message += "Title: "             + dict->Dict.GetTitle();
	message += "\nDefault Colour: "  + dict->Dict.GetColour();
	message += "\nActive Colour: "   + dict->TextColour.GetAsString( wxC2S_HTML_SYNTAX );
	message += "\nDictionary Size: " + TO_STR( dict->Dict.Size() );
	message += "\nFilepath: "        + dict->Filepath;

	auto SumDialog = wxMessageDialog( mParent, message, "Dictionary Summary", wxOK | wxSTAY_ON_TOP );
	LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
	SumDialog.ShowModal();
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
	for ( const auto& pages : mData )
	{
		ResetStyling( pages.first );
		StartStyling( pages.first );
	}
	mFrame->Show( false );
}

void DictionaryFrame::OnRefreshDict( wxCommandEvent& event )
{
	auto page = TextField::GetActivePage();
	auto textPath = TextField::mPageData[page].FilePath;
	ResetStyling( textPath );
	StartStyling( textPath );
}

void DictionaryFrame::ResetStyling( const wxString& textPath )
{
	Util::Timer tm( "Reset Styling", ADJUST, false );

	auto stc = RequestSTC( textPath );
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr || stc == nullptr ) return;

	int style = 40;
	for ( auto& dict : *list )
	{
		if ( UniformClr->GetValue() ) dict.Style = style;
		else dict.Style = style++;
		stc->StyleSetForeground( dict.Style, dict.TextColour );
	}
	
	stc->StartStyling( 0 );
	stc->SetStyling( stc->GetTextLength(), wxSTC_STYLE_DEFAULT );

	LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
}

bool DictionaryFrame::StartStyling( const wxString& textPath )
{
	Util::Timer tm( "Apply Styling", ADJUST, false );

	auto stc = RequestSTC( textPath );
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) return false;

	for ( const auto& dict : *list )
	{
		for ( const auto& word : dict.Dict.GetContent() )
		{
			for ( int get = -1; ; )
			{
				get = stc->FindText( get + 1, stc->GetTextLength(), word.first, GetFlags() );
				if ( get == -1 ) break;

				stc->StartStyling( get );
				stc->SetStyling( word.first.size(), dict.Style );
			}
		}
	}
	LOG_ALL( LEVEL_TRACE, tm.Toc_String() );
	return true;
}

int DictionaryFrame::GetFlags()
{
	int flags = 0;
	if ( MatchWhole->GetValue() ) flags += wxSTC_FIND_WHOLEWORD;
	if ( MatchCase->GetValue() ) flags += wxSTC_FIND_MATCHCASE;
	return flags;
}

wxStyledTextCtrl* DictionaryFrame::RequestSTC( int page )
{
	if ( page < 0 || page >= TextField::mPageData.size() ) return nullptr;
	return TextField::mPageData[page].TextField;
}

wxStyledTextCtrl* DictionaryFrame::RequestSTC( const wxString& textPath )
{
	for ( const auto& pages : TextField::mPageData )
		if ( pages.FilePath == textPath ) return pages.TextField;
	return nullptr;
}

DictionaryData* DictionaryFrame::RequestDictionary( const wxString& textPath, const wxString& dictPath )
{
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr ) return nullptr;
	for ( auto& dict : *list )	
		if ( dict.Filepath == dictPath ) return &dict;
	return nullptr;
}

std::vector<DictionaryData>* DictionaryFrame::RequestDictionaryList( const wxString& textPath )
{
	auto it = mData.find( textPath );
	if ( it == mData.end() ) return nullptr;
	return &it->second;
}