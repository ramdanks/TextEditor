#pragma once
#include <wx/wxprec.h>

class DictionaryFrame
{
public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus();
	static void UpdateComboBox( const wxArrayString& list, int selection );

private:
	static void OnCloseWindow( wxCloseEvent& event );
	static void OnClose( wxCommandEvent& event );
	static void OnOK( wxCommandEvent& event );

	static wxWindow* mParent;
	static wxFrame* mFrame;
	static wxComboBox* mComboBox;
	static wxPanel* mPanel;
};

