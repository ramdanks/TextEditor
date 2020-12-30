#pragma once
#include <vector>
#include "HuffmanCodes.h"
#define COMPRESSION_REVISION "0.2a"

struct sArchiveHeader
{
	uint8_t AddressingSize;
	std::vector<uint8_t> Address;
};

struct sCompressedInfo
{
	uint8_t* pData;
	size_t ClusterSize;
	size_t AddressingSize;
	size_t CompressedSize;
	size_t OriginalSize;
};

class Compression
{
public:
	Compression() = delete;
	Compression( const Compression& other ) = delete;
	Compression operator = ( const Compression& other ) = delete;

	//compression use clusterization, automatically find the best cluster size
	static sCompressedInfo Compress( const uint8_t* src, size_t size, bool mt );
	//Best efficiency of cluster size achieve by greedy algorithm
	static sCompressedInfo Compress_Greedy( const uint8_t* src, size_t size, bool mt );
	//compression with fix cluster size
	static sCompressedInfo Compress_Fix( const uint8_t* src, size_t size, uint32_t clusterSize, bool mt );
	//Decompress merge compressed file.
	static sCompressedInfo Decompress( const uint8_t* src, size_t size, bool mt );
	//Decompress merge compressed file.
	static sCompressedInfo Decompress( const sCompressedInfo& src, bool mt );

	sCompressedInfo GetInfo();

private:
	//return amount of bytes needed to store address value
	static uint8_t inline Addressing_Size( uint64_t address );
	//find compression size based on cluster size
	static uint32_t Find_Compression_Merge_Size( const uint8_t* src, size_t size, uint32_t clusterSize );

	//this function not include null address pointer, push_back(0) before entering this function
	static void Store_Cluster_Address( uint8_t* buffer, const std::vector<uint32_t>& vList, uint8_t addressing );
	//this function except null address pointer, and will return the vector without the null value
	static std::vector<uint32_t> Retrieve_Cluster_Address( const uint8_t* src, uint8_t addressing );
};