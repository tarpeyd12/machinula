#ifndef GLOBAL_ALLOCATOR_INL_INCLUDED
#define GLOBAL_ALLOCATOR_INL_INCLUDED

#include <iostream>

namespace alloc
{
    void *
    GlobalAllocator::allocateBlock( std::size_t size, uint8_t /*align*/ )
    {
        //std::cerr << "GlobalAllocator::allocateBlock\n";
        //printDebugInfo();
        //_incrementAllocations(1); // size of 1 so that allocator does not get mad at us
        _max_num_allocations = std::max( _max_num_allocations, ++_num_allocations );
        return (void*)::operator new( size );
    }

    void
    GlobalAllocator::deallocateBlock( void * block )
    {
        //std::cerr << "GlobalAllocator::deallocateBlock\n";
        //printDebugInfo();
        //_decrementAllocations(1); // size of 1 so that allocator does not get mad at us
        --_num_allocations;
        ::operator delete( block );
    }
}

#endif // GLOBAL_ALLOCATOR_INL_INCLUDED
