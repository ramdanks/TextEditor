#include "HuffmanCodes.h"
#include <stdlib.h>
#include <cmath>

#define EMPTY_CPINFO { nullptr, 0 }
constexpr uint8_t MaskTemplate[9] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

sCompressInfo HuffmanCodes::Compress( const uint8_t* src, size_t size, bool force )
{
	sCompressInfo res;
	res.pData = nullptr;
	res.Size = 0;

	if (size == 0 || src == nullptr) return res;

	bool ByteCombination[BYTESIZE];
	memset(ByteCombination, 0x00, BYTESIZE);

	//find amount of byte combination in source data
	for ( uint64_t i = 0; i < size; i++ )
		ByteCombination[src[i]] = true;

	//based on amount of byte combination in sourcedata, create new value and store old value
	std::vector<uint8_t> OldValueTable;
	OldValueTable.reserve(BYTESIZE);
	uint8_t NewValueTable[BYTESIZE];
	memset(NewValueTable, 0x00, BYTESIZE);

	uint8_t NewValue = 0;
	for ( size_t i = 0; i < BYTESIZE; ++i )
	{
		if ( ByteCombination[i] )
		{
			//the reason NewValue starts at "1" is because we need "0" to represent a terminating pointer
			//when we merge the compressed value, at the end we put "0" to indicate the end of data
			//this trick is to seperates or identify where's the compressed data and the decoder, so no need to memorize location
			NewValueTable[i] = ++NewValue;
			OldValueTable.emplace_back(i);
			if (NewValue > 0x7F) break;
		}
	}

	// newvalue needs 8 bit so compression isn't going to help
	if (NewValue > 0x7F)
	{ 
		if (!force) return res;
		return HuffmanCodes::Uncompressed(src, size);
	}

	// calculate HuffmanCodes size
	auto BitLenData = Len_Bit( NewValue ); //find bit length to represent Highest number of new value
	auto BitLenDecoder = Len_Bit( OldValueTable.back() ); //find bit length to represent Highest number of old value

	// NewValue represent how much combination of bytes in compressed format
	uint64_t TotalAllocationData = std::ceil( (double) BitLenData * (size + 1) / 8 ); //find amount of bytes to store compressed data
	uint64_t TotalAllocationDecoder = std::ceil( (double) BitLenDecoder * NewValue / 8 ); //find amount of bytes to store old (original) data
	uint64_t TotalAllocationCombined = 1 + TotalAllocationData + TotalAllocationDecoder; //header need 1 byte

	// if doesnt reduce original size then compression isn't going to help
	if (TotalAllocationCombined >= size)
	{
		if (!force) return res;
		return HuffmanCodes::Uncompressed(src, size);
	}

	//allocate compressed size
	res.Size  = TotalAllocationCombined;
	res.pData = new uint8_t[TotalAllocationCombined];


	// TODO: i think we can elliminate this
	//create buffer for HuffmanCodes data based on original order
	uint8_t* BufferCompressedData = new uint8_t[size + 1]; //additional bytes for null pointer
	BufferCompressedData[size] = 0; //add terminating pointer at the end
	for ( size_t i = 0; i < size; i++ )
		BufferCompressedData[i] = NewValueTable[src[i]];
	
	//create header
	sCompressHeader header;
	header.DataLength = BitLenData;
	header.DecoderLength = BitLenDecoder;
	res.pData[0] = Header_Create( header );

	//combine header, compressed value, decoder table to pCompressed buffer.
	Merger( &res.pData[1], BitLenData, BufferCompressedData, size + 1 );
	Merger( &res.pData[1 + TotalAllocationData], BitLenDecoder, &OldValueTable[0], OldValueTable.size() );

	delete[] BufferCompressedData;

	return res;
}

sCompressInfo HuffmanCodes::Decompress( const sCompressInfo& src )
{
	return Decompress( src.pData, src.Size );
}

sCompressInfo HuffmanCodes::Decompress( const uint8_t* src, size_t size )
{
	sCompressHeader header = Header_Read( src );
	if ( !needDecompression( header ) )
	{
		//the actual size if (size-1) because we ignore HuffmanCodes header which cost 1 byte.
		uint8_t* pDecompressed = new uint8_t[size - 1];
		memcpy( pDecompressed, &src[1], size - 1 );
		return { pDecompressed, size - 1 };
	}
	return Decompress_Pointer( src );
}

sCompressInfo HuffmanCodes::Decompress_Pointer( const uint8_t* pCompressed )
{
	if ( !pCompressed ) return { nullptr, 0 };

	uint8_t* pOriginal = nullptr;
	sCompressHeader header = Header_Read( pCompressed );

	std::vector<uint8_t> vBufferData, vBufferDecoder;
	Expand_HuffmanCodes( pCompressed, header, vBufferData, vBufferDecoder );

	//allocate, original size is determined
	size_t size = vBufferData.size();
	pOriginal = new uint8_t[size];

	//translation with decoder table
	for ( size_t i = 0; i < size; i++ )
	{
		auto val = vBufferData[i];
		pOriginal[i] = vBufferDecoder[val-1];
	}
	return { pOriginal, size };
}

sCompressInfo HuffmanCodes::Uncompressed( const uint8_t* src, size_t size )
{
	sCompressInfo res;
	res.Size = 1 + size;
	res.pData = (uint8_t*) malloc( 1 + size );

	sCompressHeader header;
	header.DataLength = 8;
	header.DecoderLength = 0;
	res.pData[0] = Header_Create( header );
	memcpy( &res.pData[1], src, size );

	return res;
}

size_t HuffmanCodes::Calc_Comp_Size( const uint8_t* src, size_t size )
{
	if ( size == 0 || src == nullptr ) return 0;

	bool ByteCombination[BYTESIZE];
	memset( ByteCombination, 0x00, BYTESIZE );

	for ( uint32_t i = 0; i < size; i++ )
		ByteCombination[src[i]] = true;

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

	size_t TotalAllocationData = std::ceil( (double) BitLenData * ( size + 1 ) / 8 );
	size_t TotalAllocationDecoder = std::ceil( (double) BitLenDecoder * NewValue / 8 );
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

void HuffmanCodes::Merger( uint8_t* pBuf, uint8_t bitlen, uint8_t* lookup, size_t size )
{
	size_t write_index = 0;
	uint8_t available_bit = BYTELEN;
	uint8_t concat, char_read, get, bit_limit;
	concat = char_read = get = 0;

	for ( size_t read_index = 0; read_index < size; read_index++ )
	{
		if ( available_bit < bitlen )
		{
			if ( available_bit != (uint8_t) 0 )
			{
				char_read = lookup[read_index] << get;
				concat = concat | char_read;
			}
			pBuf[write_index] = concat;
			write_index++;

			get = available_bit == 0 ? bitlen : bitlen - available_bit;
			char_read = lookup[read_index] >> available_bit;
			concat = char_read;
			available_bit = BYTELEN - get;
		}
		else
		{
			char_read = lookup[read_index] << get;
			get += bitlen;
			available_bit -= bitlen;
			concat = concat | char_read;
		}
	}
	pBuf[write_index] = concat;
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