#ifndef PROXY_ALLOCATOR_H_INCLUDED
#define PROXY_ALLOCATOR_H_INCLUDED

#include <iostream>

#include "allocator.h"

namespace alloc
{
    class ProxyAllocator final : public Allocator
    {
        private:
            Allocator * _allocator;

        public:
            ProxyAllocator( Allocator * a );
            ~ProxyAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) { return _allocator->allocateBlock( size, align ); };
            inline void deallocateBlock( void * block ) { _allocator->deallocateBlock( block ); };

            inline void *      getBlock() const override { return _allocator->getBlock(); }
            inline std::size_t getSize()  const override { return _allocator->getSize(); }

            inline std::size_t   usedMemory()   const override { return _allocator->usedMemory(); }
            inline std::size_t unusedMemory()   const override { return _allocator->unusedMemory(); }
            inline std::size_t numAllocations() const override { return _allocator->numAllocations(); }

            inline std::size_t maxUsedMemory()     const override { return _allocator->maxUsedMemory(); }
            inline std::size_t maxNumAllocations() const override { return _allocator->maxNumAllocations(); }

            void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            ProxyAllocator( const ProxyAllocator & ) = delete;
            ProxyAllocator & operator = ( const ProxyAllocator & ) = delete;
    };

    ProxyAllocator::ProxyAllocator(  Allocator * a  )
    : Allocator( 0, nullptr ), _allocator( a )
    {
        assert( a != nullptr );
    }

    ProxyAllocator::~ProxyAllocator()
    {
        _allocator = nullptr;
    }

    void
    ProxyAllocator::printDebugInfo( std::ostream& out ) const
    {
        out << "ProxyAllocator(" << this << ":" << _allocator << "):\n";
        out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";
    }
}

#endif // PROXY_ALLOCATOR_H_INCLUDED
