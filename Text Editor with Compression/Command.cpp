#include "Command.h"
#include "../Utilities/Timer.h"
#include "../Utilities/Filestream.h"
#include "../Utilities/Err.h"
#include "Compressor/Compression.h"

#define VERBOSE 0
#define BENCHMARK_SIZE 1000000u
#define FIRST_SPACING 3
#define SECOND_SPACING 7
#define THIRD_SPACING 35

Util::Timer tm( "", MS, false );
Util::Logging clog( { FORMAT_LEVEL, FORMAT_TIME, FORMAT_SPACE, FORMAT_MSG } );

void CMD::Init( const std::string& err_filepath, const std::string& log_filepath )
{
	FileErr = err_filepath;
	FileLog = log_filepath;
}

void CMD::Recognize( int argc, const char* argv[] )
{
	if      ( isCommand( argv[1], CMD_HELP ) )			Handle_Help();
	else if ( isCommand( argv[1], CMD_VERSION ) )		Handle_Version();
	else if ( isCommand( argv[1], CMD_ABOUT ) )			Handle_About();
	else if ( isCommand( argv[1], CMD_BENCHMARK ) )		Handle_Benchmark( argc, argv );
	else if ( isCommand( argv[1], CMD_COMPRESS ) )		Handle_Compression( argc, argv, true );
	else if ( isCommand( argv[1], CMD_DECOMPRESS ) )	Handle_Compression( argc, argv, false );
	else if ( isCommand( argv[1], CMD_READSIZE ) )		Handle_Readsize( argv[2] );
	else												Handle_Unknown();
}

bool CMD::isCommand( const char* arg, const char* other )
{
	return !strcmp( arg, other );
}

void CMD::CommandList()
{
	CmdList.reserve( 10 );
	//command list:      "cmd"                   argument                             info
	CmdList.push_back( { CMD_HELP,				"",						            "help, show all usable command" } );
	CmdList.push_back( { CMD_VERSION,			"",						            "version, show current revision of compressor" } );
	CmdList.push_back( { CMD_ABOUT,				"",						            "about, show information about compressor" } );
	CmdList.push_back( { CMD_BENCHMARK,			"iteration    <-mt>",               "benchmark, test your device compression speed" } );
	CmdList.push_back( { CMD_COMPRESS,			"input        output    <-mt>",     "compress, compress input file and output to compressed format" } );
	CmdList.push_back( { CMD_DECOMPRESS,		"input        output    <-mt>",	    "decompress, decompress file and return to original format" } );
	CmdList.push_back( { CMD_READSIZE,			"input",				            "read size, check your file compression and original size" } );
}

inline bool CMD::File_Exist( const std::string filepath )
{
	struct stat buffer;
	return ( stat( filepath.c_str(), &buffer ) == 0 );
}

void CMD::Handle_Compression( int argc, const char* argv[], bool compress )
{
	bool multi_thread = false;
	if ( argc == 5 && isCommand( argv[4], CMD_MULTITHREAD ) ) multi_thread = true;
	else if ( argc != 4 )
	{
		Handle_Unknown();
		return;
	}
	
	const char* fileread = argv[2];
	const char* filewrite = argv[3];

	try
	{
		auto read = Filestream::Read_Bin( fileread );
		THROW_ERR_IFEMPTY( read, "Cannot perform with an empty file" );

		tm.toc(); //start timer
		sCompressedInfo ci{};
		float exec_ms, write_ms;
		if ( compress )
		{
			ci = Compression::Compress( &read[0], read.size(), multi_thread );
			THROW_ERR_IFNULLPTR( ci.pData, "Compression failed" );

			//stop timer and log
			exec_ms = tm.toc();
			Filestream::Write_Bin( (const char*) ci.pData, ci.CompressedSize, filewrite );
		}
		else
		{
			ci = Compression::Decompress( &read[0], read.size(), multi_thread );
			THROW_ERR_IFNULLPTR( ci.pData, "Decompress failed" );
		
			//stop timer and log
			exec_ms = tm.toc();
			Filestream::Write_Bin( (const char*) ci.pData, ci.OriginalSize, filewrite );
		}
		
		THROW_ERR_IFNOT( File_Exist( filewrite ), "Write failed" );
	}
	catch ( Util::Err& error )
	{
		printf( "Err:%s\n", error.Seek().c_str() );
		if ( !FileErr.empty() ) Util::Log( error, FileErr );
		return;
	}
	if ( compress ) puts( "Compress and Write file success!" );
	else            puts( "Decompress and Write file success!" );
}

