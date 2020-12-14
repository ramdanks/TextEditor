#pragma once

struct sPageAttrib
{
	wxPanel* Panel;
	wxCheckBox* BoxMatchWhole;
	wxCheckBox* BoxMatchCase;
	wxCheckBox* BoxWrapAround;
	wxTextCtrl* EntryFind;
};

class FindFrame
{
	static char buf[256];
public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus( bool find = true );

private:
	static inline wxString GetFindEntry();
	static void OnFindNext( wxCommandEvent& event );
	static void OnFindPrev( wxCommandEvent& event );
	static void OnCount( wxCommandEvent& event );
	static void OnReplace( wxCommandEvent& event );
	static void OnReplaceAll( wxCommandEvent& event );
	
	static int IFind( bool reverse, const wxString& text );

	static void CreateFindPage();
	static void CreateReplacePage();
	static void OnPageChanged( wxBookCtrlEvent& event );
	static void OnCloseWindow( wxCloseEvent& event );
	static void OnClose( wxCommandEvent& event );

	static bool IsMatchWhole();
	static bool IsMatchCase();
	static bool IsWrapAround();
	static int GetFlags();

	static wxFrame* mFrame;

	static wxNotebook* mNotebook;
	static wxTextCtrl* mEntryReplace;

	static sPageAttrib* mFindAttrib;
	static sPageAttrib* mReplaceAttrib;
};

enum FindEvent
{
	FIND_CLOSE,
	FIND_NEXT,
	FIND_PREV,
	FIND_COUNT,
	FIND_REPLACE,
	FIND_REPLACEALL
};