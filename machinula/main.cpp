#include <iostream>

#include "lib/alloc.h"

int
main( int argc, char* argv[] )
{
    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    alloc::stl::multimap< int, int > mm1( &defaultAllocator );

    mm1.insert( std::pair<int,int>(1,0) );
    mm1.insert( std::pair<int,int>(1,0) );
    mm1.insert( std::pair<int,int>(1,0) );
    mm1.insert( std::pair<int,int>(1,0) );
    mm1.insert( std::pair<int,int>(1,0) );

    defaultAllocator.printDebugInfo(std::cout);

    return 0;
}
