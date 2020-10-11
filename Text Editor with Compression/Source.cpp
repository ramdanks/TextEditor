#include "Utilities/Command.h"
#include "Utilities/Filestream.h"
#include "Utilities/Merger.h"

#define ERR_FILEPATH	"Log/Error.txt"
#define LOG_FILEPATH	"Log/Log.txt"

int main( int argc, char* argv[] )
{
	Filestream::Create_Directories( "Log" );
	CMD::Init( ERR_FILEPATH, LOG_FILEPATH );

	if ( argc == 1 ) CMD::Handle_Unknown();	
	else             CMD::Recognize( argc, (const char**) argv );

	return 0;
}