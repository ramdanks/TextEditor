#pragma once
#include <wx/wxprec.h>
#include <wx/aui/auibook.h>
#include <wx/stc/stc.h>

struct sPageData
{
	bool isTemporary;
	std::string FilePath;
	wxStyledTextCtrl* TextField;
};

//can manipulate MainFrame automatically
class TextField
{
public:
	static void Init( wxWindow* parent );
	static void FetchTempFile();
	static void SaveTempAll();
	static bool ExistAbsoluteFile();

	static void OnNewFile( wxCommandEvent& event );
	static void OnRenameFile( wxCommandEvent& event );
	static void OnOpenFile( wxCommandEvent& event );
	static void OnPageClose( wxCommandEvent& event );
	static void OnPageCloseAll( wxCommandEvent& event );
	static void OnPageCloseButton( wxAuiNotebookEvent& evt );
	static void OnPageChanged( wxAuiNotebookEvent& evt );
	static void OnPageDrag( wxAuiNotebookEvent& evt );
	static void OnSaveFile( wxCommandEvent& event );
	static void OnSaveFileAll( wxCommandEvent& event );
	static void OnSaveFileAs( wxCommandEvent& event );

	static void OnUndo( wxCommandEvent& event );
	static void OnRedo( wxCommandEvent& event );
	static void OnCut( wxCommandEvent& event );
	static void OnCopy( wxCommandEvent& event );
	static void OnPaste( wxCommandEvent& event );
	static void OnDelete( wxCommandEvent& event );
	static void OnSelectAll( wxCommandEvent& event );
	static void OnZoomIn( wxCommandEvent& event );
	static void OnZoomOut( wxCommandEvent& event );
	static void OnZoomRestore( wxCommandEvent& event );
	static void OnSTCZoom( wxStyledTextEvent& event );

private:
	static void InitFilehandle();
	static void CreateTempFile( const std::string& name );
	static void AddNewTab( sPageData& pd );
	static void LoadStyle( wxStyledTextCtrl* stc );
	static void FormatSave( const std::string& sData, const std::string& filepath );
	static void UpdateParentName();
	static std::vector<uint8_t> FormatOpen( const std::string& filepath );

	static wxWindow* mParent;
	static wxAuiNotebook* mNotebook;
	static std::vector<sPageData> mPageData;

	static std::string SupportedFormat;
};