#ifndef FREE_LIST_ALLOCATOR_H_INCLUDED
#define FREE_LIST_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class FreeListAllocator : public Allocator
    {
        protected:

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

        public:
            FreeListAllocator( std::size_t block_size, void * block_start );
            virtual ~FreeListAllocator();

            virtual void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual void deallocateBlock( void * block ) override;

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const override;

        protected:
            void deallocateSortedBlockBatch( void ** blockList, std::size_t num );

        private:
            FreeListAllocator( const FreeListAllocator & ) = delete;
            FreeListAllocator & operator = ( const FreeListAllocator & ) = delete;

    };

    template < unsigned int deallocBlockSize >
    class DefferedFreeListAllocator : public FreeListAllocator
    {
        private:
            std::size_t numPointersToDeallocate;

        public:
            DefferedFreeListAllocator( std::size_t block_size, void * block_start );
            ~DefferedFreeListAllocator();

            virtual void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual void deallocateBlock( void * block ) override;

            virtual std::size_t unusedMemory() const override;

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            DefferedFreeListAllocator( const DefferedFreeListAllocator & ) = delete;
            DefferedFreeListAllocator & operator = ( const DefferedFreeListAllocator & ) = delete;

            inline void trueDeallocateBlock( void * block );

            inline void batchDeallocate();

            inline void clearDeallocationBlock();

            constexpr inline void ** getDeallocBlock() const;
    };
}

#include "free_list_allocator.inl"

#endif // FREE_LIST_ALLOCATOR_H_INCLUDED
