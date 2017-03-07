#ifndef LOCK_ALLOCATOR_H_INCLUDED
#define LOCK_ALLOCATOR_H_INCLUDED

#include "allocator.h"

#include <mutex>

namespace alloc
{
    /*template < typename AllocatorType, typename _Enabled = void >
    class LockAllocator;

    template < typename AllocatorType >
    class LockAllocator< AllocatorType, typename std::enable_if< std::is_base_of< Allocator, AllocatorType >::value >::type > final : public AllocatorType
    */

    template < typename AllocatorType >
    class LockAllocator final : public AllocatorType
    {
        private:
            std::mutex _allocatorMutex;

        public:

            template < typename ... ArgTypes >
            explicit LockAllocator( ArgTypes ... args ) : AllocatorType( args... ), _allocatorMutex() { }
            ~LockAllocator() = default;

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            inline void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            LockAllocator( const LockAllocator & ) = delete;
            LockAllocator & operator = ( const LockAllocator & ) = delete;
    };

    // disable nested lock allocators.
    template < typename SubAllocatorType >
    class LockAllocator< LockAllocator< SubAllocatorType > >
    {
        template < typename ... ArgTypes >
        explicit LockAllocator< LockAllocator< SubAllocatorType > >( ArgTypes ... args ) = delete;

        explicit LockAllocator< LockAllocator< SubAllocatorType > >() = delete;

        LockAllocator< LockAllocator< SubAllocatorType > >( const LockAllocator<LockAllocator< SubAllocatorType > > & ) = delete;
        LockAllocator< LockAllocator< SubAllocatorType > >( const LockAllocator<LockAllocator< SubAllocatorType > > && ) = delete;
        LockAllocator< LockAllocator< SubAllocatorType > > & operator = ( const LockAllocator< LockAllocator< SubAllocatorType > > & ) = delete;

        virtual void DO_NOT_USE_NESTED_LOCK_ALLOCATORS() = 0;
    };

}

#include "lock_allocator.inl"

#endif // LOCK_ALLOCATOR_H_INCLUDED
