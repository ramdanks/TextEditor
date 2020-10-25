#pragma once
#include <wx/wxprec.h>
#include <wx/combobox.h>

class PreferencesFrame : public wxFrame
{
public:
	PreferencesFrame( wxWindow* parent );
	void ShowAndFocus( bool show, bool focus );

private:
	void OnClose( wxCloseEvent& event );
	wxDECLARE_EVENT_TABLE();

	wxComboBox* mBoxLanguage;
	wxWindow* mParent;
	wxFrame* mFrame;
};