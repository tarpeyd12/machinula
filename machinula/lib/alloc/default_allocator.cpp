#include "default_allocator.h"

namespace alloc
{
    DefaultAllocator::DefaultAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _allocated_blocks()
    { }

    DefaultAllocator::~DefaultAllocator()
    {
        assert( _allocated_blocks.empty() );
    }

    void *
    DefaultAllocator::allocateBlock( std::size_t size, uint8_t /*align*/ )
    {
        assert( size > 0 );

        if( _block_size > 0 && _block_size - size < _used_memory )
        {
            return nullptr;
        }

        _incrementAllocations( size );

        void * block = ::operator new( size );

        _allocated_blocks.insert( std::pair<uintptr_t,std::size_t>( reinterpret_cast<uintptr_t>(block), size ) );

        return block;
    }

    void
    DefaultAllocator::deallocateBlock( void * block )
    {
        assert( block != nullptr );

        auto it = _allocated_blocks.find( reinterpret_cast<uintptr_t>(block) );

        assert( it != _allocated_blocks.end() );

        _decrementAllocations( it->second );

        _allocated_blocks.erase( it );

        ::operator delete( block );
    }

    void
    DefaultAllocator::printDebugInfo( std::ostream& out ) const
    {
        out << "DefaultAllocator(" << this << "):\n";
        //out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        //out << "\tBlock End:  " << (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize()) << "\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";

        if(true)
        {
            out << "{\n";
            out << "\tused:\n\t{\n";
            for( auto c : _allocated_blocks )
            {
                out << "\t\taddr:" << (void*)c.first << ",size:" << c.second << ",end:" << (void*)(c.first+c.second) << "\n";
            }
            out << "\t}(allocatedBlocks:" << _allocated_blocks.size() << ");\n";
            out << "};\n";
        }
    }
}
