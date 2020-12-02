#pragma once
#include <wx/wxprec.h>
#include <wx/aui/auibook.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/stc/stc.h>

class StyleFrame : public wxFrame
{
	struct sFontPage
	{
		wxPanel* Panel;
		wxListBox* NameLB;
		wxListBox* StyleLB;
		wxTextCtrl* NameTC;
		wxTextCtrl* StyleTC;
		wxComboBox* SizeCB;
		wxStaticBox* SampleSB;
	};

	struct sStylePage
	{
		wxPanel* Panel;
		wxColourPickerCtrl* TextBack;
		wxColourPickerCtrl* TextFore;
		wxColourPickerCtrl* Caret;
		wxColourPickerCtrl* LineBack;
		wxColourPickerCtrl* Selection;
		wxColourPickerCtrl* LinenumBack;
		wxColourPickerCtrl* LinenumFore;
		wxStyledTextCtrl* Preview;
		wxComboBox* Theme;
	};

	struct sButton
	{
		wxButton* OK;
		wxButton* Cancel;
	};

public:
	StyleFrame( wxWindow* parent );
	void ShowAndFocus( bool show, bool focus );

private:
	void OnOK( wxCommandEvent& event );
	void OnCancel( wxCommandEvent& event );
	void OnClose( wxCloseEvent& event );
	void OnColourPickerChanged( wxColourPickerEvent& event );
	
	void UpdatePreview();
	void AdjustColourPicker();
	void CreateContent();
	wxDECLARE_EVENT_TABLE();

	wxWindow* mParent;
	wxFrame* mFrame;
	wxAuiNotebook* mNotebook;
	sFontPage mFP;
	sStylePage mSP;
	sButton mButton;
};