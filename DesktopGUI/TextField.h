#pragma once
#include <wx/wxprec.h>
#include <wx/aui/auibook.h>
#include <wx/stc/stc.h>

//can manipulate MainFrame automatically
class TextField
{
public:
	static void InitFilehandle( wxWindow* parent );
	static void FetchTempFile();
	static void CreateTempFile( const std::string& name );

	static void OnNewFile();
	static void OnRenameFile();
	static void OnOpenFile();
	static void OnTabClose();
	static void OnTabClose( wxAuiNotebookEvent& evt );
	static void OnTabCloseAll();
	static void OnClose();

	static void OnSaveFile();
	static void OnSaveFileAll();
	static bool OnSaveFileAs();

private:
	static void AddNewTab( const std::string& name );

	static wxWindow* mParent;
	static wxAuiNotebook* mTab;

	static std::vector<wxStyledTextCtrl*> mTextField;
	static std::vector<std::string> mPathAbsolute;
	static std::vector<std::string> mPathTemporary;

	static bool Init;
	static std::string SupportedFormat;
};