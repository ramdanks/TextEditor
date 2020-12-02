#include "MemoSocket.h"
#include <thread>

#define SOCK_CONNECTED mServerSock.IsConnected() || mClientSock.IsConnected()

MemoSocket::MemoSocket()
    : mIsDiscoverable(false), mIsServer(true)
{
    auto& di = mHostInfo.DeviceInfo;
    gethostname( di.Hostname, sizeof di.Hostname );
}

bool MemoSocket::ListenAll()
{
    return Listen( "0.0.0.0" );
}

bool MemoSocket::Listen( const wxString& ip )
{
    wxIPV4address addr;
    addr.Service( MEMO_PORT );
    addr.Hostname( ip );

    wxSocketServer listensock( addr );
    listensock.AcceptWith( mServerSock, true );
    if ( mServerSock.IsClosed() ) return false;

    auto isconnect =  mServerSock.IsConnected();
    if ( isconnect )
    {
        mIsServer = true;
        wxIPV4address peer, local;
        mServerSock.GetPeer( peer );
        mServerSock.GetLocal( local );
        mPeerInfo.Address = peer.IPAddress();
        mHostInfo.Address = local.IPAddress();
    }
    return isconnect;
}

bool MemoSocket::Connect( const wxString& ip )
{
    wxIPV4address addr;
    addr.Service( MEMO_PORT );
    addr.Hostname( ip );

    mClientSock.Connect( addr, true );
    if ( mClientSock.IsClosed() ) return false;

    auto isconnect = mClientSock.IsConnected();
    if ( isconnect )
    {
        mIsServer = false;
        wxIPV4address peer, local;
        mClientSock.GetPeer( peer );
        mClientSock.GetLocal( local );
        mPeerInfo.Address = peer.IPAddress();
        mHostInfo.Address = local.IPAddress();
    }
    return isconnect;
}

bool MemoSocket::CloseServer()
{
    mServerSock.Close();
    return mIsOfficialyConnected;
}

bool MemoSocket::CloseClient()
{
    mClientSock.Close();
    return mIsOfficialyConnected;
}

bool MemoSocket::Disconnect()
{
    mIsOfficialyConnected = false;
    mServerSock.Close();
    mClientSock.Close();
    return true;
}

bool MemoSocket::IsOK()
{
    if ( mIsServer ) return mServerSock.IsOk();
    else             return mClientSock.IsOk();

}

bool MemoSocket::Error()
{
    if ( mIsServer ) return mServerSock.Error();
    else             return mClientSock.Error();
}

bool MemoSocket::IsLocalHost( const wxString& ip ) const
{
    wxIPV4address addr;
    addr.Hostname( ip );
    return addr.IsLocalHost();
}

bool MemoSocket::IsConnected() const
{
    auto serverconnect = mServerSock.IsConnected();
    auto clientconnect = mClientSock.IsConnected();
    return (serverconnect || clientconnect);
}

bool MemoSocket::IsDisconnected() const
{
    auto serverconnect = mServerSock.IsDisconnected();
    auto clientconnect = mClientSock.IsDisconnected();
    return (serverconnect || clientconnect);
}

bool MemoSocket::IsAvailable( const wxString& ip )
{
    return false;
}

bool MemoSocket::IsDiscoverable() const
{
    return mIsDiscoverable;
}

void MemoSocket::SetDiscoverable( bool discover )
{
    mIsDiscoverable = discover;
}

void MemoSocket::Read( Memo::cPacket& buffer )
{
    if ( SOCK_CONNECTED )
    {
        if ( mIsServer ) mServerSock.Read( &buffer, MEMO_PACKETSIZE );
        else             mClientSock.Read( &buffer, MEMO_PACKETSIZE );
    }
}

void MemoSocket::ReadWait( Memo::cPacket& buffer )
{
    if ( SOCK_CONNECTED )
    {
        if ( mIsServer )
        {
            mServerSock.WaitForRead();
            mServerSock.Read( &buffer, MEMO_PACKETSIZE );
        }
        else
        {
            mClientSock.WaitForRead();
            mClientSock.Read( &buffer, MEMO_PACKETSIZE );
        }
    }
}

void MemoSocket::Write( const Memo::cPacket& packet )
{
    if ( SOCK_CONNECTED )
    {
        if ( mIsServer ) mServerSock.Write( &packet, MEMO_PACKETSIZE );
        else             mClientSock.Write( &packet, MEMO_PACKETSIZE );
    }
}

bool MemoSocket::IsServer() const
{
    return mServerSock.IsConnected();
}

bool MemoSocket::IsClient() const
{
    return mClientSock.IsConnected();
}

sHostInfo MemoSocket::GetLocalHostInfo()
{
    wxIPV4address addr;
    wxString hostname;
    wxArrayString addresses;

    char hostbuffer[256];
    if ( gethostname( hostbuffer, sizeof( hostbuffer ) ) == -1 ) return { hostname, addresses };
    hostname = hostbuffer;

    auto host_entry = gethostbyname( hostbuffer );
    if ( host_entry == nullptr ) return { hostname, addresses };

    for ( int i = 0; host_entry->h_addr_list[i] != 0; i++ )
    {
        struct in_addr addr;
        addr.s_addr = *(u_long*) host_entry->h_addr_list[i];
        addresses.Add( inet_ntoa( addr ) );
    }
    return { hostname, addresses };
}

sSocketInfo MemoSocket::GetSocketHostInfo() const
{
    return mHostInfo;
}
sSocketInfo MemoSocket::GetSocketPeerInfo() const
{
    return mPeerInfo;
}

void MemoSocket::SendDeviceInfo()
{
    uint8_t ack = 0;
    while ( ack != TYPE_DEVICEINFO )
    {
        Write( mHostInfo.DeviceInfo );
        Memo::cPacket peerDI;
        Read( peerDI );
        ack = peerDI.AsAck().Type;
    }
}

void MemoSocket::ReadDeviceInfo()
{
    Memo::cPacket peerDI;
    while ( peerDI.isEmpty() ) Read( peerDI );
    mPeerInfo.DeviceInfo = peerDI.AsDeviceInfo();
    Memo::sAck ack = { TYPE_DEVICEINFO };
    Write( ack );
}