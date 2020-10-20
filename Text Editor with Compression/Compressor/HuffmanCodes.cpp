#include "HuffmanCodes.h"
#include <stdlib.h>
#include <cmath>

constexpr uint8_t MaskTemplate[9] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

uint8_t* HuffmanCodes::Compress( sCompressInfo info, bool force )
{
	if ( info.SizeSource == 0 || info.pData == nullptr ) return nullptr;

	bool* ByteCombination = (bool*) calloc( BYTESIZE, sizeof( bool ) );

	//find amount of byte combination in source data
	for ( uint64_t i = 0; i < info.SizeSource; i++ )
		ByteCombination[info.pData[i]] = true;

	//based on amount of byte combination in sourcedata, create new value and store old value
	std::vector<uint8_t> OldValueTable;
	uint8_t* NewValueTable = (uint8_t*) calloc( BYTESIZE, sizeof( bool ) );
	uint8_t NewValue = 0;
	for ( size_t i = 0; i < BYTESIZE; i++ )
	{
		if ( ByteCombination[i] )
		{
			//the reason NewValue starts at "1" is because we need "0" to represent a terminating pointer
			//when we merge the compressed value, at the end we put "0" to indicate the end of data
			//this trick is to seperates or identify where's the compressed data and the decoder, so no need to memorize location
			NewValue++;
			NewValueTable[i] = NewValue;
			OldValueTable.push_back( i );
		}
	}

	//calculate HuffmanCodes size
	auto BitLenData = Len_Bit( NewValue ); //find bit length to represent Highest number of new value
	auto BitLenDecoder = Len_Bit( OldValueTable.back() ); //find bit length to represent Highest number of old value

	//NewValue represent how much combination of bytes in compressed format
	uint64_t TotalAllocationData = std::ceil( (double) BitLenData * (info.SizeSource + 1) / 8 ); //find amount of bytes to store compressed data
	uint64_t TotalAllocationDecoder = std::ceil( (double) BitLenDecoder * NewValue / 8 ); //find amount of bytes to store old (original) data
	uint64_t TotalAllocationCombined = 1 + TotalAllocationData + TotalAllocationDecoder; //header need 1 byte
	this->CompressedSize = TotalAllocationCombined;

	//if HuffmanCodes doesnt reduce it's original size, then return null
	if ( TotalAllocationCombined >= info.SizeSource )
	{
		if ( !force ) return nullptr;
		else
		{
			this->CompressedSize = 1 + info.SizeSource;
			return HuffmanCodes::Uncompressed( info );
		}
	}

	uint8_t* pCompressed = (uint8_t*) calloc( this->CompressedSize, sizeof( uint8_t ) );

	//create buffer for HuffmanCodes data based on original order
	std::vector<uint8_t> BufferCompressedData;
	BufferCompressedData.resize( info.SizeSource + 1 ); //additional bytes for null pointer
	BufferCompressedData[info.SizeSource] = 0; //add terminating pointer at the end
	for ( uint64_t i = 0; i < info.SizeSource; i++ )
		BufferCompressedData[i] = NewValueTable[info.pData[i]];
	
	//create header
	sCompressHeader header;
	header.DataLength = BitLenData;
	header.DecoderLength = BitLenDecoder;
	pCompressed[0] = Header_Create( header );

	//combine header, compressed value, decoder tablo to pCompressed buffer.
	Merge_HuffmanCodes( &pCompressed[1], header, BufferCompressedData, OldValueTable );

	return pCompressed;
}

