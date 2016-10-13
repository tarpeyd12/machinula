#ifndef LINEAR_ALLOCATOR_INL_INCLUDED
#define LINEAR_ALLOCATOR_INL_INCLUDED

#include "align.h"

namespace alloc
{
    void
    LinearAllocator::deallocateBlock( void * /*block*/ )
    {
        // do not be here!
        assert( 0 && "Use LinearAllocator::clear(); not deallocateBlock." );
    }

    void
    LinearAllocator::clear()
    {
        _num_allocations = _used_memory = 0;

        _next_free_block = _block_start;
    }
}

#endif // LINEAR_ALLOCATOR_INL_INCLUDED
