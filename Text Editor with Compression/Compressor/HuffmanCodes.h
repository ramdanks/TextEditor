#pragma once
#include <vector>

#define BYTELEN 8
#define BYTESIZE 256
#define HUFFMAN_REVISION "0.1a"

struct sCompressInfo
{
	uint8_t* pData;
	uint32_t SizeSource;
};

struct cpinfo
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
	size_t CompressedSize;
	size_t DecompressedSize;

public:
	HuffmanCodes() : CompressedSize(0) {}

	//function will return nullptr if fail to reduce size. force=true will compress it anyway
	uint8_t* Compress( sCompressInfo info, bool force = false );
	//function will leave vBuffer empty if fail to reduce size. force=true will compress it anyway
	static void Compress_Buffer( std::vector<uint8_t>& vBuffer, sCompressInfo info, bool force = false );
	//this function expect param contain compressed format
	static cpinfo Decompress( const uint8_t* pCompressed );
	//this function expect param contain compressed format
	static void Decompress_Buffer( std::vector<uint8_t>& vBuffer, const uint8_t* pCompressed );
	//this function will accept uncompressed format, please give the correct pointer size
	static cpinfo Decompress_Reference( const uint8_t* pCompressed, uint32_t pSize );
	//this function will accept uncompressed format, please give the correct pointer size
	static void Decompress_Reference_Buffer( std::vector<uint8_t>& vBuffer, const uint8_t* pCompressed, uint32_t pSize );

	//return uncompressed format
	uint8_t* Uncompressed( sCompressInfo info );
	//return uncompressed format, buffer will be erased
	static void Uncompressed_Buffer( sCompressInfo info, std::vector<uint8_t>& vBuffer );

	size_t Compressed_Size() const;
	size_t Decompressed_Size() const;

	//find HuffmanCodes size
	static uint32_t Calc_Comp_Size( sCompressInfo info );

private:
	static uint8_t Len_Bit( const uint8_t& num );
	static bool needDecompression( sCompressHeader header );
	static uint8_t Header_Create( const sCompressHeader& header );
	static sCompressHeader Header_Read( const uint8_t* pCompressed );

	static void Merge_HuffmanCodes( uint8_t* pBuffer, const sCompressHeader& header, const std::vector<uint8_t>& vCompressed, const std::vector<uint8_t>& vDecoder );
	static void Expand_HuffmanCodes( const uint8_t* pCompressed, const sCompressHeader& header, std::vector<uint8_t>& vBufferData, std::vector<uint8_t>& vBufferDecoder );
};