#include "Command.h"
#include "Compressor/Compression.h"

int main( int argc, const char* argv[] )
{
	if ( argc == 1 ) CMD::Handle_Unknown();	
	else             CMD::Recognize( argc, argv );
	return 0;
}