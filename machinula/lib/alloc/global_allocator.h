#ifndef GLOBAL_ALLOCATOR_H_INCLUDED
#define GLOBAL_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class GlobalAllocator : public Allocator
    {
        private:

        public:
            GlobalAllocator();
            ~GlobalAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            GlobalAllocator( const GlobalAllocator & ) = delete;
            GlobalAllocator & operator = ( const GlobalAllocator & ) = delete;
    };
}

#include "global_allocator.inl"

#endif // GLOBAL_ALLOCATOR_H_INCLUDED
