#include "Merger.h"
#include <iostream>
#include <thread>

#define ITERATION_SIZE 8096
#define EXCEPTION_SIZE 8096

std::vector<uint8_t> Merger::Compress_Merge( const sCompressInfo& info, bool mt )
{
	uint32_t IterationSize = info.SizeSource / 1024;
	uint32_t LowestMergeSize = info.SizeSource;
	uint32_t BestClusterSize = info.SizeSource;

	//small files, doesnt need clusterization
	if ( IterationSize == 0 )
	{
		//include 1 byte for merge header
		uint32_t ReportedMergeSize = 1 + Compression::Calc_Comp_Size( info );
		//doesnt need to report BestClusterSize because there's only 1 cluster
		if ( ReportedMergeSize < LowestMergeSize ) LowestMergeSize = ReportedMergeSize;
	}
	//clusterization
	else
	{
		//single-thread workload
		if ( !mt )
		{
			for ( uint32_t i = IterationSize; i < info.SizeSource; i += IterationSize )
			{
				uint32_t ReportedMergeSize;
				if ( i > info.SizeSource ) i = info.SizeSource;
				Merger::Find_Compression_Merge_Size( i, info, ReportedMergeSize );
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
			uint32_t ThreadSize = info.SizeSource / IterationSize;
			if ( info.SizeSource % IterationSize != 0 ) ThreadSize++;

			std::vector<uint32_t> MergeSize( ThreadSize );
			std::vector<std::thread> pThread( ThreadSize );

			for ( uint32_t i = 0; i < ThreadSize; i++ )
			{
				uint32_t ClusterSize = (i+1) * IterationSize;
				if ( ClusterSize > info.SizeSource ) ClusterSize = info.SizeSource;
				pThread[i] = std::thread( Merger::Find_Compression_Merge_Size, ClusterSize, info, std::ref( MergeSize[i] ) );
			}
	
			for ( uint32_t i = 0; i < ThreadSize; i++ )
				pThread[i].join();

			for ( uint32_t i = 0; i < ThreadSize; i++ )
				if ( MergeSize[i] < LowestMergeSize )
				{
					LowestMergeSize = MergeSize[i];
					BestClusterSize = ( i + 1 ) * IterationSize;
				}
		}
	}

	//if no efficiency found
	if ( LowestMergeSize == info.SizeSource )
	{
		Compression cp;
		uint8_t* pUncompressed = cp.Uncompressed( info );
		std::vector<uint8_t> vUncompressedMerge( 1 + cp.Compression_Size() );
		vUncompressedMerge[0] = 0; //null addressing
		memcpy( &vUncompressedMerge[1], pUncompressed, cp.Compression_Size() );
		return vUncompressedMerge;
	}

	return Merger::Compress_Merge_Fix( BestClusterSize, info, mt );
}

std::vector<uint8_t> Merger::Compress_Merge_Greedy( const sCompressInfo& info, bool mt )
{
	#define REMAINING_UNCOMPRESSED (info.SizeSource - readIndex)

	std::vector<uint32_t> addressList;
	std::vector<uint8_t> bufferCompressed;
	std::vector<sCompressInfo> vCInfo;

	uint32_t readIndex = 0;
	uint32_t writeSize = info.SizeSource;

	while ( readIndex < info.SizeSource )
	{
		double bestEfficiency = 0;
		//find best ratio
		for ( uint32_t i = 0; i < REMAINING_UNCOMPRESSED; )
		{
			i += ITERATION_SIZE;
			if ( i > REMAINING_UNCOMPRESSED ) i = REMAINING_UNCOMPRESSED;
			sCompressInfo ci;
			ci.SizeSource = i;
			ci.pData = &info.pData[readIndex];
			double efficiency = (double) i / Compression::Calc_Comp_Size( ci );
			if ( efficiency > bestEfficiency )
			{
				bestEfficiency = efficiency;
				writeSize = i;
			}
		}
		//if fail to reduce size
		if ( bestEfficiency < 1.0 )
		{
			if ( REMAINING_UNCOMPRESSED < EXCEPTION_SIZE ) writeSize = REMAINING_UNCOMPRESSED;
			else writeSize = EXCEPTION_SIZE;
		}
		//save cluster address
		addressList.push_back( bufferCompressed.size() + 1 ); //save cluster address

		sCompressInfo ci;
		ci.pData = &info.pData[readIndex];
		ci.SizeSource = writeSize;

		if ( mt ) vCInfo.push_back( ci );
		else
		{
			std::vector<uint8_t> buffer;
			Compression::Compress_Buffer( buffer, ci, true );
			//copy compressed value to buffer
			for ( auto i : buffer )
				bufferCompressed.push_back( i );
		}

		readIndex += writeSize; //iterate to next non-compressed data
	}

	uint8_t addressingSize = 0;
	//if there's multiple cluster
	if ( addressList.size() > 1 )
	{
		//find amount of bytes needed to save cluster address
		addressingSize = Addressing_Size( addressList.back() );
		addressList.push_back( 0 ); //terminating address pointer (divider between address and compressed block)
	}

	//merge all component (header, address, compressedData) / assembling of compressed data
	std::vector<uint8_t> bufferMerge;
	if ( mt )
	{
		std::vector<std::thread> pThread( vCInfo.size() );
		std::vector<std::vector<uint8_t>> vCompressBuffer( vCInfo.size() );
		for ( uint32_t i = 0; i < vCInfo.size(); i++ )
			pThread[i] = std::thread( Compression::Compress_Buffer, std::ref( vCompressBuffer[i] ), vCInfo[i], true );

		for ( uint32_t i = 0; i < vCInfo.size(); i++ )
			pThread[i].join();

		uint32_t CompressSize = 0;
		for ( auto i : vCompressBuffer )
			CompressSize += i.size();

		uint32_t CombinedSize = 1 + addressList.size() * addressingSize + CompressSize;
		bufferMerge.reserve( CombinedSize );

		bufferMerge.push_back( addressingSize );
		if ( addressList.size() > 1 ) Store_Cluster_Address( addressList, addressingSize, bufferMerge );
		for ( auto i : vCompressBuffer )
			for ( auto j : i )
				bufferMerge.push_back( j );
	}
	else
	{
		//header(1) + addressingSize(n*size) + compressedData(n)
		uint32_t CombinedSize = 1 + addressList.size() * addressingSize + bufferCompressed.size();
		bufferMerge.reserve( CombinedSize );

		bufferMerge.push_back( addressingSize );
		if ( addressList.size() > 1 ) Store_Cluster_Address( addressList, addressingSize, bufferMerge );
		for ( auto i : bufferCompressed ) bufferMerge.push_back( i );
	}

	return bufferMerge;
}

std::vector<uint8_t> Merger::Compress_Merge_Fix( uint32_t clusterSize, const sCompressInfo& info, bool mt )
{
	//multi-thread based on amount of cluster
	uint32_t threadSize = std::ceil( (double) info.SizeSource / clusterSize );
	std::vector<std::vector<uint8_t>> vBufferCompressed( threadSize );

	sCompressInfo lastCluster;
	lastCluster.pData = (uint8_t*) &info.pData[( threadSize - 1 ) * clusterSize];
	if ( clusterSize == info.SizeSource ) lastCluster.SizeSource = info.SizeSource;
	else                                  lastCluster.SizeSource = info.SizeSource % clusterSize;

	//single-thread workload if not enable mt or only 1 cluster
	if ( !mt || threadSize == 1 )
	{
		for ( uint32_t i = 0; i < threadSize - 1; i++ )
		{
			sCompressInfo ci;
			ci.pData = (uint8_t*) &info.pData[i * clusterSize];
			ci.SizeSource = clusterSize;
			Compression::Compress_Buffer( vBufferCompressed[i], ci, true );
		}
		Compression::Compress_Buffer( vBufferCompressed[threadSize - 1], lastCluster, true );
	}
	//multi-thread workload
	else
	{
		std::vector<std::thread> pThread( threadSize );
		for ( uint32_t i = 0; i < threadSize - 1; i++ )
		{
			sCompressInfo ci;
			ci.pData = (uint8_t*) &info.pData[i * clusterSize];
			ci.SizeSource = clusterSize;
			pThread[i] = std::thread( Compression::Compress_Buffer, std::ref( vBufferCompressed[i] ), ci, true );
		}
		pThread[threadSize - 1] = std::thread( Compression::Compress_Buffer, std::ref( vBufferCompressed[threadSize - 1] ), lastCluster, true );

		for ( uint32_t i = 0; i < threadSize; i++ )
			pThread[i].join();
	}

	//find total cluster size
	uint32_t CompressedSize = 0;
	for ( auto i : vBufferCompressed )
		CompressedSize += i.size();

	//store cluster address if more than 1 cluster
	uint32_t addressingSize = 0;
	std::vector<uint32_t> addressList;
	if ( threadSize > 1 )
	{
		addressList.reserve( threadSize + 1 ); //include null address pointer
		addressList.push_back( 1 ); //first address always be 1
		for ( uint32_t i = 0; i < threadSize - 1; i++ )
		{
			uint32_t nextAddress = addressList.back() + vBufferCompressed[i].size();
			addressList.push_back( nextAddress );
		}
		addressingSize = Addressing_Size( addressList.back() );
		addressList.push_back( 0 );
	}

	//combined size header + address block + total cluster size
	uint32_t CombinedSize = 1 + addressList.size() * addressingSize + CompressedSize;

	std::vector<uint8_t> vCompressed;
	vCompressed.reserve( CombinedSize );
	vCompressed.push_back( addressingSize ); //insert header

	//threadSize also indicates the amount of cluster, only store store address if more than 1 cluster
	if ( threadSize > 1 ) Store_Cluster_Address( addressList, addressingSize, vCompressed );

	//save compressed format
	for ( auto i : vBufferCompressed )
		for ( auto j : i )
			vCompressed.push_back( j );
	
	return vCompressed;
}

std::vector<uint8_t> Merger::Decompress_Merge( const std::vector<uint8_t>& merged, bool mt )
{
	const uint8_t& addressingSize = merged[0];
	if ( addressingSize == 0 )
	{
		Compression cp;
		std::vector<uint8_t> vDecompressed;
		Compression::Decompress_Reference_Buffer( vDecompressed, &merged[1], merged.size() - 1 ); //no clusterization, skip merger header
		return vDecompressed;
	}
	else
	{
		std::vector<uint32_t> vAddress;
		vAddress = Retrieve_Cluster_Address( addressingSize, merged );

		//skipping address block (including null address)
		uint32_t offset = (vAddress.size() + 1) * addressingSize;
		for ( uint32_t i = 0; i < vAddress.size(); i++ )
			vAddress[i] += offset;
		vAddress.push_back( merged.size() ); //add null address pointer
		
		const uint32_t threadSize = vAddress.size() - 1; //skip null address pointer
		std::vector<std::vector<uint8_t>> vDecompressedBuffer( threadSize );

		//single threaded
		Compression cp;
		if ( !mt )
		{
			//Decompress Reference is safest way to decompress file
			//if the we are decompressing an Uncompressed format, this function can handle
			//we should give the pointer size as a reference, to tell how large the originalsize of uncompressed format
			for ( uint32_t i = 0; i < vAddress.size() - 1; i++ )
				Compression::Decompress_Reference_Buffer( vDecompressedBuffer[i], &merged[vAddress[i]], vAddress[i + 1] - vAddress[i] );
		}
		//multi threaded
		else
		{
			std::vector<std::thread> pThread( threadSize );

			for ( uint32_t i = 0; i < threadSize; i++ )
				pThread[i] = std::thread( Compression::Decompress_Reference_Buffer, std::ref( vDecompressedBuffer[i] ), &merged[vAddress[i]], vAddress[i + 1] - vAddress[i] );

			for ( uint32_t i = 0; i < threadSize; i++ )
				pThread[i].join();
		}

		//calculate OriginalSize
		uint32_t OriginalSize = 0;
		for ( auto i : vDecompressedBuffer )
			OriginalSize += i.size();

		std::vector<uint8_t> vDecompressed;
		vDecompressed.reserve( OriginalSize );

		for ( auto i : vDecompressedBuffer )
			for ( auto j : i )
				vDecompressed.push_back( j );	

		return vDecompressed;
	}
}

uint8_t Merger::Addressing_Size( uint64_t address )
{
	for ( int i = 8; i < 64; i += 8 )
		if ( address - 1 <= pow( 2, i ) ) return i / 8;
}

void Merger::Find_Compression_Merge_Size( uint32_t clusterSize, const sCompressInfo& info, uint32_t& bufferSize )
{
	if ( clusterSize == 0 ) return;

	uint32_t CombinedCompressionSize = 0;
	uint32_t LastSize = info.SizeSource % clusterSize;
	uint32_t ClusterAmount = info.SizeSource / clusterSize;
	for ( uint32_t i = 0; i < ClusterAmount; i++ )
	{
		sCompressInfo ci;
		ci.pData = &info.pData[i * clusterSize];
		ci.SizeSource = clusterSize;
		CombinedCompressionSize += Compression::Calc_Comp_Size( ci );
	}
	if ( LastSize != 0 )
	{
		sCompressInfo ci;
		ci.pData = &info.pData[ClusterAmount * clusterSize];
		ci.SizeSource = LastSize;
		CombinedCompressionSize += Compression::Calc_Comp_Size( ci );
	}
	uint32_t addressingSize = Addressing_Size( CombinedCompressionSize );
	//reported size: header + Address Block size + Combined Compression Size
	bufferSize = 1 + ClusterAmount * addressingSize + CombinedCompressionSize;
}

void Merger::Store_Cluster_Address( const std::vector<uint32_t>& vList, uint8_t addressing, std::vector<uint8_t>& vBuffer )
{
	for ( auto address : vList )
	{
		uint8_t* ptr = (uint8_t*) &address;
		for ( int i = 0; i < addressing; i++ )
		{
			vBuffer.push_back( *ptr );
			ptr++;
		}
	}
}

std::vector<uint32_t> Merger::Retrieve_Cluster_Address( uint8_t addressing, const std::vector<uint8_t>& vBuffer )
{
	constexpr uint64_t MaskBytes[] = { 0x0, 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF, 0xFFFFFFFFFF, 0xFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF };
	std::vector<uint32_t> vList;
	uint32_t readAddress;
	for ( uint32_t readIndex = 1; ; readIndex += addressing )
	{
		readAddress = *(uint32_t*) &vBuffer[readIndex];
		readAddress = readAddress & MaskBytes[addressing];
		if ( readAddress == 0 ) break;
		vList.push_back( readAddress );
	}
	return vList;
}