void CMD::Handle_Benchmark( int argc, const char* argv[] )
{
	int iteration = 1;
	bool multi_thread = false;
	if ( argc > 2 )
	{
		iteration = atoi( argv[2] );
		if (argc == 4 && isCommand(argv[3], CMD_MULTITHREAD))
		{
			multi_thread = true;
			puts("Multi-Thread Enabled");
		}
	}

	try
	{
		for ( unsigned int i = 0; i < iteration; i++ )
		{
			uint8_t* block = new uint8_t[BENCHMARK_SIZE];
;
			Util::Timer tm( "Compression", MS, false );

			tm.tic();
			auto cpi = Compression::Compress( block, BENCHMARK_SIZE, multi_thread );
			auto compression_time = tm.toc();
			THROW_ERR_IFNULLPTR( cpi.pData, "Compression Failed!" );

			tm.tic();
			auto dpi = Compression::Decompress( cpi, multi_thread );
			auto decompression_time = tm.toc();
			THROW_ERR_IFNULLPTR( dpi.pData, "Decompression Failed!" );

			double compress_speed = (double) BENCHMARK_SIZE / 1024. / compression_time;
			double decompress_speed = (double) BENCHMARK_SIZE / 1024. / decompression_time;

			delete[] block;
			delete[] cpi.pData;
			delete[] dpi.pData;

			printf( "[%u/%u]:Compression:%.2f(MBps), Decompression:%.2f(MBps)\n", i+1, iteration, compress_speed, decompress_speed );
		}
	}
	catch ( Util::Err& error )
	{
		printf( "Unhandled Exception:%s\n", error.Seek() );
		if ( !FileErr.empty() ) Util::Log( error, FileErr );
	}
}

void CMD::Handle_Readsize( const char* fileread )
{
	if ( fileread == nullptr )
	{
		Handle_Unknown();
		return;
	}
	else
	{
	}

}

void CMD::Handle_Version()
{
	printf( "HuffmanCodes Revision:%s\n", HUFFMAN_REVISION );
	printf( "Compression Revision:%s\n", COMPRESSION_REVISION );
}

void CMD::Handle_Unknown()
{
	printf( "unknown command, use -h for help\n" );
}

void CMD::Handle_About()
{
	std::string about;
	about += "Compression based on Huffman Codes Algorithm\n";
	about += "Data Segmentation:\n";
	about += "Header[1](Bytes) | Data[n](Bytes), Decoder[n](Bytes)\n";
	printf( "%s", about.c_str() );
}

void CMD::Handle_Help()
{
	if ( CmdList.empty() ) CommandList();
	printf( "Options:\n" );
	for ( auto i : CmdList )
	{
		std::string dummy_space;
		int second_spacing = SECOND_SPACING - i.cmd.size();
		int third_spacing = THIRD_SPACING - i.arg.size();

		if ( second_spacing < 0 ) second_spacing = 1;
		if ( third_spacing < 0 ) third_spacing = 1;

		for ( int align = 0; align < FIRST_SPACING; align++ ) dummy_space += " ";
		for ( int align = 0; align < second_spacing; align++ ) i.cmd += " ";
		for ( int align = 0; align < third_spacing; align++ ) i.arg += " ";

		printf( "%s%s%s%s\n", dummy_space.c_str(), i.cmd.c_str(), i.arg.c_str(), i.info.c_str() );
	}
}