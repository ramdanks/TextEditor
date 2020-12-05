#pragma once
#include "AppFrame.h"

class PreferencesFrame
{
	struct GeneralPage
	{
		wxPanel* Panel;
		wxComboBox* LocalizationCB;
		wxCheckBox* CB_SB;
		wxCheckBox* CB_DD;
		wxCheckBox* CB_SS;
		wxCheckBox* CB_NB_Hide;
		wxRadioButton* RB_NB_Top;
		wxRadioButton* RB_NB_Bot;
		wxCheckBox* CB_NB_Lock;
		wxCheckBox* CB_NB_Fixed;
		wxCheckBox* CB_NB_Middle;
		wxCheckBox* CB_NB_ShowClose;
		wxRadioButton* RB_NB_OnAll;
		wxRadioButton* RB_NB_OnAct;
	};

	struct DictionaryPage
	{
		wxPanel* Panel;
		wxCheckBox* CB_AC;
		wxCheckBox* CB_AH;
		wxSpinCtrl* SpinAutocomp;
		wxSpinCtrl* SpinAutohigh;
		
	};

	struct TempPage
	{
		wxPanel* Panel;
		wxCheckBox* CB_AS;
		wxSpinCtrl* SpinAutosave;
	};

	struct MainFrame
	{
		wxFrame* Frame;
		wxMenu* File;
		wxMenu* Edit;
		wxMenu* View;
		wxMenu* Case;
		wxMenu* EOL;
		wxMenu* Search;
		wxMenu* Setting;
		wxMenu* Help;
		wxMenuBar* MenuBar;
		wxStatusBar* Statbar;
	};

public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus( bool show, bool focus );
	static void Update();

private:
	static void CreateContent();
	static void RefreshMessage();

	static void UpdateNotebook();
	static void UpdateDragDrop();
	static void UpdateAutocomp();
	static void UpdateAutohigh();
	static void UpdateAutosave();
	static void UpdateStatbar();

	static void OnCheckHide( wxCommandEvent& event );
	static void OnCheckShowClose( wxCommandEvent& event );

	static void OnOK( wxCommandEvent& event );
	static void OnCancel( wxCommandEvent& event );

	static void OnLocalization( wxCommandEvent& event );
	static void OnClose( wxCloseEvent& event );
	static void OnScroll( wxScrollEvent& event );

	static wxFrame* mFrame;
	static DictionaryPage mDP;
	static GeneralPage mGP;
	static TempPage mTP;
	static wxAuiNotebook* mNotebook;
	static MainFrame mMF;

	friend class AppFrame;
};

#define WND_ID_SAUTOHIGH 1
#define WND_ID_SAUTOCOMP 2
#define WND_ID_SAUTOSAVE 3