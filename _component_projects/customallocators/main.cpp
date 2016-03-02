#include <iostream>
#include <vector>

#include <cstdlib>
#include <cstdio>

#include "alloc/alloc.h"

#define _MEM_POOL_SIZE (1024*1024*1024*0.5)

#define _NUM_TESTS 1

struct data_chunk
{
    uint64_t data[4];
};

int main()
{
    std::ios_base::sync_with_stdio( false );

    alloc::DefaultAllocator d( _MEM_POOL_SIZE+sizeof(uintptr_t)*64, nullptr );

    void * _mem_pool = d.allocateBlock( _MEM_POOL_SIZE, 0 );

    alloc::LinearAllocator * la;
    la = new(d.allocate<alloc::LinearAllocator>()) alloc::LinearAllocator( _MEM_POOL_SIZE, _mem_pool );

    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "la: " << c << " ... " << std::flush;

        while( la->unusedMemory() > sizeof(data_chunk) )
        {
            data_chunk * i = new(la->allocate<data_chunk>()) data_chunk;
            i->data[0] = 0;
        }

        la->clear();

        std::cout << "Done." << std::endl;
    }

    alloc::Allocator * sa;
    sa = new(d.allocate<alloc::StackAllocator>()) alloc::StackAllocator( _MEM_POOL_SIZE, _mem_pool );

    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "sa: " << c << " ... " << std::flush;

        unsigned _num_chunks = _MEM_POOL_SIZE/sizeof(data_chunk*);
        data_chunk ** chunks = (data_chunk**)sa->allocateBlock( _num_chunks, alignof(data_chunk*));

        unsigned _last_chunk = 0;

        while( sa->unusedMemory() > sizeof(data_chunk) && _last_chunk < _num_chunks )
        {
            chunks[_last_chunk] = new(sa->allocate<data_chunk>()) data_chunk;
            chunks[_last_chunk]->data[0] = 0;
            ++_last_chunk;
        }

        while( _last_chunk )
        {
            sa->deallocate<data_chunk>(chunks[--_last_chunk]);
        }

        sa->deallocateBlock( chunks );

        std::cout << "Done." << std::endl;
    }

    alloc::Allocator * pa;
    pa = new(d.allocate<alloc::PoolAllocator>()) alloc::PoolAllocator( sizeof(data_chunk), alignof(data_chunk), _MEM_POOL_SIZE, _mem_pool );

    std::vector<data_chunk*> chunks;
    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "pa: " << c << " ... " << std::flush;

        while( pa->unusedMemory() > sizeof(data_chunk) )
        {
            chunks.push_back( new(pa->allocate<data_chunk>()) data_chunk );
            chunks.back()->data[0] = 0;
        }

        for( auto p : chunks )
        {
            pa->deallocate<data_chunk>( p );
        }

        chunks.clear();

        std::cout << "Done." << std::endl;
    }

    alloc::Allocator * fa;
    fa = new(d.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _MEM_POOL_SIZE, _mem_pool );

    //std::vector<data_chunk*> chunks;
    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "fa: " << c << " ... " << std::flush;

        while( fa->unusedMemory() > sizeof(data_chunk) )
        {
            chunks.push_back( new(fa->allocate<data_chunk>()) data_chunk );
            chunks.back()->data[0] = 0;
        }

        for( auto p : chunks )
        {
            fa->deallocate<data_chunk>( p );
        }

        chunks.clear();

        std::cout << "Done." << std::endl;
    }

    std::cout << "\nLinear allocator:" << std::endl;
    std::cout << "Max bytes allocated = " << la->maxUsedMemory() << std::endl;
    std::cout << "Max num allocations = " << la->maxNumAllocations() << std::endl;

    std::cout << "\nStack allocator:" << std::endl;
    std::cout << "Max bytes allocated = " << sa->maxUsedMemory() << std::endl;
    std::cout << "Max num allocations = " << sa->maxNumAllocations() << std::endl;

    std::cout << "\nPool allocator:" << std::endl;
    std::cout << "Max bytes allocated = " << pa->maxUsedMemory() << std::endl;
    std::cout << "Max num allocations = " << pa->maxNumAllocations() << std::endl;

    std::cout << "\nFreeList allocator:" << std::endl;
    std::cout << "Max bytes allocated = " << fa->maxUsedMemory() << std::endl;
    std::cout << "Max num allocations = " << fa->maxNumAllocations() << std::endl;

    d.deallocate<alloc::LinearAllocator>( la );
    d.deallocate<alloc::StackAllocator>( sa );
    d.deallocate<alloc::PoolAllocator>( pa );
    d.deallocate<alloc::FreeListAllocator>( fa );

    d.deallocateBlock( _mem_pool );

    std::cout << "\nDefault allocator:" << std::endl;
    std::cout << "Max bytes allocated = " << d.maxUsedMemory() << std::endl;
    std::cout << "Max num allocations = " << d.maxNumAllocations() << std::endl;

    return 0;
}
