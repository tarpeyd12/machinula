#ifndef FREE_LIST_ALLOCATOR_INL_INCLUDED
#define FREE_LIST_ALLOCATOR_INL_INCLUDED

namespace alloc
{
    template < int deallocBlockSize >
    DefferedFreeListAllocator< deallocBlockSize >::DefferedFreeListAllocator( std::size_t block_size, void * block_start )
    {
        //
    }

    template < int deallocBlockSize >
    DefferedFreeListAllocator< deallocBlockSize >::~DefferedFreeListAllocator()
    {
        batchDeallocate();
    }

    template < int deallocBlockSize >
    void *
    DefferedFreeListAllocator< deallocBlockSize >::allocateBlock( std::size_t size, uint8_t align )
    {
        // call normal allocator
        // if it fails batchDeallocate()
        // then call normal allocator again
    }

    template < int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::deallocateBlock( void * block )
    {
        // if there is still space in the to free list add in block and make sure its sorted ( insertion sort )
        // if there is no more space after adding in the newest block, batchDeallocate()
    }

    template < int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::printDebugInfo( std::ostream& out ) const
    {

    }

    template < int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::trueDeallocateBlock( void * block )
    {
        FreeListAllocator::deallocate( block );
    }

    template < int deallocBlockSize >
    void
    DefferedFreeListAllocator< deallocBlockSize >::batchDeallocate()
    {
        // deallocate all in the stored list
        // then clear the list
    }
}

#endif // FREE_LIST_ALLOCATOR_INL_INCLUDED
