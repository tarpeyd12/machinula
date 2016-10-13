#ifndef DEFAULT_ALLOCATOR_INL_INCLUDED
#define DEFAULT_ALLOCATOR_INL_INCLUDED

namespace alloc
{
    void
    DefaultAllocator::clear()
    {
        while( _allocated_blocks.size() )
        {
            deallocateBlock( reinterpret_cast<void*>(_allocated_blocks.begin()->first) );
        }
    }
}

#endif // DEFAULT_ALLOCATOR_INL_INCLUDED
