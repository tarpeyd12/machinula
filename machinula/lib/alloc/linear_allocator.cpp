#include "linear_allocator.h"

namespace alloc
{
    LinearAllocator::LinearAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _next_free_block(block_start)
    {
        assert( block_size > 0 );
    }

    LinearAllocator::~LinearAllocator()
    {
        _next_free_block = nullptr;
    }

    void *
    LinearAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size > 0 );

        uint8_t adjustment = align::forwardAdj( _next_free_block, align );

        if( _block_size - size < _used_memory + adjustment )
            return nullptr;

        uintptr_t address = uintptr_t(_next_free_block) + adjustment;

        _next_free_block = (void*)( address + size );

        _incrementAllocations( size + adjustment );

        return (void*)address;
    }

    void
    LinearAllocator::printDebugInfo( std::ostream& out ) const
    {
        out << "LinearAllocator(" << this << "):\n";
        out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        out << "\tBlock End:  " << (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize()) << "\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";

        if(true)
        {
            out << "{\n";

            out << "\tused:\n\t{\n";
            out << "\t\taddr:" << _block_start << ",size:" << reinterpret_cast<uintptr_t>(_next_free_block)-reinterpret_cast<uintptr_t>(_block_start) << ",end:" << _next_free_block << "\n";
            out << "\t}\n";

            out << "\tfree:\n\t{\n";
            out << "\t\taddr:" << _next_free_block << ",size:" << _block_size-reinterpret_cast<uintptr_t>(_next_free_block)-reinterpret_cast<uintptr_t>(_block_start) << ",end:" << (void*)(reinterpret_cast<uintptr_t>(_block_start)+_block_size) << "\n";
            out << "\t}\n";

            out << "};\n";
        }
    }
}
