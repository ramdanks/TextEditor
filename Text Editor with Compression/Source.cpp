#include "Command.h"
#include "../Utilities/Filestream.h"

#define ERR_FILEPATH	"Log/CompError.txt"
#define LOG_FILEPATH	"Log/CompLog.txt"

int main( int argc, char* argv[] )
{
	Filestream::Create_Directories( "Log" );
	CMD::Init( ERR_FILEPATH, LOG_FILEPATH );

	if ( argc == 1 ) CMD::Handle_Unknown();	
	else             CMD::Recognize( argc, (const char**) argv );

	return 0;
}