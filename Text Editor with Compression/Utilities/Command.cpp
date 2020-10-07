#include "Command.h"
#include "Timer.h"
#include "..\Compressor\Compression.h"
#include "Filestream.h"
#include "Err.h"

#define BENCHMARK_SIZE 1000000u
#define FIRST_SPACING 3
#define SECOND_SPACING 5
#define THIRD_SPACING 20

Timer tm( "", MS, false );
Logging clog( { LEVEL, TIME, MSG } );

void CMD::Init()
{
	CmdList.reserve( 10 );
	//command list:      "cmd"                   argument                             info
	CmdList.push_back( { CMD_HELP,				"",						"help, show all usable command"									} );
	CmdList.push_back( { CMD_VERSION,			"",						"version, show current revision of compressor"					} );
	CmdList.push_back( { CMD_ABOUT,				"",						"about, show information about compressor"						} );
	CmdList.push_back( { CMD_BENCHMARK,			"iteration",            "benchmark, test your device compression speed"					} );
	CmdList.push_back( { CMD_COMPRESS,			"input output",		    "compress, compress input file and output to compressed format" } );
	CmdList.push_back( { CMD_DECOMPRESS,		"input output",		    "decompress, decompress file and return to original format"		} );
	CmdList.push_back( { CMD_READSIZE,			"input",				"read size, check your file compression and original size"		} );
}

void CMD::Init( const std::string& err_filepath, const std::string& log_filepath )
{
	Init();
	FileErr = err_filepath;
	FileLog = log_filepath;
}

void CMD::Recognize( int argc, const char* argv[] )
{
	if ( isCommand( argv[1], CMD_HELP ) )				Handle_Help();
	else if ( isCommand( argv[1], CMD_VERSION ) )		printf( "Compressor Revision:%s\n", COMPRESSION_REVISION );
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

inline bool CMD::File_Exist( const std::string filepath )
{
	struct stat buffer;
	return ( stat( filepath.c_str(), &buffer ) == 0 );
}

void CMD::Handle_Compression( int argc, const char* argv[], bool compress )
{
	if ( argc != 4 )
	{
		Handle_Unknown();
		return;
	}
	
	const char* fileread = argv[2];
	const char* filewrite = argv[3];
	try
	{
		std::vector<char> read = Filestream::Read_Bin( fileread );
		THROW_ERR_IF( read.empty(), "File not found or empty!" );

		tm.Tic(); //start timer
		float ms;
		Compression cp;
		if ( compress )
		{
			sCompressInfo ci;
			ci.pData = (uint8_t*) &read[0];
			ci.SizeSource = read.size();

			auto pCompressed = cp.Compress( ci );
			THROW_ERR_IFNULLPTR( pCompressed, "Compress to file failed. Compression seems cannot reduce original size!" );

			Filestream::Write_Bin( (const char*) pCompressed, cp.Compression_Size(), filewrite );
			THROW_ERR_IFNOT( File_Exist( filewrite ), "Write file failed. File cannot be saved!" );

			//stop timer and log
			ms = tm.Toc();
			clog.Log( INFO, "Compress to file success" );
			if ( !FileLog.empty() ) clog.Log_File( INFO, "Compress to file sucess", FileLog );
		}
		else
		{
			auto pDecompressed = cp.Decompress( (uint8_t*) &read[0] );
			THROW_ERR_IFNULLPTR( pDecompressed, "Decompress to file failed. May caused by logic or file integrity error!" );
			
			Filestream::Write_Bin( (const char*) pDecompressed, cp.Original_Size(), filewrite );
			THROW_ERR_IFNOT( File_Exist( filewrite ), "Write file failed. File cannot be saved!" );

			//stop timer and log
			ms = tm.Toc();
			clog.Log( INFO, "Decompress to file success" );
			if ( !FileLog.empty() ) clog.Log_File( INFO, "Decompress to file sucess", FileLog );
		}

		//trace result
		float MBps = (double) read.size() / 1000 / ms;
		std::string speed = "Time:" + std::to_string( ms ) + "(ms), " + "Speed:" + std::to_string( MBps ) + "(MBps)";
		if ( !FileLog.empty() ) clog.Log_File( TRACE, speed, FileLog );
	}
	catch ( Err& error )
	{
		clog.Log( ERROR, error.Seek() );
		if ( !FileErr.empty() ) error.Log( ERROR, FileErr );
	}
}

void CMD::Handle_Benchmark( int argc, const char* argv[] )
{
	int iteration = 0;
	if ( argc == 2 )	iteration = 1;
	else				iteration = atoi( argv[2] );

	for ( unsigned int i = 0; i < iteration; i++ )
	{
		int* block = (int*) malloc( BENCHMARK_SIZE );
		for ( uint64_t i = 0; i < BENCHMARK_SIZE / sizeof( int ); i++ ) block[i] = rand() % 63;

		Timer tm( "Compression", MS, false );
		Compression cp;
		sCompressInfo ci;
		ci.pData = (uint8_t*) block;
		ci.SizeSource = BENCHMARK_SIZE;

		tm.Tic();
		auto result_comp = cp.Compress( ci );
		auto compression_time = tm.Toc();

		tm.Tic();
		auto result_decomp = cp.Decompress( result_comp );
		auto decompression_time = tm.Toc();

		double compress_speed = (double) BENCHMARK_SIZE / 1000 / compression_time;
		double decompress_speed = (double) BENCHMARK_SIZE / 1000 / decompression_time;
		printf( "[%u/%u]:Compression:%.2f(MBps), Decompression:%.2f(MBps)\n", i+1, iteration, compress_speed, decompress_speed );

		free( block );
		free( result_comp );
		free( result_decomp );
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
		try
		{
			auto ext = Filestream::FileExtension( fileread );
			THROW_ERR_IF( ext.empty(), "Unspecified format. Cannot identify the file!" );

			auto read = Filestream::Read_Bin( fileread );
			THROW_ERR_IF( read.empty(), "File not found or empty!" );

			Compression cp;
			if ( ext == "txt" )
			{
				sCompressInfo ci;
				ci.pData = (uint8_t*) &read[0];
				ci.SizeSource = read.size();

				auto pCompressed = cp.Compress( ci );
				THROW_ERR_IFNULLPTR( pCompressed, "Cannot identify size. Compression seems cannot reduce original size!" );

				uint32_t reduce_size = read.size() - cp.Compression_Size();
				printf( "Original Size:%u(Bytes), Compression Size:%u(Bytes), Reduce:%u(Bytes)\n", read.size(), cp.Compression_Size(), reduce_size );
			}
			else if ( ext == "bin" )
			{
				auto pDecompressed = cp.Decompress( (uint8_t*) &read[0] );
				THROW_ERR_IFNULLPTR( pDecompressed, "Cannot identify size. May caused by logic or file integrity error!" );

				uint32_t reduce_size = cp.Original_Size() - read.size();
				printf( "Original Size:%u(Bytes), Compression Size:%u(Bytes), Reduce:%u(Bytes)\n", cp.Original_Size(), read.size(), reduce_size );
			}
			else THROW_ERR( "Format " + ext + " is not supported!" );
		}
		catch ( Err& error )
		{
			clog.Log( ERROR, error.Seek() );
			if ( !FileErr.empty() ) error.Log( ERROR, FileErr );
		}
	}

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