void HuffmanCodes::Compress_Buffer( std::vector<uint8_t>& vBuffer, sCompressInfo info, bool force )
{
	if ( info.SizeSource == 0 || info.pData == nullptr ) return;

	bool* ByteCombination = (bool*) calloc( BYTESIZE, sizeof( bool ) );

	//find amount of byte combination in source data
	for ( uint64_t i = 0; i < info.SizeSource; i++ )
		ByteCombination[info.pData[i]] = true;

	//based on amount of byte combination in sourcedata, create new value and store old value
	std::vector<uint8_t> OldValueTable;
	uint8_t* NewValueTable = (uint8_t*) calloc( BYTESIZE, sizeof( bool ) );
	uint8_t NewValue = 0;
	for ( size_t i = 0; i < BYTESIZE; i++ )
	{
		if ( ByteCombination[i] )
		{
			//the reason NewValue starts at "1" is because we need "0" to represent a terminating pointer
			//when we merge the compressed value, at the end we put "0" to indicate the end of data
			//this trick is to seperates or identify where's the compressed data and the decoder, so no need to memorize location
			NewValue++;
			NewValueTable[i] = NewValue;
			OldValueTable.push_back( i );
		}
	}
	//calculate HuffmanCodes size
	auto BitLenData = Len_Bit( NewValue ); //find bit length to represent Highest number of new value
	auto BitLenDecoder = Len_Bit( OldValueTable.back() ); //find bit length to represent Highest number of old value

	//NewValue represent how much combination of bytes in compressed format
	uint32_t TotalAllocationData = std::ceil( (double) BitLenData * ( info.SizeSource + 1 ) / 8 ); //find amount of bytes to store compressed data
	uint32_t TotalAllocationDecoder = std::ceil( (double) BitLenDecoder * NewValue / 8 ); //find amount of bytes to store old (original) data
	uint32_t TotalAllocationCombined = 1 + TotalAllocationData + TotalAllocationDecoder; //header need 1 byte

	//if HuffmanCodes doesnt reduce it's original size, then return null
	if ( TotalAllocationCombined >= info.SizeSource )
	{
		if ( !force ) return;
		else
		{
			return HuffmanCodes::Uncompressed_Buffer( info, vBuffer );
		}
	}

	vBuffer.resize( TotalAllocationCombined );

	//create buffer for HuffmanCodes data based on original order
	std::vector<uint8_t> BufferCompressedData;
	BufferCompressedData.resize( info.SizeSource + 1 ); //additional bytes for null pointer
	BufferCompressedData[info.SizeSource] = 0; //add terminating pointer at the end
	for ( uint32_t i = 0; i < info.SizeSource; i++ )
		BufferCompressedData[i] = NewValueTable[info.pData[i]];

	//create header
	sCompressHeader header;
	header.DataLength = BitLenData;
	header.DecoderLength = BitLenDecoder;
	vBuffer[0] = Header_Create( header );

	//combine header, compressed value, decoder tablo to pCompressed buffer.
	Merge_HuffmanCodes( &vBuffer[1], header, BufferCompressedData, OldValueTable );
}

uint8_t* HuffmanCodes::Decompress( const uint8_t* pCompressed )
{
	if ( pCompressed == nullptr ) return nullptr;

	uint8_t* pOriginal = nullptr;
	sCompressHeader header = Header_Read( pCompressed );

	std::vector<uint8_t> vBufferData, vBufferDecoder;
	Expand_HuffmanCodes( pCompressed, header, vBufferData, vBufferDecoder );

	//allocate, original size is determined
	pOriginal = (uint8_t*) malloc( vBufferData.size() * sizeof( uint8_t ) );
	this->DecompressedSize = vBufferData.size();

	//translation with decoder table
	for ( size_t i = 0; i < vBufferData.size(); i++ )
	{
		auto val = vBufferData[i];
		pOriginal[i] = vBufferDecoder[val-1];
	}
	return pOriginal;
}

void HuffmanCodes::Decompress_Buffer( std::vector<uint8_t>& vBuffer, const uint8_t* pCompressed )
{
	if ( pCompressed == nullptr ) return;

	sCompressHeader header = Header_Read( pCompressed );
	std::vector<uint8_t> vBufferData, vBufferDecoder;
	Expand_HuffmanCodes( pCompressed, header, vBufferData, vBufferDecoder );

	//reserve size to avoid dynamic allocation
	vBuffer.resize( vBufferData.size() );

	//translation with decoder table
	for ( size_t i = 0; i < vBufferData.size(); i++ )
		vBuffer[i] = vBufferDecoder[vBufferData[i] - 1];
}

