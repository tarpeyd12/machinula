#ifndef GLOBAL_ALLOCATOR_H_INCLUDED
#define GLOBAL_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class GlobalAllocator final : public Allocator
    {
        private:

        public:
            GlobalAllocator();
            ~GlobalAllocator() = default;

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            void printDebugInfo( std::ostream& out = std::cerr ) const override;

            GlobalAllocator & operator = ( const GlobalAllocator & ) = default;

        private:
            GlobalAllocator( const GlobalAllocator & ) = delete;
    };
}

#include "global_allocator.inl"

#endif // GLOBAL_ALLOCATOR_H_INCLUDED
