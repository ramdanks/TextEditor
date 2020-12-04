#include "ShareFrame.h"
#include "../Feature/LogGUI.h"
#include "../Feature/FilehandleGUI.h"

sFileMenu ShareFrame::mFileMenu;
MemoSocket* ShareFrame::mSock;
wxTextCtrl* ShareFrame::mInputctrl;
wxWindow* ShareFrame::mParent;
wxPanel* ShareFrame::mPanel;
wxPanel* ShareFrame::mIndPanel;
wxStaticText* ShareFrame::mIndText;
std::thread* ShareFrame::mListenThread;
std::thread* ShareFrame::mConnectThread;
std::thread* ShareFrame::mSendThread;
std::thread* ShareFrame::mReceiveThread;
wxFrame* ShareFrame::mFrame;
bool ShareFrame::isListenOver;
wxButton* ShareFrame::mConnectBtn;
std::vector<sFileInfo> ShareFrame::mSharedFiles;

void ShareFrame::Init( wxWindow* parent )
{
	mFrame = new wxFrame( parent, wxID_ANY, "File Sharing", wxDefaultPosition, wxSize( 400, 300 ),
						  wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE );
	mFrame->SetIcon( wxICON( ICON_APP ) );
	mFrame->CreateStatusBar();
	mFrame->SetStatusText( "Share Files over Local Area Network" );

	CreateContent();
	mFileMenu.DisableAll();

	mSock = new MemoSocket;
	mListenThread = new std::thread( &ListenDetach );
	mReceiveThread = new std::thread( &ReceiveDetach );

	mFrame->Bind( wxEVT_CLOSE_WINDOW, OnClose );
}

void ShareFrame::ShowAndFocus( bool show, bool focus )
{
	if ( show )  mFrame->Show();
	if ( focus ) mFrame->Raise();
}

bool ShareFrame::isFileAlreadyListed( const wxString& path )
{
	for ( const auto& file : mSharedFiles )
		if ( file.Path == path ) return true;
	return false;
}

void ShareFrame::OnDropFiles( wxDropFilesEvent& event )
{
	Util::Timer tm( "Drop Files", MS, false );
	try
	{
		wxBusyCursor busyCursor;
		wxWindowDisabler disabler;
		wxBusyInfo busyInfo( "Adding files, wait please..." );

		auto numberdropped = event.GetNumberOfFiles();
		if ( numberdropped < 1 ) return;
		auto files = FilehandleGUI::GetDroppedFiles( event.GetFiles(), numberdropped );
		THROW_ERR_IFEMPTY( files, "Dropped Files is Empty on Dictionary!" );

		mSharedFiles.reserve( mSharedFiles.size() + files.size() );
		for ( int i = 0; i < files.size(); i++ )
		{
			if ( isFileAlreadyListed( files[i] ) )
				LOG_DEBUG( LV_INFO, "File already in shared list: " + CV_STR( files[i] ) );
			else		
				InsertList( files[i] );
		}
	}
	catch ( Util::Err& e )
	{
		LOG_ALL( LV_ERROR, e.Seek() );
		return;
	}
	LOG_ALL_FORMAT( LV_TRACE, "Drag n Drop Shared Files: %d, Time: %f (ms)", event.GetNumberOfFiles(), tm.Toc() );
}

void ShareFrame::OnOpenFiles( wxCommandEvent& event )
{
	auto opened = FilehandleGUI::OpenDialog( mParent, "" );
	if ( isFileAlreadyListed(opened) )
	{
		mFrame->SetStatusText( "File already in share list" );
		LOG_DEBUG( LV_INFO, "File already in share list: " + opened );
		return;
	}
	InsertList( opened );
}

void ShareFrame::OnRemoveSelected( wxCommandEvent& event )
{
	int selection = mFileMenu.List->GetSelection();
	if ( selection == wxNOT_FOUND ) return;

	mFileMenu.List->Delete( selection );
	mSharedFiles.erase( mSharedFiles.begin() + selection );
}

void ShareFrame::OnRemoveAll( wxCommandEvent& event )
{
	mSharedFiles.clear();
	mFileMenu.List->Clear();
}

