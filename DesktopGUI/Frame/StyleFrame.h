#pragma once

class StyleFrame : public wxFrame
{

	struct sStylePage
	{
		wxPanel* Panel;
		wxFontPickerCtrl* FontPicker;
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
	void OnFontPickerChanged( wxFontPickerEvent& event );

	void UpdatePreview();
	void AdjustColourPicker();
	void CreateContent();
	wxDECLARE_EVENT_TABLE();

	wxWindow* mParent;
	wxFrame* mFrame;
	sStylePage mSP;
	sButton mButton;
};