#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#define TO_STR(x) std::to_string(x)

class Filestream
{
	Filestream() {}
public:
	Filestream( const Filestream& other ) = delete;
	Filestream operator = ( const Filestream& other ) = delete;

	static bool Create_Directories( const std::string& path )
	{
		return std::filesystem::create_directories( path );
	}

	inline static bool Delete_File( const std::string& filepath )
	{
		return std::remove( filepath.c_str() ) == 0;
	}

	inline static void Rename_File( const std::string& after, const std::string& filepath )
	{
		std::filesystem::rename( filepath, after );
	}

	static void Delete_Dir_File( const std::string& dir_path )
	{
		for ( const auto& entry : std::filesystem::directory_iterator( dir_path ) )
			std::filesystem::remove_all( entry.path() );
	}

	static bool Is_Exist( const std::string& filepath )
	{
		struct stat buffer;
		return stat( filepath.c_str(), &buffer ) == 0;
	}

	static void Append_Text( const std::string& string, const std::string& filepath )
	{
		auto myfile = std::fstream( filepath, std::ios::app );
		if ( myfile )
		{
			myfile.write( string.c_str(), string.size() );
		}
		myfile.close();
	}

	static std::vector<std::string> File_List( const std::string& filepath )
	{
		std::vector<std::string> list;
		for ( const auto& entry : std::filesystem::directory_iterator( filepath ) )
			list.push_back( entry.path().string() );
		return list;
	}

	static void Write_Bin( const char* fp, size_t filesize, const std::string& filepath )
	{
		auto myfile = std::fstream( filepath, std::ios::out | std::ios::binary );
		if ( myfile )
		{
			if ( filesize > 0 ) myfile.write( fp, filesize );
			myfile.close();
		}
	}

	static std::vector<uint8_t> Read_Bin( const std::string& filepath )
	{
		std::vector<uint8_t> buf;
		auto myfile = std::fstream( filepath, std::ios::in | std::ios::binary );
		if ( myfile )
		{
			myfile.seekg( 0, myfile.end );
			auto filesize = myfile.tellg();
			if ( filesize > 0 )
			{
				myfile.seekg( 0 );
				buf.resize( filesize );
				myfile.read( (char*) &buf[0], filesize );
			}
			myfile.close();
		}
		return buf;
	}

	static std::string FileExtension( const std::string& s )
	{
		size_t i = s.rfind( '.', s.length() );
		if ( i != std::string::npos ) return( s.substr( i + 1, s.length() - i ) );
		return std::string();
	}

	static std::vector<std::string> ParseString( const std::string& str, char delimiter )
	{
		std::stringstream ss( str );
		std::string item;
		std::vector<std::string> elems;
		while ( std::getline( ss, item, delimiter ) )
			elems.push_back( item );

		return elems;
	}

	static std::string getFileName( std::string filePath, bool withExtension = true, char seperator = '\\' )
	{
		// Get last dot position
		std::size_t dotPos = filePath.rfind( '.' );
		std::size_t sepPos = filePath.rfind( seperator );
		if ( sepPos != std::string::npos )
			return filePath.substr( sepPos + 1, filePath.size() - ( withExtension || dotPos != std::string::npos ? 1 : dotPos ) );

		return "";
	}

	static std::string GetLastModified( std::string filepath )
	{
		struct stat attrib;
		stat( filepath.c_str(), &attrib );
		struct tm* time = gmtime( &attrib.st_mtime );
		char* asctime = std::asctime( time );
		return std::string( asctime, strlen( asctime ) - 1 );
	}

	static std::string GetLastCreated( std::string filepath )
	{
		struct stat attrib;
		stat( filepath.c_str(), &attrib );
		struct tm* time = gmtime( &attrib.st_ctime );
		char* asctime = std::asctime( time );
		return std::string( asctime, strlen( asctime ) - 1 );
	}

	static size_t GetFileSize( std::string filepath )
	{
		auto myfile = std::fstream( filepath, std::ios::in | std::ios::binary );
		myfile.seekg( 0, myfile.end );
		return size_t( myfile.tellg() );
	}

	struct sCompressedInfo
	{
		size_t ClusterSize;
		size_t AddressingSize;
		size_t CompressedSize;
		size_t OriginalSize;
	};

	static sCompressedInfo GetCompressedInfo( std::string filepath )
	{
		sCompressedInfo info;
		auto vCompressed = Read_Bin( filepath );
		info.ClusterSize = 0;
		info.CompressedSize = vCompressed.size();
		info.AddressingSize = vCompressed[0];

		if ( info.AddressingSize == 0 ) return info;

		constexpr uint32_t MaskBytes[] = { 0x0, 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };
		for ( uint32_t readIndex = 1; ; readIndex += info.AddressingSize )
		{
			uint32_t readAddress = *(uint32_t*) &vCompressed[readIndex];
			readAddress = readAddress & MaskBytes[info.AddressingSize];
			if ( readAddress == 0 ) break;
			info.ClusterSize++;
		}
		return info;
	}
};