void ShareFrame::OnSendAll( wxCommandEvent& event )
{
	if ( mSendThread != nullptr )
	{
		mSendThread->join();
		delete mSendThread;
	}
	mSendThread = new std::thread( &SendDetach );
}

void ShareFrame::InsertList( const wxString& path )
{
	auto fName = wxFileNameFromPath( path );
	auto fSize = Filestream::GetFileSize( CV_STR( path ) );
	mSharedFiles.emplace_back( fName, path, fSize );
	mFileMenu.List->Append( fName );
}

void ShareFrame::SendDetach()
{
	mFileMenu.DisableAll();
	for ( uint32_t i = 0; i < mFileMenu.List->GetCount(); i++ )
	{
		Memo::sDataInfo di;
		di.DataIndex = i;
		di.Size = mSharedFiles[i].Size;
		memcpy( di.Name, mSharedFiles[i].Name.mb_str(), sizeof di.Name );
		mSock->Write( di );
	}
	mFileMenu.EnableAll();
}

void ShareFrame::ReceiveDetach()
{
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		while ( mSock->IsServer() )
		{
			Memo::cPacket buffer;
			mSock->ReadWait( buffer );
			auto type = buffer.GetType();
			if ( type == TYPE_DATAINFO )
			{
				auto di = buffer.AsDataInfo();
				mFileMenu.List->Append( di.Name );
			}
		}
	}
}

void ShareFrame::ListenDetach()
{
	isListenOver = false;
	if ( mSock->ListenAll() );
	isListenOver = true;
}

void ShareFrame::ConnectDetach()
{
	mConnectBtn->Disable();
	mInputctrl->Disable();
	mIndText->SetLabel( "Establishing Connection" );
	auto ip = mInputctrl->GetValue();

	try
	{
		auto local = mSock->IsLocalHost( ip );
		THROW_ERR_IF( local, "Cannot connect to local host!" );

		auto connected = mSock->Connect( ip );
		THROW_ERR_IFNOT( connected, "Cannot connec to destination host!" );
	}
	catch ( ... )
	{
		LOG_DEBUG( LV_WARN, "Cannot connect to Host:" + CV_STR( mInputctrl->GetValue() ) );
		mIndText->SetLabel( "Disconnected" );
		mInputctrl->Enable();
	}

	mConnectBtn->Enable();
}

void ShareFrame::CreateContent()
{
	auto hostinfo = MemoSocket::GetLocalHostInfo();

	mPanel = new wxPanel( mFrame, -1 );
	mIndPanel = new wxPanel( mPanel, -1, wxDefaultPosition, wxSize( 0, 20 ) );
	auto hboxInput = new wxBoxSizer( wxHORIZONTAL );
	auto hboxFile = new wxBoxSizer( wxHORIZONTAL );
	auto vboxFile = new wxBoxSizer( wxVERTICAL );
	auto vbox = new wxBoxSizer( wxVERTICAL );

	mIndPanel->SetBackgroundColour( *wxRED );
	mIndPanel->SetForegroundColour( *wxWHITE );
	mIndText = new wxStaticText( mIndPanel, -1, "Disconnected", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER );
	auto indSizer = new wxBoxSizer( wxHORIZONTAL );
	indSizer->Add( mIndText, wxALIGN_CENTER );
	mIndPanel->SetSizer( indSizer );

	auto text = new wxStaticText( mPanel, -1, hostinfo.Hostname );
	auto listip = new wxComboBox( mPanel, -1 );
	listip->Append( hostinfo.Addresses );
	listip->SetSelection( 0 );

	auto patext = new wxStaticText( mPanel, -1, "Peer Address:" );
	mInputctrl  = new wxTextCtrl( mPanel, -1 );
	mConnectBtn = new wxButton( mPanel, 99, "Connect" );
	mConnectBtn->Bind( wxEVT_BUTTON, OnConnect );
	hboxInput->Add( patext, 0, wxRIGHT, 10 );
	hboxInput->Add( mInputctrl, 1, wxRIGHT, 10 );
	hboxInput->Add( mConnectBtn );

	mFileMenu.List = new wxListBox( mPanel, -1 );
	mFileMenu.List->DragAcceptFiles( true );
	mFileMenu.List->Bind( wxEVT_DROP_FILES, OnDropFiles );

	mFileMenu.BtnOpen      = new wxButton( mPanel, -1, "Open File", wxDefaultPosition, wxSize( 130, 25 ) );
	mFileMenu.BtnRemoveSel = new wxButton( mPanel, -1, "Remove Selected", wxDefaultPosition, wxSize( 130, 25 ) );
	mFileMenu.BtnRemoveAll = new wxButton( mPanel, -1, "Remove All", wxDefaultPosition, wxSize( 130, 25 ) );
	mFileMenu.BtnSend      = new wxButton( mPanel, -1, "Send All", wxDefaultPosition, wxSize( 130, 25 ) );

	mFileMenu.BtnOpen->Bind( wxEVT_BUTTON, OnOpenFiles );
	mFileMenu.BtnRemoveSel->Bind( wxEVT_BUTTON, OnRemoveSelected );
	mFileMenu.BtnRemoveAll->Bind( wxEVT_BUTTON, OnRemoveAll );
	mFileMenu.BtnSend->Bind( wxEVT_BUTTON, OnSendAll );

	vboxFile->Add( mFileMenu.BtnOpen, 0 );
	vboxFile->Add( mFileMenu.BtnRemoveSel, 0 );
	vboxFile->Add( mFileMenu.BtnRemoveAll, 0 );
	vboxFile->Add( mFileMenu.BtnSend, 0 );

	hboxFile->Add( mFileMenu.List, 1, wxEXPAND | wxRIGHT, 10 );
	hboxFile->Add( vboxFile, 0 );
	
	vbox->Add( mIndPanel, 0, wxEXPAND );
	vbox->Add( -1, 10 );
	vbox->Add( text, 0, wxCENTER| wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 10 );
	vbox->Add( listip, 0, wxCENTER| wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 10 );
	vbox->Add( hboxInput, 0, wxEXPAND| wxCENTER | wxLEFT | wxRIGHT, 10 );
	vbox->Add( -1, 10 );
	vbox->Add( hboxFile, 1, wxEXPAND| wxCENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10 );
	mPanel->SetSizer( vbox );
}

