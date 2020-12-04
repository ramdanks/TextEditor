#pragma once
#include "AppFrame.h"

class PreferencesFrame
{
	struct GeneralPage
	{
		wxPanel* Panel;
		wxComboBox* LocalizationCB;
	};

	struct DictionaryPage
	{
		wxPanel* Panel;
	};

	struct Autosave
	{
		wxPanel* Panel;
	};

	struct MainFrame
	{
		wxStatusBar* Statbar;
		wxMenu* File;
		wxMenu* Edit;
		wxMenu* View;
		wxMenu* Case;
		wxMenu* EOL;
		wxMenu* Search;
		wxMenu* Setting;
		wxMenu* Help;
		wxMenuBar* MenuBar;
	};

public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus( bool show, bool focus );

private:
	static void CreateContent();
	static void RefreshMessage();
	static void OnLocalization( wxCommandEvent& event );
	static void OnClose( wxCloseEvent& event );

	static wxFrame* mFrame;
	static DictionaryPage mDP;
	static GeneralPage mGP;
	static wxAuiNotebook* mNotebook;
	static MainFrame mMFmenu;

	friend class AppFrame;
};