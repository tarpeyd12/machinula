#include <iostream>

#include "lib/alloc.h"

#include <cstdlib>

#define USE_DEFAULT 0

int
main( int argc, char* argv[] )
{
    std::ios_base::sync_with_stdio( false );

    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    #if !USE_DEFAULT
    void * _mem_pool;

    std::size_t _mem_size = 1024*1024*1024;

    alloc::FreeListAllocator * fla = new(defaultAllocator.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );
    //alloc::LinearAllocator * fla = new(defaultAllocator.allocate<alloc::LinearAllocator>()) alloc::LinearAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    alloc::stl::unordered_multimap< int, int > * mm1 = new(fla->allocate<alloc::stl::unordered_multimap< int, int >>()) alloc::stl::unordered_multimap< int, int >( fla );
    #else
    alloc::stl::unordered_multimap< int, int > * mm1 = new(defaultAllocator.allocate<alloc::stl::unordered_multimap< int, int >>()) alloc::stl::unordered_multimap< int, int >( &defaultAllocator );
    #endif

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

    #if !USE_DEFAULT
    fla->deallocate<alloc::stl::unordered_multimap< int, int > >( mm1 );
    defaultAllocator.deallocate<alloc::FreeListAllocator>( fla );
    //fla->clear();
    //defaultAllocator.deallocate<alloc::LinearAllocator>( fla );

    defaultAllocator.deallocateBlock( _mem_pool );
    #else
    defaultAllocator.deallocate<alloc::stl::unordered_multimap< int, int > >( mm1 );
    #endif

    defaultAllocator.printDebugInfo(std::cout);

    std::cout << "complete." << std::endl;

    return 0;
}
