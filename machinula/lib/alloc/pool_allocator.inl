#ifndef POOL_ALLOCATOR_INL_INCLUDED
#define POOL_ALLOCATOR_INL_INCLUDED

// TODO(dean): migrate implementation to cpp files

namespace alloc
{
    inline
    void *
    PoolAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size == _object_size && align == _object_alignment );

        if( nullptr == _free_block_list  )
        {
            return nullptr;
        }

        // get the pointer that the _free_block_list is pointing to( this is our memory block)
        void * block = (void*)_free_block_list;

        // set the _free_block_list to the pointer that the data that _free_block_list is pointing to
        _free_block_list = (void**)(*_free_block_list);

        _incrementAllocations( _object_size );

        return block;
    }

    inline
    void
    PoolAllocator::deallocateBlock( void * block )
    {
        assert( nullptr != block );

        // set the data at block to point to the same address as _free_block_list
        *((void**)block) = _free_block_list;

        // set _free_block_list to point to the same address as the given block
        _free_block_list = (void**)block;

        _decrementAllocations( _object_size );
    }
}

#endif // POOL_ALLOCATOR_INL_INCLUDED
