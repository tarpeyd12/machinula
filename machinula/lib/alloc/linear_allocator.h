#ifndef LINEAR_ALLOCATOR_H_INCLUDED
#define LINEAR_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class LinearAllocator : public Allocator
    {
        private:
            void * _next_free_block;

        public:
            LinearAllocator( std::size_t block_size, void * block_start );
            ~LinearAllocator();

            virtual        void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual inline void deallocateBlock( void * block ) override;

            virtual        void printDebugInfo( std::ostream& out = std::cerr ) const override;

            inline void clear();

        private:
            LinearAllocator( const LinearAllocator & ) = delete;
            LinearAllocator & operator = ( const LinearAllocator & ) = delete;
    };
}

#include "linear_allocator.inl"

#endif // LINEAR_ALLOCATOR_H_INCLUDED
