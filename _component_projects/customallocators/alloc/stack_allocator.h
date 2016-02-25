#ifndef STACK_ALLOCATOR_H_INCLUDED
#define STACK_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class StackAllocator : public Allocator
    {
        public:
            StackAllocator( std::size_t block_size, void * block_start );
            ~StackAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align ) override;
            inline void deallocateBlock( void * block ) override;

        private:
            StackAllocator( const DefaultAllocator & ) = delete;
            StackAllocator & operator = ( const DefaultAllocator & ) = delete;

            struct _AllocationHeader
            {
                #ifndef NDEBUG
                void * prev_address;
                #endif // NDEBUG

                uint8_t adjust;
            };

            #ifndef NDEBUG
            void * _prev_pos;
            #endif // NDEBUG

            void * _current_pos;
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


}

#endif // STACK_ALLOCATOR_H_INCLUDED
