#ifndef PROXY_ALLOCATOR_INL_INCLUDED
#define PROXY_ALLOCATOR_INL_INCLUDED

// TODO(dean): migrate implementation to cpp files

namespace alloc
{
    inline
    ProxyAllocator::ProxyAllocator(  Allocator * a  )
    : Allocator( 0, nullptr ), _allocator( a )
    {
        assert( a != nullptr );
    }

    inline
    ProxyAllocator::~ProxyAllocator()
    {
        _allocator = nullptr;
    }

    // TODO(dean): ? inline this ?
    void
    ProxyAllocator::printDebugInfo( std::ostream& out ) const
    {
        out << "ProxyAllocator(" << this << "->" << _allocator << "):\n";
        out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        out << "\tBlock End:  " << (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize()) << "\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";
    }
}

#endif // PROXY_ALLOCATOR_INL_INCLUDED
