#include <iostream>
#include <string>
#include "Compressor/Compression.h"
#include "Utilities/Filestream.h"

#define READ_COMPRESSION "R"
#define WRITE_COMPRESSION "W"

int main( int argc, char* argv[] )
{
	uint8_t* pFile = nullptr;
	std::string select = "x";
	
	sFileinfo fi;
	fi.filesize = 0;
	
	while ( true )
	{
		std::cout << "[R]Read Compression" << std::endl;
		std::cout << "[W]Write Compression" << std::endl;
		std::cout << "Select:" << std::endl;
		std::getline( std::cin, select );
		if ( select == READ_COMPRESSION || select == WRITE_COMPRESSION ) break;
	}

	std::string filename;
	while ( fi.filesize == 0 )
	{
		if ( select == READ_COMPRESSION ) std::cout << "Open .bin file:";
		if ( select == WRITE_COMPRESSION ) std::cout << "Open .txt file:";

		std::getline( std::cin, filename );

		if ( select == READ_COMPRESSION ) fi.filename = filename + ".bin";
		if ( select == WRITE_COMPRESSION ) fi.filename = filename + ".txt";

		pFile = (uint8_t*) File_Read_Bin( fi );
		std::cout << "Read Size:" << fi.filesize << "(Bytes)" << std::endl;
	}

	sCompressInfo cp_info;
	cp_info.pData = pFile;
	cp_info.SizeSource = fi.filesize;
	 
	Compression cp;

	if ( select == WRITE_COMPRESSION )
	{
		auto compressed = cp.Compress( cp_info );
		if ( compressed != nullptr )
		{
			std::cout << "Compression Success" << std::endl;
			std::cout << "Size After Compression:" << cp.Compression_Size() << "(Bytes)" << std::endl;
			std::cout << "Save .bin file:";
			std::getline( std::cin, filename );

			fi.filename = filename + ".bin";
			fi.filesize = cp.Compression_Size();
			File_Write_Bin( (char*) compressed, fi );
		}
		else
		{
			std::cout << "Compression not Effective" << std::endl;
		}
	}
	else if ( select == READ_COMPRESSION )
	{
		auto decompressed = cp.Decompress( pFile );
		if ( decompressed != nullptr )
		{
			std::cout << "Decompression Success" << std::endl;
			std::cout << "Size After Decompression:" << cp.Original_Size() << "(Bytes)" << std::endl;
			std::cout << "Save .txt file:";
			std::getline( std::cin, filename );

			fi.filename = filename + ".txt";
			fi.filesize = cp.Original_Size();
			File_Write_Bin( (char*) decompressed, fi );
		}
		else
		{
			std::cout << "File Error!" << std::endl;
		}
	}

	system( "pause" );
	return 0;
}