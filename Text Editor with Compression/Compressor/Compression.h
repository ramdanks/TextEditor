#pragma once
#include <vector>
#include "HuffmanCodes.h"
#define COMPRESSION_REVISION "0.2a"

struct sArchiveHeader
{
	uint8_t AddressingSize;
	std::vector<uint8_t> Address;
};

class Compression
{
public:
	Compression() = delete;
	Compression( const Compression& other ) = delete;
	Compression operator = ( const Compression& other ) = delete;

	//Merge compression use clusterization, automatically find the best cluster size
	static std::vector<uint8_t> Compress_Merge( const sCompressInfo& info, bool mt );
	//Best efficiency of cluster size achieve by greedy algorithm
	static std::vector<uint8_t> Compress_Merge_Greedy( const sCompressInfo& info, bool mt );
	//Merge compression with fix clusterSize
	static std::vector<uint8_t> Compress_Merge_Fix( uint32_t clusterSize, const sCompressInfo& info, bool mt );

	//Decompress merge compressed file. cannot handle non-merged compression file.
	static std::vector<uint8_t> Decompress_Merge( const std::vector<uint8_t>& merged, bool mt );

private:
	//return amount of bytes needed to store address value
	static uint8_t inline Addressing_Size( uint64_t address );
	//find compression size based on cluster size
	static uint32_t Find_Compression_Merge_Size( uint32_t clusterSize, const sCompressInfo& info );

	//this function not include null address pointer, push_back(0) before entering this function
	static void Store_Cluster_Address( const std::vector<uint32_t>& vList, uint8_t addressing, std::vector<uint8_t>& vBuffer );
	//this function except null address pointer, and will return the vector without the null value
	static std::vector<uint32_t> Retrieve_Cluster_Address( uint8_t addressing, const std::vector<uint8_t>& vBuffer );
};