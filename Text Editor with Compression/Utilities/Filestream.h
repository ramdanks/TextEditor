#pragma once
#include <fstream>

struct sFileinfo
{
	std::string filename;
	size_t filesize;
};

void File_Write_Bin( char* fp, const sFileinfo& info )
{
	auto myfile = std::fstream( info.filename, std::ios::out | std::ios::binary );
	if ( myfile )
	{
		if ( info.filesize > 0 )
		{
			myfile.write( fp, info.filesize );
		}
		myfile.close();
	}
}

char* File_Read_Bin( sFileinfo& info )
{
	char* fp = nullptr;
	auto myfile = std::fstream( info.filename, std::ios::in | std::ios::binary );
	if ( myfile )
	{
		myfile.seekg( 0, myfile.end );
		info.filesize = myfile.tellg();
		if ( info.filesize > 0 )
		{
			myfile.seekg( 0 );
			fp = (char*) malloc( info.filesize * sizeof(char) );
			myfile.read( fp, info.filesize );
		}
		myfile.close();
	}
	return fp;
}