#ifndef FREE_LIST_ALLOCATOR_H_INCLUDED
#define FREE_LIST_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class FreeListAllocator : public Allocator
    {
        private:

            struct _AllocationHeader
            {
                std::size_t size;
                uint8_t     adjust;
            };

            struct _FreeBlock
            {
                std::size_t  size;
                _FreeBlock * next;
            };

            _FreeBlock * _free_blocks;
            //uintmax_t deallocLoops;

        public:
            FreeListAllocator( std::size_t block_size, void * block_start );
            ~FreeListAllocator();

            virtual void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual void deallocateBlock( void * block ) override;

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            FreeListAllocator( const FreeListAllocator & ) = delete;
            FreeListAllocator & operator = ( const FreeListAllocator & ) = delete;

    };
}

#endif // FREE_LIST_ALLOCATOR_H_INCLUDED
