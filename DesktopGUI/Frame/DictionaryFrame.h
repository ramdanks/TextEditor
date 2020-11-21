#pragma once
#include <wx/wxprec.h>
#include "../Feature/Dictionary.h"
#include <wx/stc/stc.h>
#include <unordered_map>

struct DictionaryData
{
	Dictionary Dict;
	wxString Filepath;
	wxColour TextColour;
	int Style = 0;
};

typedef std::unordered_map<wxString, std::vector<DictionaryData>> PagesDictionaryList;

class DictionaryFrame
{
public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus();
	static void UpdateComboBox( const wxArrayString& list, const wxString& textPath );
	
	static bool ExistList( const wxString& filepath );
	static void AddNewDict( const wxString& textPath );

	static void ClearList( const wxString& textPath );
	static void EraseList( const wxString& textPath );
	static void EraseDict( const wxString& textPath, const wxString& dictPath );

	static void OnRefreshDict( wxCommandEvent& event );
	static void ResetStyling( const wxString& textPath );
	static bool StartStyling( const wxString& textPath );

private:
	static bool AlreadyLoaded( const wxString& textPath, const wxString& dictPath );
	static bool LoadFromFile( const wxString& textPath, const wxString& dictPath );
	static void OnDropFiles( wxDropFilesEvent& event );

	static void UpdatePreview( const wxString& textPath, const wxColour& clr );
	static void OnUpdateCombo( wxCommandEvent& event );
	static void OnUpdateListbox( wxCommandEvent& event );
	static void UpdateList( const wxString& textPath );

	static void OnOpenFile( wxCommandEvent& event );
	static void OnRemoveSelected( wxCommandEvent& event );
	static void OnRemoveAll( wxCommandEvent& event );
	static void OnSetColour( wxCommandEvent& event );
	static void OnSummary( wxCommandEvent& event );

	static void OnCloseWindow( wxCloseEvent& event );
	static void OnClose( wxCommandEvent& event );
	static void OnOK( wxCommandEvent& event );

	static int GetFlags();
	static wxStyledTextCtrl* RequestSTC( int page );
	static wxStyledTextCtrl* RequestSTC( const wxString& textPath );
	static DictionaryData* RequestDictionary( const wxString& textPath, const wxString& dictPath );
	static std::vector<DictionaryData>* RequestDictionaryList( const wxString& textPath );

	static wxWindow* mParent;
	static wxFrame* mFrame;
	static wxPanel* mPanel;
	static wxComboBox* mComboBox;
	static wxListBox* mListBox;

	static wxStyledTextCtrl* mDefaultSTC;
	static wxStyledTextCtrl* mDictionarySTC;

	static PagesDictionaryList mData;
};