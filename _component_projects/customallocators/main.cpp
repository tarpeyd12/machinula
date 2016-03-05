#include <iostream>
#include <vector>

#include <cstdlib>
#include <cstdio>

#include "alloc/alloc.h"

#define _MEM_POOL_SIZE (1024*1024*1024*0.5)

#define _NUM_TESTS 10

#define TEST_LINEAR 1
#define TEST_STACK  1
#define TEST_POOL   1
#define TEST_FLIST  1
#define TEST_SLTADP 1

struct data_chunk
{
    uint8_t data[8];
};

int main()
{
    std::ios_base::sync_with_stdio( false );

    //alloc::DefaultAllocator d( _MEM_POOL_SIZE+sizeof(uintptr_t)*1024, nullptr );
    alloc::DefaultAllocator d( 0, nullptr );

    void * _mem_pool = d.allocateBlock( _MEM_POOL_SIZE, 0 );

    alloc::LinearAllocator * la;
    la = new(d.allocate<alloc::LinearAllocator>()) alloc::LinearAllocator( _MEM_POOL_SIZE, _mem_pool );

    #if TEST_LINEAR
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
    #endif

    alloc::Allocator * sa;
    sa = new(d.allocate<alloc::StackAllocator>()) alloc::StackAllocator( _MEM_POOL_SIZE, _mem_pool );
    #if TEST_STACK
    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "sa: " << c << " ... " << std::flush;

        unsigned _num_chunks = _MEM_POOL_SIZE/sizeof(data_chunk*);
        //data_chunk ** chunks = (data_chunk**)sa->allocateBlock( _num_chunks, alignof(data_chunk*));
        data_chunk ** chunks = (data_chunk**)d.allocateBlock( _num_chunks, alignof(data_chunk*));

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

        //sa->deallocateBlock( chunks );
        d.deallocateBlock( chunks );

        std::cout << "Done." << std::endl;
    }
    #endif

    alloc::Allocator * pa;
    pa = new(d.allocate<alloc::PoolAllocator>()) alloc::PoolAllocator( sizeof(data_chunk), alignof(data_chunk), _MEM_POOL_SIZE, _mem_pool );
    #if TEST_POOL
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
    #endif

    alloc::Allocator * fa;
    fa = new(d.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _MEM_POOL_SIZE, _mem_pool );

    #if TEST_FLIST
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
    #endif

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

    #if TEST_SLTADP
    std::cout << "\nalloc::stl_adapter expirementation:" << std::endl;

    _mem_pool = d.allocateBlock( _MEM_POOL_SIZE, 0 );

    fa = new(d.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _MEM_POOL_SIZE, _mem_pool );

    std::vector< data_chunk, alloc::stl_adapter<data_chunk> > v( (const alloc::stl_adapter<data_chunk>&)alloc::stl_adapter<data_chunk>(fa) );

    for( unsigned c = 0; c < _NUM_TESTS; ++c  )
    {
        v.reserve( (fa->unusedMemory()-sizeof(std::size_t)-sizeof(uint8_t))/sizeof(data_chunk) );

        for(; v.size() < v.capacity() ;)
        {
            data_chunk dc;
            dc.data[0] = 0;
            v.push_back( dc );
        }

        v.clear();
        v.shrink_to_fit(); // NOTE: some implementations won't actually clear the memory with this call.
    }

    d.deallocate<alloc::FreeListAllocator>( fa );

    d.deallocateBlock( _mem_pool );

    std::cout << "\nComplete!" << std::endl;
    #endif

    return 0;
}
