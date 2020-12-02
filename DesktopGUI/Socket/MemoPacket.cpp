#include "MemoPacket.h"

Memo::cPacket::cPacket()
{
    memset( mPacket.Payload, 0x00, MEMO_PAYLOADSIZE );
}

Memo::cPacket::cPacket( char* packet )
{
    mPacket = *(sPacket*) packet;
}

Memo::cPacket::cPacket( const cPacket& other )
    : mPacket(other.mPacket)
{
}

Memo::cPacket::cPacket( const sDeviceInfo& payload )
{
    auto copy = payload;
#ifdef _WIN64
    strcpy( copy.OS, "Windows 64-bit" );
#elif _WIN34
    strcpy( copy.OS, "Windows 32-bit" );
#elif __linux__
    strcpy( copy.OS, "Linux" );
#endif	
    mPacket.Header = CreateHeader( TYPE_DEVICEINFO );
    memcpy( mPacket.Payload, (const void*) &copy, sizeof sDeviceInfo );
}

Memo::cPacket::cPacket( const sCRC& payload )
{
    mPacket.Header = CreateHeader( TYPE_CRC );
    memcpy( mPacket.Payload, (const void*) &payload, sizeof sCRC );
}

Memo::cPacket::cPacket( const sDataInfo& payload )
{
    mPacket.Header = CreateHeader( TYPE_DATAINFO );
    memcpy( mPacket.Payload, (const void*) &payload, sizeof sDataInfo );
}

Memo::cPacket::cPacket( const sData& payload )
{
    mPacket.Header = CreateHeader( TYPE_DATA );
    memcpy( mPacket.Payload, (const void*) &payload, sizeof sData );
}

Memo::cPacket::cPacket( const sAck& payload )
{
    mPacket.Header = CreateHeader( TYPE_ACKNOWLEDGEMENT );
    memcpy( mPacket.Payload, (const void*) &payload, sizeof sAck );
}

bool Memo::cPacket::isEmpty()
{
    static char zero[MEMO_PACKETSIZE];
    return memcmp( (void*) &mPacket, zero, MEMO_PACKETSIZE ) == 0;
}

uint8_t Memo::cPacket::GetVersion()
{
    return mPacket.Header.Version;
}

uint8_t Memo::cPacket::GetType()
{
    return mPacket.Header.Type;
}

Memo::sPacket Memo::cPacket::AsPacket()
{
    return mPacket;
}

Memo::sDeviceInfo Memo::cPacket::AsDeviceInfo()
{
    auto payload = (sDeviceInfo*) mPacket.Payload;
    return *payload;
}

Memo::sCRC Memo::cPacket::AsCRC()
{
    auto payload = (sCRC*) mPacket.Payload;
    return *payload;
}

Memo::sDataInfo Memo::cPacket::AsDataInfo()
{
    auto payload = (sDataInfo*) mPacket.Payload;
    return *payload;
}

Memo::sData Memo::cPacket::AsData()
{
    auto payload = (sData*) mPacket.Payload;
    return *payload;
}

Memo::sAck Memo::cPacket::AsAck()
{
    auto payload = (sAck*) mPacket.Payload;
    return *payload;
}

Memo::sHeader Memo::cPacket::CreateHeader( uint8_t type )
{
    return sHeader{ type, MEMO_PROTOCOL_VER };
}