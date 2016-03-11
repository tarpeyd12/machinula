#include <iostream>
#include <vector>

#include <cstdlib>
#include <cstdio>

#include <fstream>

#include "alloc/alloc.h"

//#define _MEM_POOL_SIZE (1024*1024*1024*0.125)
#define _MEM_POOL_SIZE (1024*1024*1024*0.5)
#define _DATA_CHUNK_SIZE 64

#define _NUM_TESTS 10

#define TEST_LINEAR 1
#define TEST_STACK  1
#define TEST_POOL   1
#define TEST_FLIST  1
#define TEST_SLTADP 1

#define PRINT_DEBUG 1

struct data_chunk
{
    uint8_t data[_DATA_CHUNK_SIZE];
};

int main()
{
    std::ios_base::sync_with_stdio( false );

    #if PRINT_DEBUG
    std::fstream outfile( "data_dump.txt", std::fstream::out );
    #endif // PRINT_DEBUG

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

        std::cout << "clearing ... " << std::flush;

        la->clear();

        std::cout << "Done." << std::endl;
    }
    #endif
    #if PRINT_DEBUG
    std::cout << "Printing Debug ... " << std::flush;
    la->printDebugInfo(outfile);
    std::cout << "Done." << std::endl;
    #endif // PRINT_DEBUG

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

        std::cout << "clearing ... " << std::flush;

        while( _last_chunk )
        {
            sa->deallocate<data_chunk>(chunks[--_last_chunk]);
        }

        //sa->deallocateBlock( chunks );
        d.deallocateBlock( chunks );

        std::cout << "Done." << std::endl;
    }
    #endif
    #if PRINT_DEBUG
    std::cout << "Printing Debug ... " << std::flush;
    sa->printDebugInfo(outfile);
    std::cout << "Done." << std::endl;
    #endif // PRINT_DEBUG

    alloc::Allocator * pa;
    pa = new(d.allocate<alloc::PoolAllocator>()) alloc::PoolAllocator( sizeof(data_chunk), alignof(data_chunk), _MEM_POOL_SIZE, _mem_pool );

    std::vector<data_chunk*> chunks;
    #if TEST_POOL
    for( unsigned c = 0; c < _NUM_TESTS; ++c )
    {
        std::cout << "pa: " << c << " ... " << std::flush;

        //pa->printDebugInfo();

        //while( pa->unusedMemory() > sizeof(data_chunk) )
        while( pa->unusedMemory() > _MEM_POOL_SIZE/2 )
        {
            chunks.push_back( new(pa->allocate<data_chunk>()) data_chunk );
            chunks.back()->data[0] = 0;
        }

        std::cout << "clearing ... " << std::flush;

        for( auto p : chunks )
        {
            pa->deallocate<data_chunk>( p );
        }

        chunks.clear();

        std::cout << "Done." << std::endl;
    }
    #endif
    #if PRINT_DEBUG
    std::cout << "Printing Debug ... " << std::flush;
    pa->printDebugInfo(outfile);
    std::cout << "Done." << std::endl;
    #endif // PRINT_DEBUG

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

        std::cout << "clearing ... " << std::flush;

        for( auto p : chunks )
        {
            fa->deallocate<data_chunk>( p );
        }

        chunks.clear();

        std::cout << "Done." << std::endl;
    }
    #endif
    #if PRINT_DEBUG
    std::cout << "Printing Debug ... " << std::flush;
    fa->printDebugInfo(outfile);
    std::cout << "Done." << std::endl;
    #endif // PRINT_DEBUG

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
    alloc::Allocator * ra = new(d.allocate<alloc::ProxyAllocator>()) alloc::ProxyAllocator( fa );

    std::cout << "\nstd::vector<>:" << std::endl;

    for( unsigned c = 0; c < _NUM_TESTS; ++c  )
    {
        std::cout << "test " << c << " ... " << std::flush;
        alloc::stl::vector< data_chunk > v( ra );

        v.reserve( (ra->unusedMemory()-sizeof(std::size_t)-sizeof(uint8_t))/sizeof(data_chunk) );

        for(; v.size() < v.capacity() ;)
        {
            data_chunk dc;
            dc.data[0] = 0;
            v.push_back( dc );
        }

        std::cout << "clearing ... " << std::flush;

        if(!v.clear_all_memory()) std::cout << "err ";

        //v.clear();
        //v.shrink_to_fit(); // NOTE: some implementations won't actually clear the memory with this call.
        std::cout << "Done.\n" << std::flush;
    }

    std::cout << "\nstd::unordered_map<>:" << std::endl;

    for( unsigned c = 0; c < _NUM_TESTS; ++c  )
    {
        std::cout << "test " << c << " ... " << std::flush;
        alloc::stl::unordered_map< std::size_t, data_chunk > m( ra );

        for(; ra->unusedMemory() > 1024*1024*33 ;)
        {
            data_chunk dc;
            dc.data[0] = 0;
            m.insert( std::pair<std::size_t,data_chunk>(m.size(),dc) );
        }

        std::cout << "clearing ... " << std::flush;

        if(!m.clear_all_memory()) std::cout << "err ";

        std::cout << "Done.\n" << std::flush;
    }

    std::cout << "\nstd::map<>:" << std::endl;

    //pa = new(d.allocate<alloc::ObjectPoolAllocator<std::pair<const std::size_t,data_chunk>>>()) alloc::ObjectPoolAllocator<std::pair<const std::size_t,data_chunk>>( _MEM_POOL_SIZE, _mem_pool );

    alloc::stl::map< std::size_t, data_chunk > m( fa );
    for( unsigned c = 0; c < _NUM_TESTS; ++c  )
    {
        std::cout << "test " << c << " ... " << std::flush;
        for(; ra->unusedMemory() > 1024*1024*33 ;)
        {
            data_chunk dc;
            dc.data[0] = 0;
            m.insert( std::pair<std::size_t,data_chunk>(m.size(),dc) );
        }

        std::cout << "clearing ... " << std::flush;

        if(!m.clear_all_memory()) std::cout << "err ";

        std::cout << "Done.\n" << std::flush;
    }

    std::cout << "\nstd::deque<>:" << std::endl;

    for( unsigned c = 0; c < _NUM_TESTS; ++c  )
    {
        alloc::stl::deque< data_chunk > q( fa );

        std::cout << "test " << c << " ... " << std::flush;

        for(; ra->unusedMemory() > 1024*1024*33 ;)
        {
            data_chunk dc;
            dc.data[0] = 0;
            q.push_back( dc );
            q.push_back( dc );
            q.pop_front();
        }

        std::cout << "clearing ... " << std::flush;

        if(!q.clear_all_memory()) std::cout << "err ";

        std::cout << "Done.\n" << std::flush;
    }

    d.deallocate<alloc::ProxyAllocator>( ra );
    d.deallocate<alloc::FreeListAllocator>( fa );
    //d.deallocate< alloc::ObjectPoolAllocator<std::pair<const std::size_t,data_chunk>> >( pa );

    d.deallocateBlock( _mem_pool );

    std::cout << "\nComplete!" << std::endl;
    #endif

    #if PRINT_DEBUG
    d.printDebugInfo(outfile);
    outfile.close();
    #endif // PRINT_DEBUG

    return 0;
}
