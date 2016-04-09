#include <iostream>

#include "lib/alloc.h"

#include <cstdlib>

#define USE_DEFAULT 0

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    void * _mem_pool;

    std::size_t _mem_size = 1024*1024*1024;

    alloc::FreeListAllocator * fla = new(defaultAllocator.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );


    {

    }


    fla->printDebugInfo( std::cout );
    std::cout << std::flush;

    std::cout << "cleaning up ... fla" << std::endl;
    defaultAllocator.deallocate<alloc::FreeListAllocator>( fla );

    std::cout << "cleaning up ... _mem_pool" << std::endl;
    defaultAllocator.deallocateBlock( _mem_pool );

    defaultAllocator.printDebugInfo( std::cout );

    std::cout << "complete." << std::endl;

    return 0;
}
