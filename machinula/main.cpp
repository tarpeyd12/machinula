#include <cstdlib>

#include <iostream>
#include <stack>
#include <set>
#include <algorithm>

#include "simple_test.h"

#include "lib/alloc.h"

void
test_random_deallocation( ptr::Allocator * alloc, bool debug = false )
{
    std::set< void* > allocated_blocks;

    unsigned int testblocksize = 32;

    while( alloc->unusedMemory() > 2 * testblocksize )
    {
        allocated_blocks.insert( alloc->allocateBlock( testblocksize, 1 ) );
    }

    while( allocated_blocks.size() )
    {
        if( debug ) alloc->printDebugInfo();

        std::set< void* >::const_iterator it(allocated_blocks.begin());
        std::advance( it, int(rand()%allocated_blocks.size()) );

        alloc->deallocateBlock( *it );
        allocated_blocks.erase( it );

        if( allocated_blocks.size() % 1000 == 0 )
        {
            std::cout << allocated_blocks.size() << std::endl;
        }
    }
    if( debug ) alloc->printDebugInfo();
}

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    Simple_Test::test_all();

    std::size_t _size = 1024*1024*1;
    void * _mem = ptr::SafeGlobalAllocator().allocateBlock( _size );

    auto dfla = new ptr::DefferedFreeListAllocator<512>( _size, _mem );
    //auto dfla = new ptr::FreeListAllocator( _size - sizeof(void*)*512, _mem );

    //test_random_deallocation( dfla );
    //test_random_deallocation( dfla );

    std::cout << "complete.\nCleaning up..." << std::endl;

    delete dfla;

    ptr::SafeGlobalAllocator().deallocateBlock( _mem );

    return EXIT_SUCCESS;
}
