#include <vector>
#include <future>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "../Utilities/Timer.h"
#include "../Text Editor with Compression/Compressor/Compression.h"

using namespace std;

std::vector<uint8_t> read_bin( const filesystem::path& filepath )
{
    std::vector<uint8_t> buf;
    auto myfile = std::fstream( filepath, std::ios::in | std::ios::binary );
    if ( myfile )
    {
        myfile.seekg( 0, myfile.end );
        auto filesize = myfile.tellg();
        if ( filesize )
        {
            myfile.seekg( 0 );
            buf.resize( filesize );
            myfile.read( (char*) &buf[0], filesize );
        }
    }
    return buf;
}

bool worker( const filesystem::path& filepath )
{
    if ( filepath.empty() )
    {
        printf( "Empty Filepath?\n" );
        return true;
    };

    vector<uint8_t> vRead;
    vRead = read_bin( filepath );

    sCompressInfo ci;
    ci.pData = &vRead[0];
    ci.SizeSource = vRead.size();
    auto vComp = Compression::Compress_Merge( ci, false );
    auto vRes = Compression::Decompress_Merge( vComp, false );

    if ( vRead.size() == vRes.size()
         && memcmp( &vRes[0], &vRead[0], vRead.size() ) == 0 )
    {
        wprintf( L"[PASS]:%ls\n", filepath.c_str() );
        return true;
    }
    wprintf( L"[FAIL]:%ls\n", filepath.c_str() );
    return false;
}

int main( int argc, const char** argv )
{
    TIMER_FUNCTION( timer, MS, false );

    if ( argc != 2 )
    {
        puts( "Please specify directory path." );
        return -1;
    }

    vector<filesystem::path> filepaths;
    for ( const auto& entry : filesystem::directory_iterator( argv[1] ) )
        filepaths.emplace_back( entry.path() );

    if ( filepaths.empty() )
    {
        printf( "Cannot find a file in:%s\n", argv[1] );
        return -2;
    }

    vector<future<bool>> futures;
    futures.reserve( filepaths.size() );

    puts( "Running Worker!" );
    for ( auto& fp : filepaths )
        futures.emplace_back( async( launch::async, worker, fp ) );

    uint32_t fail = 0;
    for ( auto& ft : futures )
        if ( !ft.get() ) fail++;

    printf( "Fail:%u, Accuracy:%.2f%%\n", fail, ( (float) filepaths.size() - fail ) / filepaths.size() * 100 );

    return 0;
}