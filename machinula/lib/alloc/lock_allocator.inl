#ifndef LOCK_ALLOCATOR_INL_INCLUDED
#define LOCK_ALLOCATOR_INL_INCLUDED

namespace alloc
{
    template < typename AllocatorType >
    inline
    void *
    LockAllocator<AllocatorType>::allocateBlock( std::size_t size, uint8_t align )
    {
        std::lock_guard<std::mutex> lock( _allocatorMutex );
        return AllocatorType::allocateBlock( size, align );
    }

    template < typename AllocatorType >
    inline
    void
    LockAllocator<AllocatorType>::deallocateBlock( void * block )
    {
        std::lock_guard<std::mutex> lock( _allocatorMutex );
        AllocatorType::deallocateBlock( block );
    }

    template < typename AllocatorType >
    void
    LockAllocator<AllocatorType>::printDebugInfo( std::ostream& out ) const
    {
        // FIXME(dean): fix up this line, will probably break with two threads.
        std::lock_guard<std::mutex> lock( const_cast<std::mutex&>(_allocatorMutex) );
        AllocatorType::printDebugInfo( out );
    }
}

#endif // LOCK_ALLOCATOR_INL_INCLUDED
