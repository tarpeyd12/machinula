#ifndef POOL_ALLOCATOR_H_INCLUDED
#define POOL_ALLOCATOR_H_INCLUDED

#include "allocator.h"

namespace alloc
{
    class PoolAllocator : public Allocator
    {
        private:
            std::size_t _object_size;
            uint8_t _object_alignment;

            void ** _free_block_list;

        public:
            PoolAllocator( std::size_t object_size, uint8_t object_align, std::size_t block_size, void * block_start );
            ~PoolAllocator();

            inline void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            inline void deallocateBlock( void * block ) override;

            void printDebugInfo( std::ostream& out = std::cerr ) const override;

        private:
            PoolAllocator( const PoolAllocator & ) = delete;
            PoolAllocator & operator = ( const PoolAllocator & ) = delete;
    };

    PoolAllocator::PoolAllocator( std::size_t object_size, uint8_t object_align, std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start ), _object_size(object_size), _object_alignment(object_align), _free_block_list(nullptr)
    {
        /*if( _object_size < sizeof(void*) )
            _object_size = sizeof(void*);*/

        assert( _object_size >= sizeof(void*) );

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

    inline
    void *
    PoolAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size == _object_size && align == _object_alignment );

        if( nullptr == _free_block_list  )
            return nullptr;

        // get the pointer that the _free_block_list is pointing to( this is our memory block)
        void * block = (void*)_free_block_list;

        // set the _free_block_list to the pointer that the data that _free_block_list is pointing to
        _free_block_list = (void**)(*_free_block_list);

        _incrementAllocations( _object_size );

        return block;
    }

    inline
    void
    PoolAllocator::deallocateBlock( void * block )
    {
        assert( nullptr != block );

        // set the data at block to point to the same address as _free_block_list
        *((void**)block) = _free_block_list;

        // set _free_block_list to point to the same address as the given block
        _free_block_list = (void**)block;

        _decrementAllocations( _object_size );
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

#endif // POOL_ALLOCATOR_H_INCLUDED
