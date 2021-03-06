#pragma once
#include "Frame/DictionaryFrame.h"
#include "Frame/StyleFrame.h"
#include "Frame/PreferencesFrame.h"

struct sPageData
{
	sPageData( bool change, bool temp, std::string&& fn, std::string&& fp )
		: isChanged( change ), isTemporary( temp ), FileName( std::move( fn ) ), FilePath( std::move( fp ) ) {}
	sPageData( bool change, bool temp, std::string const& fn, std::string const& fp )
		: isChanged( change ), isTemporary( temp ), FileName( fn ), FilePath( fp ) {}

	bool isChanged;
	bool isTemporary;
	std::string FileName;
	std::string FilePath;
	wxStyledTextCtrl* TextField = nullptr;
	wxString Suggestion = "";
};

//can manipulate MainFrame automatically
class TextField
{
public:
	static void Init( wxFrame* parent );
	static void Destroy();

	static void FetchTempFile();
	static void SaveTempAll();
	static bool ExistTempFile();
	static bool ExistChangedFile();
	static bool SaveToExit();
	static int GetActivePage();
	static wxStyledTextCtrl* GetSTC( int page );

	// feature
	static bool LoadFile( const std::string& filepath );
	static void OnDropFiles( wxDropFilesEvent& event );

	// internal changes
	static void OnUpdateUI( wxStyledTextEvent& event );
	static void OnTextChanged( wxStyledTextEvent& event );
	static void OnSTCZoom( wxStyledTextEvent& event );

	// notebook changes
	static void OnPageCloseButton( wxAuiNotebookEvent& evt );
	static void OnPageChanged( wxAuiNotebookEvent& evt );
	static void OnPageDrag( wxAuiNotebookEvent& evt );

	// windows creation
	static void OnTextSummary( wxCommandEvent& event );
	static void OnCompSummary( wxCommandEvent& event );

	// command changes
	static void OnEOL_CR( wxCommandEvent& event );
	static void OnEOL_LF( wxCommandEvent& event );
	static void OnEOL_CRLF( wxCommandEvent& event );

	static void OnUpperCase( wxCommandEvent& event );
	static void OnLowerCase( wxCommandEvent& event );
	static void OnInverseCase( wxCommandEvent& event );
	static void OnRandomCase( wxCommandEvent& event );

	static void OnNewFile( wxCommandEvent& event );
	static void OnRenameFile( wxCommandEvent& event );
	static void OnOpenFile( wxCommandEvent& event );
	static void OnPageClose( wxCommandEvent& event );
	static void OnPageCloseAll( wxCommandEvent& event );
	static void OnSaveFile( wxCommandEvent& event );
	static void OnSaveFileAll( wxCommandEvent& event );
	static void OnSaveFileAs( wxCommandEvent& event );
	static void OnEmbedDict( wxCommandEvent& event );

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

	static void OnFind( wxCommandEvent& event );
	static void OnReplace( wxCommandEvent& event );
	static void OnGoto( wxCommandEvent& event );
	static void OnFindNext( wxCommandEvent& event );
	static void OnFindPrev( wxCommandEvent& event );
	static void OnOpenMenuSearch( wxMenuEvent& event );

private:
	static void UpdateMenuWindow();
	static void OnSelectMenuWindow( wxCommandEvent& event );
	static void OnOpenMenuWindow( wxMenuEvent& event );

	static void ShowAutoComp();
	static void OnAutoCompCompleted( wxStyledTextEvent& event );

	static bool UpdateEOLString( wxStyledTextCtrl* stc, int mode );

	static void UpdateStatusEOL( wxStyledTextCtrl* stc );
	static void UpdateStatusEncoding( wxStyledTextCtrl* stc );
	static void UpdateStatusPos( wxStyledTextCtrl* stc );

	static void FindText( wxStyledTextCtrl* stc, bool next );
	static void MarginAutoAdjust();
	static void CreateTempFile( const std::string& path );
	static bool AlreadyOpened( const std::string& filepath, bool focus );

	static void AddNewTab( sPageData& pd );
	static void LoadStyle( wxStyledTextCtrl* stc );
	static void UpdateParentName();
	static void UpdateSaveIndicator( bool save, int index = -1 );
	
	static bool isGotoInit;
	static bool isFindInit;
	static bool isDictInit;

	static std::mutex mMutex;
	static std::future<void> mFuture;
	static wxMenu* mMenuWnd;
	static wxFrame* mParent;
	static wxAuiNotebook* mNotebook;
	static std::vector<sPageData> mPageData;
	static std::vector<wxMenuItem*> mWindowItem;

	friend class PreferencesFrame;
	friend class DictionaryFrame;
	friend class AutoThread;
	friend class StyleFrame;
};