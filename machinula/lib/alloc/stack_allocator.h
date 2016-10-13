#ifndef STACK_ALLOCATOR_H_INCLUDED
#define STACK_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class StackAllocator : public Allocator
    {
        private:

            struct _AllocationHeader
            {
                #ifndef NDEBUG
                void * prev_address;
                #endif // NDEBUG

                uint8_t adjust;
            };

            void * _current_pos;

            #ifndef NDEBUG
            void * _prev_pos;
            #endif // NDEBUG

        public:
            StackAllocator( std::size_t block_size, void * block_start );
            ~StackAllocator();

                   void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

                   void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            StackAllocator( const StackAllocator & ) = delete;
            StackAllocator & operator = ( const StackAllocator & ) = delete;
    };
}

#include "stack_allocator.inl"

#endif // STACK_ALLOCATOR_H_INCLUDED
