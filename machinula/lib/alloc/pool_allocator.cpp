#include "align.h"

#include "pool_allocator.h"

namespace alloc
{
    PoolAllocator::PoolAllocator( std::size_t object_size, uint8_t object_align, std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _object_size(object_size), _object_alignment(object_align), _free_block_list(nullptr)
    {
        // TODO(dean): make a function to re-initialize the empty bucketspace as a way to defragment the pool.

        // make sure the programmer did not do something stupid
        assert( _object_size >= sizeof(void*) );

        // fail gracefully if the programmer is dumb and always works in release mode
        if( _object_size < sizeof(void*) )
        {
            _object_size = sizeof(void*);
        }

        uint8_t adjust = align::forwardAdj( _block_start, _object_alignment );

        _free_block_list = (void**)align::_add( _block_start, adjust );

        std::size_t num_objects = ( _block_size - adjust ) / _object_size - 1;

        void ** ptemp = _free_block_list;

        for( std::size_t i = 0; i < num_objects; ++i )
        {
            // set the data at ptemp to point to memory _object_size away.
            *ptemp = align::_add( ptemp, _object_size );

            // point ptemp to the address that the data at ptemp is pointing to.
            ptemp = (void**)(*ptemp);
        }

        // set the last item to point to null.
        *ptemp = nullptr;
    }

    PoolAllocator::~PoolAllocator()
    {
        _free_block_list = nullptr;
    }

    void
    PoolAllocator::printDebugInfo( std::ostream& out ) const
    {
        void * pend;
        out << "PoolAllocator(" << this << "):\n";
        out << "\tBlock Start: " << getBlock() << "\n";
        out << "\tBlock Size: " << getSize() << " bytes\n";
        out << "\tBlock End:  " << (pend = (void*)(reinterpret_cast<uintptr_t>(getBlock())+getSize())) << "\n";
        out << "\tUsed Memory: " << usedMemory() << " bytes\n";
        out << "\tUnused Memory: " << unusedMemory() << " bytes\n";
        out << "\tNumber of Allocations: " << numAllocations() << "\n";
        out << "\tMax Used Memory: " << maxUsedMemory() << " bytes\n";
        out << "\tMax Number of Allocations: " << maxNumAllocations() << "\n";
        out << "\tNumber of Buckets: " << (( _block_size - align::forwardAdj( _block_start, _object_alignment ) ) / _object_size - 1) << "\n";

        if(true)
        {
            out << "{\n";
            out << "\tfree:\n\t{\n";
            std::size_t numFreeBuckets = 0;
            void ** fbl = _free_block_list;
            while( fbl != nullptr && (void*)fbl >= _block_start && (void*)fbl < pend )
            {
                void * block = (void*)fbl;
                void ** next = (void**)(*fbl);

                out << "\t\taddr:" << block << ",size:" << _object_size << ",end:" << (void*)(reinterpret_cast<uintptr_t>(block)+_object_size) << ",next:" << next << "\n";

                // point fbl to the address that the data at fbl is pointing to.
                fbl = (void**)(*fbl);
                ++numFreeBuckets;
                //if( numFreeBuckets >= 100 ) break;
            }

            out << "\t}(freeBuckets:" << numFreeBuckets << ");\n";
            out << "};\n";

        }
    }
}
