#pragma once

struct sGotoInfo
{
	uint32_t mMaxLine;
	uint32_t mCurrentLine;
	uint32_t mMaxPos;
	uint32_t mCurrentPos;
};

class GotoFrame
{
public:
	static void Init( wxWindow* parent );
	static void UpdateInfo( wxStyledTextCtrl* tf );
	static void ShowAndFocus();
	static void RadioLinePressed( wxCommandEvent& event );
	static void RadioPosPressed( wxCommandEvent& event );

private:
	static void OnCloseWindow( wxCloseEvent& event );
	static void OnClose( wxCommandEvent& event );
	static void OnGo( wxCommandEvent& event );

	static wxWindow* mParent;
	static wxFrame* mFrame;
	static wxTextCtrl* mEntryGoLine;
		   
	static wxStaticText* mMaxSpecifier;
	static wxStaticText* mCurrentSpecifier;
		   
	static wxRadioButton* mButtonLine;
	static wxRadioButton* mButtonPos;
	static wxStyledTextCtrl* mTextField;
		   
	static sGotoInfo mInfo;
};