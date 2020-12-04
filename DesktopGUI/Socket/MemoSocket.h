#pragma once
#include "MemoPacket.h"

#define MEMO_PORT 6000

struct sHostInfo
{
	wxString Hostname;
	wxArrayString Addresses;
};

struct sSocketInfo
{
	Memo::sDeviceInfo DeviceInfo;
	wxString Address;
};

class MemoSocket
{
public:
	MemoSocket();

	bool ListenAll();
	bool Listen( const wxString& ip );
	bool Connect( const wxString& ip );

	bool CloseServer(); // return is device still connected after closing
	bool CloseClient(); // return is device still connected after closing
	bool Disconnect();  // disconnect both client and server socket
	bool IsOK();
	bool Error();

	bool IsLocalHost( const wxString& ip ) const;
	bool IsConnected() const;
	bool IsDisconnected() const;
	bool IsAvailable( const wxString& ip );
	bool IsDiscoverable() const;
	void SetDiscoverable( bool discover );
	
	void Read( Memo::cPacket& buffer ); 
	void ReadWait( Memo::cPacket& buffer ); // interrupt until packet read
	void Write( const Memo::cPacket& packet );

	bool IsServer() const;
	bool IsClient() const;

	static sHostInfo GetLocalHostInfo();
	sSocketInfo GetSocketHostInfo() const;
	sSocketInfo GetSocketPeerInfo() const;

private:
	void SendDeviceInfo();
	void ReadDeviceInfo();
	sSocketInfo mHostInfo;
	sSocketInfo mPeerInfo;

	wxSocketBase mServerSock;
	wxSocketClient mClientSock;

	std::thread mThread;
	bool mIsOfficialyConnected;
	bool mIsDiscoverable;
	bool mIsServer;
};