#include "global_allocator.h"

namespace alloc
{
    GlobalAllocator::GlobalAllocator()
    : Allocator( 0, nullptr )
    {

    }

    GlobalAllocator::~GlobalAllocator()
    {
        _num_allocations = 0;
    }

    void
    GlobalAllocator::printDebugInfo( std::ostream& out ) const
    {
        out << "GlobalAllocator(" << this << "):\n";
        //out << "\tBlock Start: " << getBlock() << "\n";
        //out << "\tBlock Size: " << getSize() << " bytes\n";
        //out << "\tBlock End:  " << (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize()) << "\n";
        //out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        //out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        //out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";
    }
}
