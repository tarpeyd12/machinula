#ifndef POOL_ALLOCATOR_H_INCLUDED
#define POOL_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class PoolAllocator : public Allocator
    {
        private:
            std::size_t _object_size;
            uint8_t _object_alignment;

            void ** _free_block_list;

        public:
            PoolAllocator( std::size_t object_size, uint8_t object_align, std::size_t block_size, void * block_start );
            ~PoolAllocator();

            virtual inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual inline void deallocateBlock( void * block ) override;

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            PoolAllocator( const PoolAllocator & ) = delete;
            PoolAllocator & operator = ( const PoolAllocator & ) = delete;
    };
}

#include "pool_allocator.inl"

#endif // POOL_ALLOCATOR_H_INCLUDED
