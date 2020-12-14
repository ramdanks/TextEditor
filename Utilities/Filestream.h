#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <time.h>

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

	static bool Exist( const char* filepath )
	{
		struct stat buffer;
		return stat( filepath, &buffer ) == 0;
	}

	static bool Exist( const std::string& filepath )
	{
		struct stat buffer;
		return stat( filepath.c_str(), &buffer ) == 0;
	}

	static void Append_Text( const std::string& string, const std::string& filepath )
	{
		auto myfile = std::fstream( filepath, std::ios::app );
		if ( myfile ) myfile.write( string.c_str(), string.size() );
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
			myfile.write( fp, filesize );
			myfile.close();
		}
	}

	static bool adjust_seperator(const char* fp)
	{
		for ( const char* i = fp; i != NULL; i++ )
			if ( *i == '\\' && *( i + 1 ) != '\\' ) return false;
			else if ( *i == '/' ) return true;
		return true;
	}

	static std::vector<uint8_t> Read_Bin( const char* fp )
	{
		std::vector<uint8_t> buf;
		auto myfile = std::fstream( fp, std::ios::in | std::ios::binary );
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

	static std::vector<uint8_t> Read_Bin( const std::string& filepath )
	{
		return Read_Bin( filepath.c_str() );
	}

	static std::string FileExtension( const std::string& s )
	{
		size_t i = s.rfind( '.', s.length() );
		if ( i != std::string::npos ) return( s.substr( i + 1, s.length() - i ) );
		return std::string();
	}

	static std::vector<std::string> ParseString( const std::string& str, char delimiter, uint32_t max_parse = 0 )
	{
		if ( str.empty() ) return std::vector<std::string>();

		std::stringstream ss( str );
		std::string item;
		std::vector<std::string> elems;
		if ( max_parse == 0 )
		{
			while ( std::getline( ss, item, delimiter ) )
				elems.push_back( item );
		}
		else
		{
			for ( uint32_t i = 0; i < max_parse; i++ )
			{
				std::getline( ss, item, delimiter );
				elems.push_back( item );
			}
		}
		return elems;
	}

	static std::string GetEOLChar( const std::string& str )
	{
		for ( uint32_t i = 0; i < str.size(); i++ )
			if ( str[i] == '\r' )
				if ( i + 1 < str.size() && str[i + 1] == '\n' ) return "\r\n";
				else return "\r";
			else if ( str[i] == '\n' ) return "\n";
		return std::string();
	}

	static std::vector<std::string> ParseNewline( const std::string& str )
	{
		auto eol = GetEOLChar( str );
		if ( eol.empty() ) return std::vector<std::string>();

		std::vector<std::string> parse;
		if ( eol == "\r\n" )
		{
			parse = ParseString( str, eol[1] );
			for ( auto& i : parse ) i.pop_back();
		}
		else parse = ParseString( str, eol[0] );

		return parse;
	}

	static std::string GetFileName( std::string filePath, bool withExtension = true, char seperator = '\\' )
	{
		// Get last dot position
		std::size_t dotPos = filePath.rfind( '.' );
		std::size_t sepPos = filePath.rfind( seperator );
		if ( sepPos != std::string::npos )
			return filePath.substr( sepPos + 1, filePath.size() - ( withExtension || dotPos != std::string::npos ? 1 : dotPos ) );

		return "";
	}

	static std::string GetLastModified( const std::string& filepath )
	{
		tm* time = new tm;
		struct stat attrib;
		stat( filepath.c_str(), &attrib );
		gmtime_s( time, &attrib.st_mtime );
		char asctime[64];
		asctime_s( asctime, sizeof asctime, time );
		return std::string( asctime );
	}

	static std::string GetLastCreated( const std::string& filepath )
	{
		tm* time = new tm;
		struct stat attrib;
		stat( filepath.c_str(), &attrib );
		gmtime_s( time, &attrib.st_mtime );
		char asctime[64];
		asctime_s( asctime, sizeof asctime, time );
		return std::string( asctime );
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