#include <iostream>

#include "lib/alloc.h"

#include <cstdlib>

#define USE_DEFAULT 0

int
main( int argc, char* argv[] )
{
    std::ios_base::sync_with_stdio( false );

    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    void * _mem_pool;

    std::size_t _mem_size = 1024*1024*1024;

    alloc::FreeListAllocator * fla = new(defaultAllocator.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    alloc::stl::unordered_multimap< int, int > * mm1 = new(fla->allocate<alloc::stl::unordered_multimap< int, int >>()) alloc::stl::unordered_multimap< int, int >( fla );

    std::cout << "inserting ..." << std::endl;

    for( int i = 0; i < 100000; ++i )
    {
        mm1->insert( std::pair<int,int>(rand()%256,rand()%256) );
    }

    fla->printDebugInfo(std::cout);
    std::cout << std::flush;

    std::cout << "clearing ..." << std::endl;

    mm1->clear_all_memory();

    std::cout << "cleaning up ..." << std::endl;

    fla->deallocate<alloc::stl::unordered_multimap< int, int > >( mm1 );
    defaultAllocator.deallocate<alloc::FreeListAllocator>( fla );

    defaultAllocator.deallocateBlock( _mem_pool );

    defaultAllocator.printDebugInfo(std::cout);

    std::cout << "complete." << std::endl;

    return 0;
}
