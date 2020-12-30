#include "Compression.h"
#include <iostream>
#include <future>
#include "../../Utilities/Timer.h"
#include "../../Utilities/Logging.h"

#define ITERATION_SIZE 1024
#define EXCEPTION_SIZE 8096

using namespace std;

sCompressedInfo Compression::Compress( const uint8_t* src, size_t size, bool mt )
{
	uint32_t LowestMergeSize = size;
	uint32_t BestClusterSize = size;

	//small files, doesnt need clusterization
	if ( size <= ITERATION_SIZE )
	{
		//include 1 byte for merge header
		uint32_t ReportedMergeSize = 1 + HuffmanCodes::Calc_Comp_Size( src, size );
		//doesnt need to report BestClusterSize because there's only 1 cluster
		if ( ReportedMergeSize < LowestMergeSize ) LowestMergeSize = ReportedMergeSize;
	}
	//clusterization
	else
	{
		//single-thread workload
		if ( !mt )
		{
			for ( uint32_t i = ITERATION_SIZE; i < size; i += ITERATION_SIZE )
			{				
				if ( i > size ) i = size;
				uint32_t ReportedMergeSize = Compression::Find_Compression_Merge_Size( src, size, i );
				if ( ReportedMergeSize < LowestMergeSize )
				{
					BestClusterSize = i;
					LowestMergeSize = ReportedMergeSize;
				}
			}
		}
		//multi-thread workload
		else
		{
			uint32_t WorkSize = size / ITERATION_SIZE;
			if ( size % ITERATION_SIZE != 0 ) WorkSize++;

			unordered_map<uint32_t, future<uint32_t>> mFuture;
			mFuture.reserve( WorkSize );

			for ( uint32_t i = 0; i < WorkSize; i++ )
			{
				uint32_t ClusterSize = (i+1) * ITERATION_SIZE;
				if ( ClusterSize > size ) ClusterSize = size;
				mFuture[ClusterSize] = async( launch::async, Find_Compression_Merge_Size, src, size, ClusterSize );
			}

			unordered_map<uint32_t, uint32_t> Map;
			Map.reserve( WorkSize );
			for ( auto& future : mFuture )
				Map[future.first] = future.second.get();

			typedef std::pair<const uint32_t, uint32_t> MyPair;
			auto res = min_element( Map.begin(), Map.end(), []( MyPair& a, MyPair& b )
			{
				return a.second < b.second;
			});

			BestClusterSize = res->first;
			LowestMergeSize = res->second;
		}
	}

	//if no efficiency found
	if ( LowestMergeSize >= size )
	{
		auto ucp = HuffmanCodes::Uncompressed( src, size );
		//returning res
		sCompressedInfo res{};
		res.pData            = new uint8_t[1 + ucp.Size];
		res.pData[0]         = res.AddressingSize;
		res.ClusterSize      = 0;
		res.OriginalSize     = size;
		res.AddressingSize   = 0;
		res.CompressedSize   = 1 + ucp.Size;
		memmove( &res.pData[1], ucp.pData, ucp.Size );
		delete[] ucp.pData;
		return res;
	}

	return Compression::Compress_Fix( src, size, BestClusterSize, mt );
}

sCompressedInfo Compression::Compress_Greedy( const uint8_t* src, size_t size, bool mt )
{
	return sCompressedInfo();
}

sCompressedInfo Compression::Compress_Fix( const uint8_t* src, size_t size, uint32_t clusterSize, bool mt )
{
	sCompressedInfo res{};
	res.OriginalSize = size;

	//multi-thread based on amount of cluster
	size_t worker = size / clusterSize;
	size_t remain = size % clusterSize;
	size_t cluster_amount;

	if ( remain ) cluster_amount = worker + 1;
	else cluster_amount = worker;

	//no clusterization; return immediately
	if ( cluster_amount == 1 )
	{
		auto cp = HuffmanCodes::Compress( src, size, true );
		//returning res
		res.pData            = new uint8_t[1 + cp.Size];
		res.pData[0]         = 0;
		res.ClusterSize      = 0;
		res.AddressingSize   = 0;
		res.CompressedSize   = 1 + cp.Size;
		memmove( &res.pData[1], cp.pData, cp.Size );
		return res;
	}

	vector<sCompressInfo> vCompressed;
	vCompressed.reserve(cluster_amount);

	//single-threaded workload
	if ( !mt )
	{
		for ( uint32_t i = 0; i < worker; i++ )
			vCompressed.emplace_back( HuffmanCodes::Compress( &src[i * clusterSize], clusterSize, true ) );
		if ( remain )
			vCompressed.emplace_back( HuffmanCodes::Compress( &src[worker * clusterSize], remain, true ) );
	}
	//multi-threaded workload
	else
	{
		vector<future<sCompressInfo>> vFutures;
		vFutures.reserve( worker );
		for ( uint32_t i = 0; i < worker; i++ )
		{
			vFutures.emplace_back( async( launch::async, 
								   HuffmanCodes::Compress,
								   &src[i * clusterSize],
								   clusterSize,
								   true ) );
		}
		if ( remain )
		{
			vFutures.emplace_back( async( launch::async,
								   HuffmanCodes::Compress,
								   &src[worker * clusterSize],
								   remain,
								   true ) );
		}
		//save
		for ( auto& f : vFutures )
			vCompressed.emplace_back( f.get() );
	}

	//find total cluster size
	size_t compressedSize = 0;
	for ( auto& c : vCompressed )
		compressedSize += c.Size;

	vector<uint32_t> addressList;
	addressList.reserve( cluster_amount + 1 ); //include null address pointer
	addressList.emplace_back( 1 ); //first address always be 1
	for ( uint32_t i = 0; i < cluster_amount - 1; ++i )
	{
		uint32_t nextAddress = addressList.back() + vCompressed[i].Size;
		addressList.emplace_back( nextAddress );
	}
	addressList.emplace_back( 0 );

	//combined size header + address block + total cluster size
	auto addr_len = Addressing_Size( addressList[cluster_amount-1] );
	auto addr_size = addressList.size() * addr_len;
	size_t combinedSize = 1 + addr_size + compressedSize;

	//store return value
	res.pData            = new uint8_t[combinedSize];
	res.pData[0]         = addr_len;
	res.ClusterSize      = cluster_amount;
	res.AddressingSize   = addr_size;
	res.CompressedSize   = combinedSize;
	Store_Cluster_Address( &res.pData[1], addressList, addr_len );

	//save compressed format
	size_t i = 1 + addr_size;
	for ( auto& cp : vCompressed )
	{
		memcpy( &res.pData[i], cp.pData, cp.Size );
		delete[] cp.pData;
		i += cp.Size;
	}
	
	return res;
}

