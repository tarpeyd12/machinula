#ifndef LOCK_ALLOCATOR_H_INCLUDED
#define LOCK_ALLOCATOR_H_INCLUDED

#include "allocator.h"

#include <mutex>

namespace alloc
{
    template < typename AllocatorType >
    class LockAllocator final : public AllocatorType
    {
        private:
            std::mutex _allocatorMutex;

        public:

            template < typename ... ArgTypes >
            LockAllocator<AllocatorType>( ArgTypes ... args ) : AllocatorType( args... ), _allocatorMutex() {  }
            virtual ~LockAllocator<AllocatorType>() = default;

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            inline void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            LockAllocator<AllocatorType>( const LockAllocator<AllocatorType> & ) = delete;
            LockAllocator<AllocatorType> & operator = ( const LockAllocator<AllocatorType> & ) = delete;
    };

    template < typename AllocatorType >
    void *
    LockAllocator<AllocatorType>::allocateBlock( std::size_t size, uint8_t align )
    {
        std::lock_guard<std::mutex> lock( _allocatorMutex );
        return AllocatorType::allocateBlock( size, align );
    }

    template < typename AllocatorType >
    void
    LockAllocator<AllocatorType>::deallocateBlock( void * block )
    {
        std::lock_guard<std::mutex> lock( _allocatorMutex );
        return AllocatorType::deallocateBlock( block );
    }

    template < typename AllocatorType >
    void
    LockAllocator<AllocatorType>::printDebugInfo( std::ostream& out ) const
    {
        // FIXME(dean): fix up this line, will probably break with two threads.
        std::lock_guard<std::mutex> lock( const_cast<std::mutex&>(_allocatorMutex) );
        return AllocatorType::printDebugInfo( out );
    }

    // disable nested lock allocators.
    // FIXME(dean): this does not block compilation when the object is empty parameter initialized. WTF?
    /*template < typename SubAllocatorType >
    class LockAllocator< LockAllocator< SubAllocatorType > >
    {
        template < typename ... ArgTypes >
        explicit LockAllocator< LockAllocator< SubAllocatorType > >( ArgTypes ... args ) = delete;

        explicit LockAllocator< LockAllocator< SubAllocatorType > >() = delete;

        LockAllocator< LockAllocator< SubAllocatorType > >( const LockAllocator<LockAllocator< SubAllocatorType > > & ) = delete;
        LockAllocator< LockAllocator< SubAllocatorType > >( const LockAllocator<LockAllocator< SubAllocatorType > > && ) = delete;
        LockAllocator< LockAllocator< SubAllocatorType > > & operator = ( const LockAllocator< LockAllocator< SubAllocatorType > > & ) = delete;

        virtual void DO_NOT_USE_NESTED_LOCK_ALLOCATORS() = 0;
    };*/

}


#endif // LOCK_ALLOCATOR_H_INCLUDED
