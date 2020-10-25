#pragma once
#include <wx/wxprec.h>

class StyleFrame : public wxFrame
{
public:
	StyleFrame( wxWindow* parent );
	void ShowAndFocus( bool show, bool focus );

private:
	void OnClose( wxCloseEvent& event );
	wxDECLARE_EVENT_TABLE();

	wxWindow* mParent;
	wxFrame* mFrame;
};