#ifndef GLOBAL_ALLOCATOR_INL_INCLUDED
#define GLOBAL_ALLOCATOR_INL_INCLUDED

#include <iostream>

namespace alloc
{
    void *
    GlobalAllocator::allocateBlock( std::size_t size, uint8_t /*align*/ )
    {
        std::cerr << "GlobalAllocator::allocateBlock\n";
        _incrementAllocations(0);
        return (void*)::operator new( size );
    }

    void
    GlobalAllocator::deallocateBlock( void * block )
    {
        std::cerr << "GlobalAllocator::deallocateBlock\n";
        _decrementAllocations(0);
        ::operator delete( block );
    }
}

#endif // GLOBAL_ALLOCATOR_INL_INCLUDED
