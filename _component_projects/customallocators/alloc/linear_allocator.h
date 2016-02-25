#ifndef LINEAR_ALLOCATOR_H_INCLUDED
#define LINEAR_ALLOCATOR_H_INCLUDED

#include "allocator.h"
#include "align.h"

namespace alloc
{
    class LinearAllocator : public Allocator
    {
        private:
            void * _next_free_block;

        public:
            LinearAllocator( std::size_t block_size, void * block_start );
            ~LinearAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align ) override;
            inline void deallocateBlock( void * block ) override;

            void clear();

        private:
            LinearAllocator( const LinearAllocator & ) = delete;
            LinearAllocator & operator = ( const LinearAllocator & ) = delete;
    };

    LinearAllocator::LinearAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _next_free_block(block_start)
    {
        assert( block_size > 0 );
    }

    LinearAllocator::~LinearAllocator()
    {
        _next_free_block = nullptr;
    }

    void *
    LinearAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size > 0 );

        uint8_t adjustment = align::forwardAdj( _next_free_block, align );

        if( _block_size - size < _used_memory + adjustment )
            return nullptr;

        uintptr_t address = uintptr_t(_next_free_block) + adjustment;

        _next_free_block = (void*)( address + size );

        _used_memory += adjustment + size;
        ++_num_allocations;

        return (void*)address;
    }

    void
    LinearAllocator::deallocateBlock( void * block )
    {
        // do not be here!
        assert( 0 && "Use LinearAllocator::clear(); not deallocateBlock." );
    }

    void
    LinearAllocator::clear()
    {
        _num_allocations = _used_memory = 0;

        _next_free_block = _block_start;
    }
}

#endif // LINEAR_ALLOCATOR_H_INCLUDED
