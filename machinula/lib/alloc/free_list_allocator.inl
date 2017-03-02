#ifndef FREE_LIST_ALLOCATOR_INL_INCLUDED
#define FREE_LIST_ALLOCATOR_INL_INCLUDED

#include <cstring>

namespace alloc
{
    template < unsigned int deallocBlockSize >
    DefferedFreeListAllocator< deallocBlockSize >::DefferedFreeListAllocator( std::size_t block_size, void * block_start )
    : FreeListAllocator( block_size, block_start ), numPointersToDeallocate(0)
    {
        static_assert( deallocBlockSize > 3, "template argument must have 4 or more as its value." );
        assert( deallocBlockSize*sizeof(void*) < _block_size/2 && "deallocation block mus t be less than half the size of the total size of the allocation space." );

        // subtract the deallocation block from the available size in the free list
        _free_blocks->size = block_size - sizeof(void*) * deallocBlockSize;

        // clear the deallocation block
        clearDeallocationBlock();
    }

    template < unsigned int deallocBlockSize >
    DefferedFreeListAllocator< deallocBlockSize >::~DefferedFreeListAllocator()
    {
        // make sure there is nothing left to deallocate
        batchDeallocate();
    }

    template < unsigned int deallocBlockSize >
    void *
    DefferedFreeListAllocator< deallocBlockSize >::allocateBlock( std::size_t size, uint8_t align )
    {
        // call normal allocator
        // if it fails batchDeallocate()
        // then call normal allocator again

        void * block = FreeListAllocator::allocateBlock( size, align );

        if( block )
        {
            return block;
        }

        batchDeallocate();

        return FreeListAllocator::allocateBlock( size, align );
    }

    template < unsigned int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::deallocateBlock( void * block )
    {
        // if there is no more space to add in the newest block, batchDeallocate()
        // if there is still space in the to free list add in block and make sure its sorted ( insertion sort )

        void ** deallocList = deallocBlock();

        // if the list of pointers is full
        if( deallocList[0] )
        {
            batchDeallocate();
        }

        std::size_t i = deallocBlockSize - ++numPointersToDeallocate;
        deallocList[i] = block;

        /*while( i < deallocBlockSize-1 )
        {
            if( deallocList[i+1] || i+2 >= deallocBlockSize )
            {
                deallocList[i] = block;
                break;
            }
            ++i;
        }*/

        //deallocList[0] = block;

        // insertion sort the pointers
        for( ; i < deallocBlockSize-1; ++i )
        {
            if( reinterpret_cast<uintptr_t>(deallocList[i]) > reinterpret_cast<uintptr_t>(deallocList[i + 1]) )
            {
                std::swap<void*>( deallocList[i], deallocList[i + 1] );
            }
        }
    }

    template < unsigned int deallocBlockSize >
    std::size_t
    DefferedFreeListAllocator< deallocBlockSize >::unusedMemory() const
    {
        return _block_size - _used_memory - ( sizeof(void*) * deallocBlockSize );
    }

    template < unsigned int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::printDebugInfo( std::ostream& out ) const
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
        out << "\tMin Free Memory: " << minFreeMemory() << " bytes\n";
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


            out << "\tpending_deallocation:\n\t{\n";
            void ** deallocList = deallocBlock();
            for( std::size_t i = 0; i < deallocBlockSize; ++i )
            {
                if( !deallocList[i] )
                {
                    //continue;
                }
                out << "\t\t" << i << ": 0x" << deallocList[i] << "\n";
            }
            out << "\t};\n";

            out << "};\n";
        }
    }

    template < unsigned int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::trueDeallocateBlock( void * block )
    {
        FreeListAllocator::deallocateBlock( block );
    }

    template < unsigned int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::batchDeallocate()
    {
        // deallocate all in the stored list
        // then clear the list

        void ** deallocList = deallocBlock();

        for( std::size_t i = 0; i < deallocBlockSize; ++i )
        {
            if( !deallocList[i] )
            {
                deallocList[i] = nullptr;
                continue;
            }

            trueDeallocateBlock( deallocList[i] );
            deallocList[i] = nullptr;
        }

        clearDeallocationBlock();
    }

    template < unsigned int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::clearDeallocationBlock()
    {
        // sets all the pointers in the deallocation list to null
        memset( (void*)deallocBlock(), 0, sizeof(void*) * deallocBlockSize );
        numPointersToDeallocate = 0;
    }

    template < unsigned int deallocBlockSize >
    constexpr
    void **
    DefferedFreeListAllocator< deallocBlockSize >::deallocBlock() const
    {
        return (void**)( ( reinterpret_cast<intptr_t>( getBlock() ) + getSize() ) - ( sizeof(void*) * deallocBlockSize ) );
    }
}

#endif // FREE_LIST_ALLOCATOR_INL_INCLUDED
