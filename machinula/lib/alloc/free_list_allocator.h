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
                std::size_t size;
                _FreeBlock * next;
            };

            _FreeBlock * _free_blocks;
            //uintmax_t deallocLoops;

        public:
            FreeListAllocator( std::size_t block_size, void * block_start );
            ~FreeListAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            FreeListAllocator( const FreeListAllocator & ) = delete;
            FreeListAllocator & operator = ( const FreeListAllocator & ) = delete;

    };

    FreeListAllocator::FreeListAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _free_blocks( (_FreeBlock*)block_start )
    {
        assert( block_size > sizeof(_FreeBlock) );

        _free_blocks->size = block_size;
        _free_blocks->next = nullptr;
        //deallocLoops = 0;
    }

    FreeListAllocator::~FreeListAllocator()
    {
        _free_blocks = nullptr;
    }

    void *
    FreeListAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size != 0 && align != 0 );

        _FreeBlock * prev_block = nullptr;
        _FreeBlock * curr_block = _free_blocks;

        while( curr_block != nullptr )
        {
            uint8_t adjust = align::forwardAdj_withHeader( curr_block, align, sizeof(_AllocationHeader) );

            std::size_t size_total = size + adjust;

            // if the size needed to be allocated is too large for the current block, go to next block
            if( curr_block->size < size_total )
            {
                prev_block = curr_block;
                curr_block = curr_block->next;
                continue;
            }

            static_assert( sizeof(_AllocationHeader) >= sizeof(_FreeBlock), "sizeof(_AllocationHeader) < sizeof(_FreeBlock)" );

            // if allocations in the remaining memory is impossible
            if( curr_block->size - size_total <= sizeof(_AllocationHeader) )
            {
                // TODO(dean): figure out what the hell is going on here
                // increase allocation size rather than creating a new _FreeBlock
                size_total = curr_block->size;

                if( prev_block != nullptr )
                {
                    prev_block->next = curr_block->next;
                }
                else
                {
                    _free_blocks = curr_block->next;
                }
            }
            else
            {
                // create a new _FreeBlock that points to the remaining free memory in the selected block
                _FreeBlock * next_block = (_FreeBlock*)align::_add( curr_block, size_total );
                next_block->size = curr_block->size - size_total;
                next_block->next = curr_block->next;

                if( prev_block != nullptr )
                {
                    prev_block->next = next_block;
                }
                else
                {
                    _free_blocks = next_block;
                }
            }

            uintptr_t aligned_adderss = reinterpret_cast<uintptr_t>(curr_block) + adjust;

            _AllocationHeader * header = (_AllocationHeader*)( aligned_adderss - sizeof(_AllocationHeader) );
            header->size   = size_total;
            header->adjust = adjust;

            _incrementAllocations( size_total );

            assert( align::forwardAdj( (void*)aligned_adderss, align ) == 0 );

            return (void*)aligned_adderss;
        }

        assert( 0 && "Could not find block of memory large enough for requested size." );

        return nullptr;
    }

    void
    FreeListAllocator::deallocateBlock( void * block )
    {
        // TODO(dean): Figure out what the hell is going on in this function
        // TODO(dean): Comment this function
        assert( block != nullptr );

        _AllocationHeader * header = (_AllocationHeader*)align::_sub( block, sizeof(_AllocationHeader) );

        uintptr_t   block_start = reinterpret_cast<uintptr_t>(block) - header->adjust;
        std::size_t block_size  = header->size;
        uintptr_t   block_end   = block_start + block_size;

        _FreeBlock * prev_block = nullptr;
        _FreeBlock * curr_block = _free_blocks;

        while( curr_block != nullptr )
        {
            if( reinterpret_cast<uintptr_t>(curr_block) >= block_end )
            {
                break;
            }
            prev_block = curr_block;
            curr_block = curr_block->next;
            //++deallocLoops;
        }

        if( prev_block == nullptr )
        {
            prev_block = (_FreeBlock*)block_start;
            prev_block->size = block_size;
            prev_block->next = _free_blocks;

            _free_blocks = prev_block;
        }
        else if( reinterpret_cast<uintptr_t>(prev_block) + prev_block->size == block_start )
        {
            prev_block->size += block_size;
        }
        else
        {
            _FreeBlock * tmp = (_FreeBlock*)block_start;
            tmp->size = block_size;
            tmp->next = prev_block->next;
            prev_block->next = tmp;

            prev_block = tmp;
        }

        if( curr_block != nullptr && reinterpret_cast<uintptr_t>(curr_block) == block_end )
        {
            prev_block->size += curr_block->size;
            prev_block->next = curr_block->next;
        }

        _decrementAllocations( block_size );
    }

    void
    FreeListAllocator::printDebugInfo( std::ostream& out ) const
    {
        void * pend;
        out << "FreeListAllocator(" << this << "):\n";
        out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        out << "\tBlock End:  " << (pend = (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize())) << "\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";
        //out << "\tNumber of deallocLoops: " << deallocLoops << "\n";

        if(true)
        {
            out << "{\n";
            out << "\tfree:\n\t{\n";
            std::size_t numFreeBlocks = 0;
            _FreeBlock * curr_block = _free_blocks;
            while( curr_block != nullptr && (void*)curr_block >= _block_start && (void*)curr_block < pend)
            {

                out << "\t\taddr:" << curr_block << ",size:" << curr_block->size << ",end:" << (void*)(reinterpret_cast<uintptr_t>(curr_block)+curr_block->size) << ",next:" << curr_block->next << "\n";
                curr_block = curr_block->next;
                ++numFreeBlocks;
            }
            out << "\t}(freeBlocks:" << numFreeBlocks << ");\n";
            out << "};\n";
        }
    }
}

#endif // FREE_LIST_ALLOCATOR_H_INCLUDED