void ShareFrame::OnClose( wxCloseEvent& event )
{
	mFrame->Show( false );
}

void ShareFrame::OnConnect( wxCommandEvent& event )
{
	if ( mInputctrl->IsEmpty() ) return;

	if ( mSock->IsConnected() ) mSock->Disconnect();
	else
	{
		if ( mConnectThread != nullptr )
		{
			mConnectThread->join();
			delete mConnectThread;
		}
		mConnectThread = new std::thread( &ConnectDetach );
	}
}

void ShareFrame::UpdateInterface( bool connected )
{
	if ( connected )
	{
		mIndPanel->SetBackgroundColour( *wxGREEN );
		mIndPanel->Refresh();
		mIndText->SetLabelText( "Connected" );
		mConnectBtn->SetLabel( "Disconnect" );
		mConnectBtn->Enable();
		mInputctrl->Disable();
		mInputctrl->SetValue( mSock->GetSocketPeerInfo().Address );
		
		if ( mSock->IsClient() ) mFileMenu.EnableAll();
	}
	else
	{
		mIndPanel->SetBackgroundColour( *wxRED );
		mIndPanel->Refresh();
		mIndText->SetLabelText( "Disconnected" );
		mConnectBtn->SetLabel( "Connect" );
		mInputctrl->Enable();

		mSharedFiles.clear();
		mFileMenu.List->Clear();
		mFileMenu.DisableAll();
	}
}

void sFileMenu::DisableAll()
{
	if ( List         != nullptr ) List->Disable();
	if ( BtnOpen      != nullptr ) BtnOpen->Disable();
	if ( BtnSend      != nullptr ) BtnSend->Disable();
	if ( BtnRemoveSel != nullptr ) BtnRemoveSel->Disable();
	if ( BtnRemoveAll != nullptr ) BtnRemoveAll->Disable();
}

void sFileMenu::EnableAll()
{
	if ( List         != nullptr ) List->Enable();
	if ( BtnOpen      != nullptr ) BtnOpen->Enable();
	if ( BtnSend      != nullptr ) BtnSend->Enable();
	if ( BtnRemoveSel != nullptr ) BtnRemoveSel->Enable();
	if ( BtnRemoveAll != nullptr ) BtnRemoveAll->Enable();
}