sCompressedInfo Compression::Decompress( const uint8_t* src, size_t size, bool mt )
{
	sCompressedInfo ci;
	ci.pData = nullptr;
	if ( !src || size == 0 ) return ci;

	const uint8_t& addressingSize = src[0];
	if ( !addressingSize )
	{
		auto dp             = HuffmanCodes::Decompress( &src[1], size-1 );
		ci.pData            = dp.pData;
		ci.ClusterSize      = 0;
		ci.OriginalSize     = dp.Size;
		ci.AddressingSize   = 0;
		ci.CompressedSize   = size;
	}
	else
	{
		vector<uint32_t> vAddress;
		vAddress = Retrieve_Cluster_Address( src, addressingSize );

		//skipping address block (including null address)
		uint32_t offset = ( vAddress.size() + 1 ) * addressingSize;
		for ( uint32_t i = 0; i < vAddress.size(); i++ )
			vAddress[i] += offset;
		vAddress.push_back( size ); //for iteration purpose

		const uint32_t workSize = vAddress.size() - 1; //skip last index
		vector<sCompressInfo> vDecomp;
		vDecomp.reserve( workSize );

		//single threaded
		HuffmanCodes cp;
		if ( !mt )
		{
			for ( uint32_t i = 0; i < workSize; i++ )
				vDecomp.emplace_back( HuffmanCodes::Decompress( &src[vAddress[i]], vAddress[i + 1] - vAddress[i] ) );
		}
		//multi threaded
		else
		{
			vector<future<sCompressInfo>> futures;
			futures.reserve( workSize );

			for ( uint32_t i = 0; i < workSize; i++ )
			{
				auto worker = [=] { return HuffmanCodes::Decompress(&src[vAddress[i]], vAddress[i + 1] - vAddress[i]); };
				futures.emplace_back(async(launch::async, worker));
			}

			for ( auto& ft : futures )
				vDecomp.emplace_back( ft.get() );
		}

		//calculate OriginalSize
		uint64_t originalSize = 0;
		for ( auto& i : vDecomp )
			originalSize += i.Size;

		//move to vector
		ci.pData = new uint8_t[originalSize];
		size_t write = 0;
		for ( auto& i : vDecomp )
		{
			memcpy( &ci.pData[write], i.pData, i.Size );
			delete[] i.pData;
			write += i.Size;
		}

		ci.ClusterSize      = workSize;
		ci.OriginalSize     = originalSize;
		ci.AddressingSize   = addressingSize;
		ci.CompressedSize   = size;
	}
	return ci;
}

sCompressedInfo Compression::Decompress( const sCompressedInfo& src, bool mt )
{
	return Decompress( src.pData, src.CompressedSize, mt );
}

uint8_t Compression::Addressing_Size( uint64_t address )
{
	for ( int i = 8; i < 64; i += 8 )
		if ( address - 1 <= pow( 2, i ) ) return i / 8;
}

uint32_t Compression::Find_Compression_Merge_Size( const uint8_t* src, size_t size, uint32_t clusterSize )
{
	if ( clusterSize == 0 ) return 0;

	uint32_t CombinedCompressionSize = 0;
	uint32_t LastSize = size % clusterSize;
	uint32_t ClusterAmount = size / clusterSize;

	for ( uint32_t i = 0; i < ClusterAmount; i++ )
	{
		CombinedCompressionSize += HuffmanCodes::Calc_Comp_Size( &src[i * clusterSize], clusterSize );
	}
	if ( LastSize != 0 )
	{
		CombinedCompressionSize += HuffmanCodes::Calc_Comp_Size( &src[ClusterAmount * clusterSize], LastSize );
	}
	uint32_t addressingSize = Addressing_Size( CombinedCompressionSize );
	//reported size: header + Address Block size + Combined Compression Size
	return 1 + ClusterAmount * addressingSize + CombinedCompressionSize;
}

void Compression::Store_Cluster_Address( uint8_t* buffer, const std::vector<uint32_t>& vList, uint8_t addressing )
{
	size_t i = 0;
	for ( auto& address : vList )
	{
		memcpy( &buffer[i*addressing], &address, addressing );
		i++;
	}
}

std::vector<uint32_t> Compression::Retrieve_Cluster_Address( const uint8_t* src, uint8_t addressing )
{
	constexpr uint32_t MaskBytes[] = { 0x0, 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };
	vector<uint32_t> vList;
	vList.reserve(64);
	for ( uint32_t readIndex = 1; ; readIndex += addressing )
	{
		uint32_t readAddress = *(uint32_t*) &src[readIndex];
		readAddress &= MaskBytes[addressing];
		if ( !readAddress ) break;
		vList.emplace_back( readAddress );
	}
	return vList;
}