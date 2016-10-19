#ifndef PROXY_ALLOCATOR_H_INCLUDED
#define PROXY_ALLOCATOR_H_INCLUDED

#include <iostream>

#include "allocator.h"

namespace alloc
{
    class ProxyAllocator : public Allocator // NOTE(dean): by deriving from Allocator we waste memory specifically: sizeof(Allocator)
    {
        private:
            Allocator * _allocator;

        public:
            ProxyAllocator( Allocator * a );
            ~ProxyAllocator();

            virtual inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) { return _allocator->allocateBlock( size, align ); }
            virtual inline void deallocateBlock( void * block ) { _allocator->deallocateBlock( block ); }

            inline void *      getBlock() const override { return _allocator->getBlock(); }
            inline std::size_t getSize()  const override { return _allocator->getSize(); }

            inline std::size_t   usedMemory()   const override { return _allocator->usedMemory(); }
            inline std::size_t unusedMemory()   const override { return _allocator->unusedMemory(); }
            inline std::size_t numAllocations() const override { return _allocator->numAllocations(); }

            inline std::size_t maxUsedMemory()     const override { return _allocator->maxUsedMemory(); }
            inline std::size_t maxNumAllocations() const override { return _allocator->maxNumAllocations(); }

            virtual inline void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            ProxyAllocator( const ProxyAllocator & ) = delete;
            ProxyAllocator & operator = ( const ProxyAllocator & ) = delete;
    };
}

#include "proxy_allocator.inl"

#endif // PROXY_ALLOCATOR_H_INCLUDED
