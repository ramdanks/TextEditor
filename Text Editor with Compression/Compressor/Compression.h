#pragma once
#include <vector>
#include <cmath>
#include <fstream>

#define BYTELEN 8
#define BYTESIZE 256

struct sCompressInfo
{
	uint8_t* pData;
	uint64_t SizeSource;
};

struct sCompressHeader
{
	uint8_t DataLength : 4;
	uint8_t DecoderLength : 4;
};

class Compression
{
	size_t CompressionSize;
	size_t OriginalSize;

public:
	Compression() : CompressionSize(0) {}

	uint8_t* Compress( sCompressInfo info );
	uint8_t* Decompress( uint8_t* pCompressed );
	size_t Compression_Size() const;
	size_t Original_Size() const;


private:
	uint8_t Len_Bit( const uint8_t& num );
	uint8_t Header_Create( const sCompressHeader& header );
	sCompressHeader Header_Read( const uint8_t* pCompressed );

	void Merge_Compression( uint8_t* pBuffer, const sCompressHeader& header, const std::vector<uint8_t>& vCompressed, const std::vector<uint8_t>& vDecoder );
	void Expand_Compression( uint8_t* pCompressed, const sCompressHeader& header, std::vector<uint8_t>& vBufferData, std::vector<uint8_t>& vBufferDecoder );
};