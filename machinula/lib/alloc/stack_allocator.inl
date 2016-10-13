#ifndef STACK_ALLOCATOR_INL_INCLUDED
#define STACK_ALLOCATOR_INL_INCLUDED

namespace alloc
{
    void
    StackAllocator::deallocateBlock( void * block )
    {
        assert( block == _prev_pos );

        // retrieve the header
        _AllocationHeader * header = (_AllocationHeader*)(align::_sub( block, sizeof(_AllocationHeader) ));

        _decrementAllocations( uintptr_t(_current_pos) - uintptr_t(block) + header->adjust );

        _current_pos = align::_sub( block, header->adjust );

        #ifndef NDEBUG
        _prev_pos = header->prev_address;
        #endif // NDEBUG
    }
}

#endif // STACK_ALLOCATOR_INL_INCLUDED
