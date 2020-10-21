#pragma once
#include <wx/wxprec.h>
#include <wx/notebook.h>
#include <wx/stc/stc.h>

//can manipulate MainFrame automatically
class Filehandle
{
public:
	void InitFilehandle( wxWindow* parent, wxNotebook* tab, std::vector<wxStyledTextCtrl*>* vTextField );

	void OnRenameFile();
	void OnOpenFile();

	void OnSaveFile();
	void OnSaveFileAll();
	void OnSaveFileAs();

private:
	void AddNewTab( const std::string& name );

	wxWindow* mParent;
	wxNotebook* mTab;
	std::vector<wxStyledTextCtrl*>* vTextField;

	bool Init = false;
	std::string SupportedFormat;
};