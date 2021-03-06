#pragma once
#include <vector>
#include <string>

typedef struct sCMD
{
	std::string cmd;
	std::string arg;
	std::string info;
} sCMD;

//command list
static std::vector<sCMD> CmdList;
static std::string FileErr;
static std::string FileLog;

//singleton
class CMD
{
public:
	CMD() = delete;
	CMD( const CMD& other ) = delete;
	CMD operator = ( const CMD& other ) = delete;

	//initialize cmd list, no logging and error logging will be reported.
	//initialize cmd list, logging and error logging will be reported to the desired filepath.
	static void Init( const std::string& err_filepath, const std::string& log_filepath );
	static void Recognize( int argc, const char* argv[] );
	static bool isCommand( const char* arg, const char* other );
	static void Handle_Unknown();

private:
	static void CommandList();
	static inline bool File_Exist( const std::string filepath );
	
	static void Handle_Compression( int argc, const char* argv[], bool compress );
	static void Handle_Benchmark( int argc, const char* argv[] );
	static void Handle_Readsize( const char* fileread );
	static void Handle_Version();
	static void Handle_Help();
	static void Handle_About();
};

#define CMD_BENCHMARK	    "-b"
#define CMD_BENCHMARK_RAW   "-br"
#define CMD_VERSION		    "-v"
#define CMD_HELP		    "-h"
#define CMD_ABOUT		    "-a"

#define CMD_COMPRESS	    "-c"
#define CMD_COMPRESS_RAW    "-cr"
#define CMD_DECOMPRESS	    "-d"
#define CMD_DECOMPRESS_RAW  "-dr"
#define CMD_READSIZE	    "-r"

#define CMD_MULTITHREAD     "-mt"
#define CMD_SINGLETHREAD    "-st"