uint8_t* HuffmanCodes::Decompress_Reference( const uint8_t* pCompressed, uint32_t pSize )
{
	sCompressHeader header = Header_Read( pCompressed );
	if ( !needDecompression( header ) )
	{
		//pSize is size of an uncompressed format
		//the actual size if (size-1) because we ignore HuffmanCodes header which cost 1 byte.
		this->DecompressedSize = pSize - 1;
		uint8_t* pOriginal = (uint8_t*) malloc( this->DecompressedSize );
		memcpy( pOriginal, &pCompressed[1], this->DecompressedSize );
		return pOriginal;
	}
	return Decompress( pCompressed );
}

void HuffmanCodes::Decompress_Reference_Buffer( std::vector<uint8_t>& vBuffer, const uint8_t* pCompressed, uint32_t pSize )
{
	sCompressHeader header = Header_Read( pCompressed );
	if ( !needDecompression( header ) )
	{
		uint32_t OriginalSize = pSize - 1;
		vBuffer.resize( OriginalSize );
		for ( uint32_t i = 0; i < OriginalSize; i++ ) vBuffer[i] = pCompressed[1 + i];
		return;
	}
	Decompress_Buffer( vBuffer, pCompressed );
}

uint8_t* HuffmanCodes::Uncompressed( sCompressInfo info )
{
	this->DecompressedSize = 1 + info.SizeSource;
	uint8_t* pUncompressed = (uint8_t*) malloc( 1 + info.SizeSource );
	sCompressHeader header;
	header.DataLength = 8;
	header.DecoderLength = 0;
	pUncompressed[0] = Header_Create( header );
	memcpy( &pUncompressed[1], info.pData, info.SizeSource );
	return pUncompressed;
}

void HuffmanCodes::Uncompressed_Buffer( sCompressInfo info, std::vector<uint8_t>& vBuffer )
{
	vBuffer.resize( 1 + info.SizeSource );
	sCompressHeader header;
	header.DataLength = 8;
	header.DecoderLength = 0;
	vBuffer[0] = Header_Create( header );
	memcpy( &vBuffer[1], info.pData, info.SizeSource );
}

size_t HuffmanCodes::Compressed_Size() const
{
	return this->CompressedSize;
}

size_t HuffmanCodes::Decompressed_Size() const
{
	return this->DecompressedSize;
}

uint32_t HuffmanCodes::Calc_Comp_Size( sCompressInfo info )
{
	if ( info.SizeSource == 0 || info.pData == nullptr ) return 0;

	bool* ByteCombination = (bool*) calloc( BYTESIZE, sizeof( bool ) );
	for ( uint32_t i = 0; i < info.SizeSource; i++ )
		ByteCombination[info.pData[i]] = true;

	uint8_t OriginalHighestValue = 0;
	uint8_t NewValue = 0;

	for ( size_t i = 0; i < BYTESIZE; i++ )
	{
		if ( ByteCombination[i] )
		{
			NewValue++;
			OriginalHighestValue = i;
		}
	}

	//calculate HuffmanCodes size
	auto BitLenData = Len_Bit( NewValue );
	auto BitLenDecoder = Len_Bit( OriginalHighestValue );

	uint32_t TotalAllocationData = std::ceil( (double) BitLenData * ( info.SizeSource + 1 ) / 8 );
	uint32_t TotalAllocationDecoder = std::ceil( (double) BitLenDecoder * NewValue / 8 );
	return 1 + TotalAllocationData + TotalAllocationDecoder;
}

uint8_t HuffmanCodes::Len_Bit( const uint8_t& num )
{
	return (uint8_t) log2( num ) + 1;
}

bool HuffmanCodes::needDecompression( sCompressHeader header )
{
	return header.DecoderLength != 0;
}

