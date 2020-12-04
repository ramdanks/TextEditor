#pragma once

#define MEMO_PROTOCOL_VER 0

#define MEMO_PACKETSIZE 1024
#define MEMO_HEADERSIZE sizeof Memo::sHeader
#define MEMO_PAYLOADSIZE MEMO_PACKETSIZE - MEMO_HEADERSIZE

#define TYPE_DEVICEINFO 1
#define TYPE_CRC 2
#define TYPE_DATAINFO 3
#define TYPE_DATA 4
#define TYPE_ACKNOWLEDGEMENT 5

namespace Memo
{
	struct sHeader
	{
		uint8_t Type : 4;
		uint8_t Version : 4;
	};

	struct sPacket
	{
		sHeader Header;
		char Payload[MEMO_PAYLOADSIZE];
	};

	// All the struct below is considered as payload
	// payload must be <= MEMO_PAYLOADSIZE

	struct sDeviceInfo
	{
		char OS[64];
		char Hostname[256];
	};

	struct sCRC
	{
		uint8_t DataIndex;
		uint64_t Hash;
	};

	struct sDataInfo
	{
		uint8_t DataIndex; //assigned index
		uint64_t Size;
		char Name[256];
	};

	struct sData
	{
		uint8_t DataIndex; //match the assigned index
		uint64_t PayloadIndex ;
		char Data[MEMO_PAYLOADSIZE - 9];
	};

	struct sAck
	{
		uint8_t Type;
	};

	class cPacket
	{
	public:
		cPacket();
		cPacket( char* packet );
		cPacket( const cPacket& other );
		cPacket( const sDeviceInfo& payload );
		cPacket( const sCRC& payload );
		cPacket( const sDataInfo& payload );
		cPacket( const sData& payload );
		cPacket( const sAck& payload );

		bool isEmpty();
		uint8_t GetVersion();
		uint8_t GetType();
		
		sPacket AsPacket();
		sDeviceInfo AsDeviceInfo();
		sCRC AsCRC();
		sDataInfo AsDataInfo();
		sData AsData();
		sAck AsAck();

	private:
		sHeader CreateHeader( uint8_t type );
		sPacket mPacket;
	};
}