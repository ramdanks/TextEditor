#include "Command.h"
#include "Timer.h"
#include "Filestream.h"
#include "Err.h"
#include "Merger.h"

#define BENCHMARK_SIZE 1000000u
#define FIRST_SPACING 3
#define SECOND_SPACING 7
#define THIRD_SPACING 25

Timer tm( "", MS, false );
Logging clog( { LEVEL, TIME, MSG } );

void CMD::Init()
{
	CmdList.reserve( 10 );
	//command list:      "cmd"                   argument                             info
	CmdList.push_back( { CMD_HELP,				"",						    "help, show all usable command"									} );
	CmdList.push_back( { CMD_VERSION,			"",						    "version, show current revision of compressor"					} );
	CmdList.push_back( { CMD_ABOUT,				"",						    "about, show information about compressor"						} );
	CmdList.push_back( { CMD_BENCHMARK,			"iteration     -mt",        "benchmark, test your device compression speed"					} );
	CmdList.push_back( { CMD_COMPRESS,			"input         output",     "compress, compress input file and output to compressed format"   } );
	CmdList.push_back( { CMD_DECOMPRESS,		"input         output",	    "decompress, decompress file and return to original format"		} );
	CmdList.push_back( { CMD_READSIZE,			"input",				    "read size, check your file compression and original size"		} );
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

inline bool CMD::File_Exist( const std::string filepath )
{
	struct stat buffer;
	return ( stat( filepath.c_str(), &buffer ) == 0 );
}

void CMD::Handle_Compression( int argc, const char* argv[], bool compress )
{
	Timer timer( "", MS, false );

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
		std::vector<uint8_t> pWrite;
		auto read = Filestream::Read_Bin( fileread );
		THROW_ERR_IF( read.empty(), "File not found or empty!" );

		tm.Tic(); //start timer
		float exec_ms, write_ms;
		if ( compress )
		{
			sCompressInfo ci;
			ci.pData = (uint8_t*) &read[0];
			ci.SizeSource = read.size();

			pWrite = Merger::Compress_Merge( ci, multi_thread );
			THROW_ERR_IF( pWrite.empty(), "Compress to file failed. Compression seems cannot reduce original size!" );

			//stop timer and log
			exec_ms = tm.Toc();
			printf( "Compress and Write file success!" );
			if ( !FileLog.empty() ) clog.Log_File( INFO, "Compress and Write file success!", FileLog );
		}
		else
		{
			pWrite = Merger::Decompress_Merge( read, multi_thread );
			THROW_ERR_IF( pWrite.empty(), "Decompress to file failed. May caused by logic or file integrity error!" );
			
			//stop timer and log
			exec_ms = tm.Toc();
			printf( "Decompress and Write file success!" );
			if ( !FileLog.empty() ) clog.Log_File( INFO, "Decompress and Write file success!", FileLog );
		}
		//use timer when write to files
		tm.Tic();
		Filestream::Write_Bin( (const char*) &pWrite[0], pWrite.size(), filewrite );
		THROW_ERR_IFNOT( File_Exist( filewrite ), "Write file failed. File cannot be saved!" );
		write_ms = tm.Toc();

		//trace result
		float combined_time = timer.Toc();
		float exec_MBps = (double) read.size() / 1000 / exec_ms;
		float write_MBps = (double) pWrite.size() / 1000 / write_ms;

		#define ftos(x) std::to_string(x)
		std::string bandwidth = "MT:" + ftos(multi_thread) + ", Size:" + ftos( pWrite.size() ) + "(Bytes), [C/D]Bandwidth:" + ftos( exec_MBps ) +
								"(MBps), Write:" + ftos( write_MBps ) + "(MBps), CombinedTime:" + ftos( combined_time ) + "(ms)";
		if ( !FileLog.empty() ) clog.Log_File( TRACE, bandwidth, FileLog );
	}
	catch ( Err& error )
	{
		printf( "Unhandled Exception:%s\n", error.Seek() );
		if ( !FileErr.empty() ) error.Log( ERROR, FileErr );
	}
}

void CMD::Handle_Benchmark( int argc, const char* argv[] )
{
	int iteration = 1;
	bool multi_thread = false;
	if ( argc > 2 )
	{
		iteration = atoi( argv[2] );
		if ( argc == 4 && isCommand( argv[3], CMD_MULTITHREAD ) ) multi_thread = true;
	}

	try
	{
		for ( unsigned int i = 0; i < iteration; i++ )
		{
			int* block = (int*) malloc( BENCHMARK_SIZE );
			for ( uint64_t i = 0; i < BENCHMARK_SIZE / sizeof( int ); i++ ) block[i] = rand() % 63;

			Timer tm( "Compression", MS, false );
			sCompressInfo ci;
			ci.pData = (uint8_t*) block;
			ci.SizeSource = BENCHMARK_SIZE;

			tm.Tic();
			auto result_comp = Merger::Compress_Merge( ci, multi_thread );
			auto compression_time = tm.Toc();
			THROW_ERR_IF( result_comp.empty(), "Compression and Merger Failed!" );

			tm.Tic();
			auto result_decomp = Merger::Decompress_Merge( result_comp, multi_thread );
			auto decompression_time = tm.Toc();
			THROW_ERR_IF( result_decomp.empty(), "Decompression and Merger Failed!" );

			double compress_speed = (double) BENCHMARK_SIZE / 1000 / compression_time;
			double decompress_speed = (double) BENCHMARK_SIZE / 1000 / decompression_time;
			printf( "[%u/%u]:Compression:%.2f(MBps), Decompression:%.2f(MBps)\n", i+1, iteration, compress_speed, decompress_speed );

			free( block );
		}
	}
	catch ( Err& error )
	{
		printf( "Unhandled Exception:%s\n", error.Seek() );
		if ( !FileErr.empty() ) error.Log( ERROR, FileErr );
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

			if ( ext == "bin" )
			{
				auto vDecompressed = Merger::Decompress_Merge( read, true );
				THROW_ERR_IF( vDecompressed.empty(), "Cannot identify size. May caused by logic or file integrity error!" );

				uint32_t reduce_size = vDecompressed.size() - read.size();
				printf( "Original:%u(Bytes), Compression:%u(Bytes), Reduced:%u(Bytes)\n", vDecompressed.size(), read.size(), reduce_size );
			}
			else
			{
				sCompressInfo ci;
				ci.pData = (uint8_t*) &read[0];
				ci.SizeSource = read.size();

				auto vCompressed = Merger::Compress_Merge( ci, true );
				THROW_ERR_IF( vCompressed.empty(), "Cannot identify size. May caused by Merger or Compression logic error!" );

				uint32_t reduce_size = read.size() - vCompressed.size();
				printf( "Original:%u(Bytes), Compression:%u(Bytes), Reduced:%u(Bytes)\n", read.size(), vCompressed.size(), reduce_size );
			}
		}
		catch ( Err& error )
		{
			printf( "Unhandled Exception:%s\n", error.Seek() );
			if ( !FileErr.empty() ) error.Log( ERROR, FileErr );
		}
	}

}

void CMD::Handle_Version()
{
	printf( "Compression Revision:%s\n", COMPRESSION_REVISION );
	printf( "Merger Revision:%s\n", MERGER_REVISION );
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