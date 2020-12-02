#pragma once
#include "../Socket/MemoSocket.h"
#include "../Feature/AutoThread.h"
#include <wx/wxprec.h>
#include <thread>

struct sFileInfo
{
	sFileInfo( const wxString& name, const wxString& path, const uint64_t size )
		: Name( name ), Path( path ), Size( size ) {}
	wxString Name;
	wxString Path;
	uint64_t Size;
};

struct sFileMenu
{
	void DisableAll();
	void EnableAll();

	wxListBox* List;
	wxButton* BtnOpen;
	wxButton* BtnRemoveSel;
	wxButton* BtnRemoveAll;
	wxButton* BtnSend;
};

class ShareFrame
{
public:
	static void Init( wxWindow* parent );
	static void ShowAndFocus( bool show, bool focus );

private:
	static void InsertList( const wxString& path );
	static bool isFileAlreadyListed( const wxString& path );
	
	static void OnDropFiles( wxDropFilesEvent& event );
	static void OnOpenFiles( wxCommandEvent& event );
	static void OnRemoveSelected( wxCommandEvent& event );
	static void OnRemoveAll( wxCommandEvent& event );
	static void OnSendAll( wxCommandEvent& event );

	static void SendDetach();
	static void ReceiveDetach(); // TODO: After swapping Server/Client 2 Times, files not received
	static void ListenDetach();
	static void ConnectDetach();
	static void CreateContent();
	static void OnClose( wxCloseEvent& event );
	static void OnConnect( wxCommandEvent& event );
	static void UpdateInterface( bool connected );

	static MemoSocket* mSock;
	static bool isListenOver;

	static sFileMenu mFileMenu;
	static wxButton* mConnectBtn;
	static wxTextCtrl* mInputctrl;
	static wxWindow* mParent;
	static wxPanel* mPanel;
	static wxPanel* mIndPanel;
	static wxStaticText* mIndText;
	static std::thread* mListenThread;
	static std::thread* mConnectThread;
	static std::thread* mSendThread;
	static std::thread* mReceiveThread;

	static std::vector<sFileInfo> mSharedFiles;
	static wxFrame* mFrame;

	friend class AutoThread;
};