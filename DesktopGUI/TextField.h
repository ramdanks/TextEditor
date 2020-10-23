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
	static void SaveTempAll();
	static bool ExistAbsoluteFile();

	static void OnNewFile();
	static void OnRenameFile();
	static void OnOpenFile();
	static void OnPageClose();
	static void OnPageCloseAll();
	static void OnSaveFile();
	static void OnSaveFileAll();
	static bool OnSaveFileAs();

	static void OnZoom( bool zoomIn, bool reset );

private:
	static void AddNewTab( const std::string& name );
	static void ClearPage( size_t page, const std::string& name = std::string() );

	static wxWindow* mParent;
	static wxAuiNotebook* mNotebook;

	static std::vector<wxStyledTextCtrl*> mTextField;
	static std::vector<std::string> mPathAbsolute;
	static std::vector<std::string> mPathTemporary;

	static std::string SupportedFormat;
};