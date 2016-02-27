#ifndef STACK_ALLOCATOR_H_INCLUDED
#define STACK_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class StackAllocator : public Allocator
    {
        private:
            #ifndef NDEBUG
            void * _prev_pos;
            #endif // NDEBUG

            void * _current_pos;

        public:
            StackAllocator( std::size_t block_size, void * block_start );
            ~StackAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align ) override;
            inline void deallocateBlock( void * block ) override;

        private:
            StackAllocator( const StackAllocator & ) = delete;
            StackAllocator & operator = ( const StackAllocator & ) = delete;

            struct _AllocationHeader
            {
                #ifndef NDEBUG
                void * prev_address;
                #endif // NDEBUG

                uint8_t adjust;
            };
    };

    StackAllocator::StackAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _current_pos(block_start)
    {
        assert( block_size > 0 );

        #ifndef NDEBUG
        _prev_pos = nullptr;
        #endif
    }

    StackAllocator::~StackAllocator()
    {
        #ifndef NDEBUG
        _prev_pos = nullptr;
        #endif

        _current_pos = nullptr;
    }

    void *
    StackAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size > 0 );

        uint8_t adjustment = align::forwardAdj_withHeader( _current_pos, align, sizeof(_AllocationHeader) );

        if( _block_size - size < _used_memory + adjustment )
            return nullptr;

        void * address_aligned = align::_add( _current_pos, adjustment );

        _AllocationHeader * header = (_AllocationHeader*)(align::_sub(address_aligned, sizeof(_AllocationHeader)));

        header->adjust = adjustment;

        #ifndef NDEBUG
        header->prev_address = _prev_pos;
        _prev_pos = address_aligned;
        #endif // NDEBUG

        _current_pos = align::_add( address_aligned, size );

        _incrementAllocations( size + adjustment );

        return address_aligned;
    }

    void
    StackAllocator::deallocateBlock( void * block )
    {
        assert( block == _prev_pos );

        // retrieve the header
        _AllocationHeader * header = (_AllocationHeader*)(align::_sub( block, sizeof(_AllocationHeader) ));

        _decrementAllocations( uintptr_t(_current_pos) - uintptr_t(block) + header->adjust );

        _current_pos = align::_sub( block, header->adjust );

        #ifndef NDEBUG
        _prev_pos = header->prev_address;
        #endif // NDEBUG

    }

}

#endif // STACK_ALLOCATOR_H_INCLUDED