uint8_t HuffmanCodes::Header_Create( const sCompressHeader& header )
{
	#define DATA_REPOSITION header.DataLength << 4
	#define DECODER_REPOSITION header.DecoderLength << 0
	return ( DATA_REPOSITION ) | ( DECODER_REPOSITION );
}

sCompressHeader HuffmanCodes::Header_Read( const uint8_t* pCompressed )
{
	sCompressHeader header;
	header.DataLength = pCompressed[0] >> 4;
	header.DecoderLength = 0x0F & pCompressed[0];
	return header;
}

void HuffmanCodes::Merge_HuffmanCodes( uint8_t* pBuffer, const sCompressHeader& header,  const std::vector<uint8_t>& vCompressed, const std::vector<uint8_t>& vDecoder )
{
	size_t write_index = 0;
	for ( int i = 0; i < 2; i++ )
	{
		size_t write_size;
		const uint8_t* data_ref;
		uint8_t available_bit = BYTELEN;
		uint8_t concat, char_read, get, bit_limit;
		concat = char_read = get = 0;
		if ( i == 0 )
		{
			data_ref = &vCompressed[0];
			bit_limit = header.DataLength;
			write_size = vCompressed.size();
		}
		else
		{
			data_ref = &vDecoder[0];
			bit_limit = header.DecoderLength;
			write_size = vDecoder.size();
		}

		for ( size_t read_index = 0; read_index < write_size; read_index++ )
		{
			if ( available_bit < bit_limit )
			{
				if ( available_bit != (uint8_t) 0 )
				{
					char_read = data_ref[read_index] << get;
					concat = concat | char_read;
				}
				pBuffer[write_index] = concat;
				write_index++;

				get = available_bit == 0 ? bit_limit : bit_limit - available_bit;
				char_read = data_ref[read_index] >> available_bit;
				concat = char_read;
				available_bit = BYTELEN - get;
			}
			else
			{
				char_read = data_ref[read_index] << get;
				get += bit_limit;
				available_bit -= bit_limit;
				concat = concat | char_read;
			}
		}
		pBuffer[write_index] = concat;
		write_index++;
	}
}

void HuffmanCodes::Expand_HuffmanCodes( const uint8_t* pCompressed, const sCompressHeader& header, std::vector<uint8_t>& vBufferData, std::vector<uint8_t>& vBufferDecoder )
{
	uint8_t charRead = 1;
	size_t readIndex = 1;
	uint8_t totalVariation = 0;

	for ( int i = 1; i < 3; i++ )
	{
		uint8_t charlen;
		uint8_t get = 0;
		uint8_t concat = 0;
		uint8_t bitAvailable = BYTELEN;
		std::vector<uint8_t>* refBuffer;
		if ( i == 1 )
		{
			charlen = header.DataLength;
			refBuffer = &vBufferData;
		}
		else
		{
			readIndex++;
			charlen = header.DecoderLength;
			refBuffer = &vBufferDecoder;
			vBufferDecoder.reserve( totalVariation );
		}

		//get data
		while ( true )
		{
			if ( bitAvailable < charlen )
			{
				if ( bitAvailable != 0 )
				{
					get = BYTELEN - bitAvailable;
					concat = pCompressed[readIndex] >> get;
					concat = concat & MaskTemplate[bitAvailable];
					get = charlen - bitAvailable;
				}
				else
				{
					charRead = 0;
					get = charlen;
				}
				readIndex++;
				charRead = pCompressed[readIndex] << bitAvailable;
				charRead = charRead & MaskTemplate[charlen];
				charRead = concat | charRead;
				bitAvailable = BYTELEN - get;
				concat = 0;
			}
			else
			{
				charRead = pCompressed[readIndex] >> get;
				charRead = charRead & MaskTemplate[charlen];
				bitAvailable -= charlen;
				get += charlen;
			}
			if ( i == 1 && charRead == NULL ) break;
			refBuffer->push_back( charRead );
			if ( i == 2 && vBufferDecoder.size() >= totalVariation ) break;
			
			//check how amount of combination
			if ( i == 1 && totalVariation < charRead ) totalVariation = charRead;
		}
	}
}