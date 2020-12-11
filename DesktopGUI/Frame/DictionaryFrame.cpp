#include "DictionaryFrame.h"
#include "../Feature/FilehandleGUI.h"
#include "../Feature/Language.h"
#include "../Feature/LogGUI.h"
#include "../Feature/Config.h"
#include "../TextField.h"

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
	mFrame = new wxFrame( parent, wxID_ANY, "Dictionary", wxDefaultPosition, wxSize( 400, 410 ),
						  wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE );
	mFrame->SetIcon( wxICON( ICON_APP ) );
	mFrame->Bind( wxEVT_CLOSE_WINDOW, &OnCloseWindow );
	mPanel = new wxPanel( mFrame, -1 );

	CreateContent();
	Update();
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
	if ( !stc ) return;

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

void DictionaryFrame::ChangeListKey( const wxString& before, const wxString& after )
{
	auto it = mData.find( before );
	if ( it == mData.end() ) return;

	auto temp = std::move( it->second );
	mData.erase( it );
	mData[after] = std::move( temp );
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

void DictionaryFrame::EraseDict( const wxString& textPath, int index )
{
	auto list = RequestDictionaryList( textPath );
	if ( !list || index >= list->size() ) return;
	list->erase( list->begin() + index );
}

bool DictionaryFrame::AlreadyLoaded( const wxString& textPath, const wxString& dictPath )
{
	auto list = RequestDictionaryList( textPath );
	if ( !list ) return false;

	for ( const auto& dict : *list )
		if ( dict.Filepath == dictPath ) return true;
	return false;
}

void DictionaryFrame::LoadFromDir( const wxString& textPath, const wxString& dir )
{
	auto files = FilehandleGUI::GetDroppedFiles( &dir, 1 );
	THROW_ERR_IFEMPTY( files, "Dropped Files is Empty on Dictionary!" );

	uint32_t dropped = files.size();
	for ( int i = 0; i < dropped; i++ )
	{
		if      ( AlreadyLoaded( textPath, files[i] ) ) LOG_DEBUG( LV_INFO, "Dictionary already loaded" );
		else if ( !LoadFromFile( textPath, files[i] ) ) LOG_DEBUG( LV_WARN, "Rejecting Dictionary File" );
	}
}

bool DictionaryFrame::LoadFromFile( const wxString& textPath, const wxString& dictPath )
{
	Dictionary dict;
	auto list = RequestDictionaryList( textPath );
	if ( !list ) AddNewDict( textPath );

	if ( dict.Load( std::string( dictPath ) ) )
	{
		char* p;
		unsigned long colRGB = strtol( dict.GetColour().c_str() + 1, &p, 16 );
		wxColour clr( colRGB );
		mData[textPath].push_back( { dict, dictPath, clr } );
		if ( mListBox ) mListBox->AppendString( dict.GetTitle() );
		return true;
	}
	return false;
}

void DictionaryFrame::OnDropFiles( wxDropFilesEvent& event )
{
	TIMER_ONLY( timer, ADJUST, false );

	uint32_t dropped = 0;
	if ( event.GetNumberOfFiles() < 1 ) return;

	wxBusyCursor busyCursor;
	wxWindowDisabler disabler;
	wxBusyInfo busyInfo( "Adding files, wait please..." );

	try
	{	
		auto files = FilehandleGUI::GetDroppedFiles( event.GetFiles(), event.GetNumberOfFiles() );
		THROW_ERR_IFEMPTY( files, "Dropped Files is Empty on Dictionary!" );

		auto textPath = mComboBox->GetStringSelection();
		THROW_ERR_IFEMPTY( textPath, "Invalid Selection on Dictionary!" );

		for ( int i = 0; i < files.size(); i++ )
			LoadFromFile( textPath, files[i] );
	}
	catch ( Util::Err& e )
	{	
		LOG_ALL( LV_ERROR, e.Seek() );
		return;
	}
	LOG_ALL_FORMAT( LV_TRACE, "Drag n Drop Dictionary: %d, Time: %.2f (ms)", dropped, timer.toc() );
}

void DictionaryFrame::CreateContent()
{
	auto hboxOption = new wxBoxSizer( wxHORIZONTAL );
	auto hboxPrompt = new wxBoxSizer( wxHORIZONTAL );
	auto hboxDict = new wxBoxSizer( wxHORIZONTAL );
	auto vboxInfo = new wxBoxSizer( wxVERTICAL );
	auto vbox = new wxBoxSizer( wxVERTICAL );

	auto text = new wxStaticText( mPanel, -1, "Selected Pages:" );
	mComboBox = new wxComboBox( mPanel, -1 );
	mComboBox->SetEditable( false );
	mComboBox->Bind( wxEVT_COMBOBOX, OnUpdateCombo );

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

	MatchCase = new wxCheckBox( mPanel, -1, "Match Case" );
	MatchWhole = new wxCheckBox( mPanel, -1, "Match Whole Word" );
	UniformClr = new wxCheckBox( mPanel, -1, "Uniform Colour" );
	hboxOption->Add( MatchCase, 0 );
	hboxOption->Add( MatchWhole, 0 );
	hboxOption->Add( UniformClr, 0 );

	mListBox = new wxListBox( mPanel, -1 );
	auto font = mListBox->GetFont();
	font.Scale( 1.2 );
	mListBox->SetFont( font );
	mListBox->DragAcceptFiles( true );
	mListBox->Bind( wxEVT_DROP_FILES, OnDropFiles );
	mListBox->Bind( wxEVT_LISTBOX, OnUpdateListbox );
	hboxDict->Add( mListBox, 1, wxEXPAND | wxRIGHT, 10 );
	hboxDict->Add( vboxInfo, 0, wxEXPAND );

	vbox->Add( text, 0, wxLEFT | wxTOP, 10 );
	vbox->Add( mComboBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxDict, 1, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxOption, 0, wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );
	vbox->Add( hboxPrompt, 0, wxALIGN_RIGHT | wxRIGHT, 10 );
	vbox->Add( -1, 15 );

	mPanel->SetSizer( vbox );
	buttonOK->Bind( wxEVT_BUTTON, OnOK );
	buttonCC->Bind( wxEVT_BUTTON, OnClose );
}

void DictionaryFrame::Update()
{
	MatchCase->SetValue( Config::mDictionary.MatchCase );
	MatchWhole->SetValue( Config::mDictionary.MatchWhole );
	UniformClr->SetValue( Config::mDictionary.UniformClr );
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
	auto sel = mListBox->GetSelection();
	auto dict = RequestDictionary( textPath, sel );
	if ( dict == nullptr ) return;

	UpdatePreview( textPath, dict->TextColour );
}

void DictionaryFrame::UpdateList( const wxString& textPath )
{
	mListBox->Clear();
	auto list = RequestDictionaryList( textPath );
	if ( !list ) return;

	wxArrayString strList;
	for ( const auto& dict : *list )
		strList.Add( dict.Dict.GetTitle() );
	mListBox->Append( strList );
}

void DictionaryFrame::OnOpenFile( wxCommandEvent& event )
{
	auto opened = FilehandleGUI::OpenDialog( mParent, "" );
	auto textPath = mComboBox->GetStringSelection();
	if ( AlreadyLoaded( textPath, opened ) )
	{
		LOG_DEBUG( LV_INFO, "Dictionary already loaded: " + opened );
		return;
	}
	LoadFromFile( textPath, opened );
}

void DictionaryFrame::OnRemoveSelected( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	auto sel = mListBox->GetSelection();

	mListBox->Delete( sel );
	EraseDict( textPath, sel );

	ResetStyling( textPath );
	StartStyling( textPath, Config::GetNotebookStyle() );

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

	auto stc = RequestSTC( textPath );
	if ( stc == nullptr ) return;
	UpdatePreview( textPath, FOREGROUND_DEF_CLR( stc ) );
}

void DictionaryFrame::OnSetColour( wxCommandEvent& event )
{
	auto textPath = mComboBox->GetStringSelection();
	auto sel = mListBox->GetSelection();
	auto dict = RequestDictionary( textPath, sel );
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
	auto textPath = mComboBox->GetStringSelection();
	auto sel = mListBox->GetSelection();
	auto dict = RequestDictionary( textPath, sel );
	if ( !dict ) return;

	wxString message;
	message += "Title: "             + dict->Dict.GetTitle();
	message += "\nDefault Colour: "  + dict->Dict.GetColour();
	message += "\nActive Colour: "   + dict->TextColour.GetAsString( wxC2S_HTML_SYNTAX );
	message += "\nDictionary Size: " + TO_STR( dict->Dict.Size() );
	message += "\nFilepath: "        + dict->Filepath;

	auto SumDialog = wxMessageDialog( mParent, message, "Dictionary Summary", wxOK | wxSTAY_ON_TOP );
	SumDialog.ShowModal();
}

void DictionaryFrame::OnCloseWindow( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void DictionaryFrame::OnClose( wxCommandEvent& event )
{
	Update();
	mFrame->Show( false );
}

void DictionaryFrame::OnOK( wxCommandEvent& event )
{
	PROFILE_FUNC();

	Config::mDictionary.MatchCase = MatchCase->GetValue();
	Config::mDictionary.MatchWhole = MatchWhole->GetValue();
	Config::mDictionary.UniformClr = UniformClr->GetValue();

	for ( auto const& pages : mData )
	{
		CreateSuggestion( pages.first );
		ResetStyling( pages.first );
		StartStyling( pages.first, Config::GetDictionaryFlags() );
	}

	mFrame->Show( false );
}

void DictionaryFrame::OnRefreshDict( wxCommandEvent& event )
{
	auto page = TextField::GetActivePage();
	auto textPath = TextField::mPageData[page].FilePath;
	ResetStyling( textPath );
	StartStyling( textPath, Config::GetDictionaryFlags() );
}

void DictionaryFrame::ResetStyling( const wxString& textPath )
{
	PROFILE_FUNC();

	auto stc = RequestSTC( textPath );
	auto list = RequestDictionaryList( textPath );
	if ( !list || !stc ) return;

	int style = 40;
	for ( auto& dict : *list )
	{
		if ( Config::mDictionary.UniformClr ) dict.Style = style;
		else dict.Style = style++;
		stc->StyleSetForeground( dict.Style, dict.TextColour );
	}
	
	stc->StartStyling( 0 );
	stc->SetStyling( stc->GetTextLength(), wxSTC_STYLE_DEFAULT );
}

bool DictionaryFrame::StartStyling( const wxString& textPath, int flags )
{
	PROFILE_FUNC();

	auto stc = RequestSTC( textPath );
	auto list = RequestDictionaryList( textPath );
	if ( !list ) return false;

	for ( auto const& dict : *list )
		for ( auto const& word : dict.Dict.GetContent() )
			for ( int get = -1; ; )
			{
				get = stc->FindText( get + 1, stc->GetTextLength(), word.first, flags );
				if ( get == -1 ) break;

				stc->StartStyling( get );
				stc->SetStyling( word.first.size(), dict.Style );
			}
	return true;
}

void DictionaryFrame::CreateSuggestion( const wxString& textPath )
{
	PROFILE_FUNC();

	auto list = RequestDictionaryList( textPath );
	auto autocomplist = RequestSuggestion( textPath );
	autocomplist->clear();

	std::vector<std::string> words;
	for ( auto const& dict : *list )
	{
		auto content = dict.Dict.GetContent();
		for ( auto& key : content )
		{
			if ( words.size() > Config::mAutocomp.Param ) break;
			words.emplace_back( key.first );
		}
	}

	// sort for auto completion
	std::sort( words.begin(), words.end(), []( std::string const& a, std::string const& b )
	{ return a < b; } );

	// insert autocompletion with seperator and already sorted
	for ( auto& word : words ) autocomplist->Append( word + "|" );
}

int DictionaryFrame::GetFlags()
{
	int flags = 0;
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

DictionaryData* DictionaryFrame::RequestDictionary( const wxString& textPath, int index )
{
	auto list = RequestDictionaryList( textPath );
	if ( list == nullptr || index >= list->size() ) return nullptr;
	return &(*list )[index];
}

std::vector<DictionaryData>* DictionaryFrame::RequestDictionaryList( const wxString& textPath )
{
	auto it = mData.find( textPath );
	if ( it == mData.end() ) return nullptr;
	return &it->second;
}

wxString* DictionaryFrame::RequestSuggestion( const wxString& textPath )
{
	for ( auto& page : TextField::mPageData )
		if ( page.FilePath == textPath ) return &page.Suggestion;
	return nullptr;
}
