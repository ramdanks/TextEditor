#pragma once
#include <vector>

#define BYTELEN 8
#define BYTESIZE 256
#define HUFFMAN_REVISION "0.1a"

struct sCompressInfo
{
	uint8_t* pData;
	size_t Size;
};

struct sCompressHeader
{
	uint8_t DataLength : 4;
	uint8_t DecoderLength : 4;
};

class HuffmanCodes
{
public:
	//function will return nullptr if fail to reduce size. force=true will compress it anyway
	static sCompressInfo Compress( const uint8_t* src, size_t size, bool force = false );
	//this function will accept uncompressed format, please give the correct pointer size
	static sCompressInfo Decompress( const sCompressInfo& src );
	//this function will accept uncompressed format, please give the correct pointer size
	static sCompressInfo Decompress( const uint8_t* src, size_t size );
	//this function expect param contain compressed format
	static sCompressInfo Decompress_Pointer( const uint8_t* pCompressed );

	//return uncompressed format
	static sCompressInfo Uncompressed( const uint8_t* src, size_t size );

	//find HuffmanCodes size
	static size_t Calc_Comp_Size( const uint8_t* src, size_t size );

private:
	static uint8_t Len_Bit( const uint8_t& num );
	static bool needDecompression( sCompressHeader header );
	static uint8_t Header_Create( const sCompressHeader& header );
	static sCompressHeader Header_Read( const uint8_t* pCompressed );

	static void Merger( uint8_t* pBuf, uint8_t bitlen, uint8_t* lookup, size_t size );

	static void Expand_HuffmanCodes( const uint8_t* pCompressed, const sCompressHeader& header, std::vector<uint8_t>& vBufferData, std::vector<uint8_t>& vBufferDecoder );
};