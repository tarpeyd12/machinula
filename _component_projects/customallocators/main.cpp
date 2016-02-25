#include <iostream>

#include <cstdlib>
#include <cstdio>

#include "alloc/alloc.h"

#define _MEM_POOL_SIZE (1024*1024*1024)

struct data_chunk
{
    int data[32];
};

int main()
{
    alloc::DefaultAllocator d;

    void * _mem_pool = d.allocateBlock( _MEM_POOL_SIZE, 0 );

    alloc::LinearAllocator * la;
    la = new(d.allocate<alloc::LinearAllocator>()) alloc::LinearAllocator( _MEM_POOL_SIZE, _mem_pool );

    for( unsigned c = 0; c < 1000; ++c )
    {
        std::cout << c << " ... " << std::flush;

        while( la->unusedMemory() > sizeof(data_chunk) )
        {
            data_chunk * i = new(la->allocate<data_chunk>()) data_chunk;
            i->data[0] = 0;
        }

        la->clear();

        std::cout << "Done." << std::endl;
    }

    d.deallocate<alloc::LinearAllocator>( la );
    d.deallocateBlock( _mem_pool );

    return 0;
}
