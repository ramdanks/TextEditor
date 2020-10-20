#include "Command.h"
#include "../Utilities/Filestream.h"

#define ERR_FILEPATH	"log/CompError.txt"
#define LOG_FILEPATH	"log/CompLog.txt"

int main( int argc, char* argv[] )
{
	Filestream::Create_Directories( "log" );
	CMD::Init( ERR_FILEPATH, LOG_FILEPATH );

	if ( argc == 1 ) CMD::Handle_Unknown();	
	else             CMD::Recognize( argc, (const char**) argv );

	return 0;
}