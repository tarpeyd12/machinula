#ifndef DEFAULT_ALLOCATOR_H_INCLUDED
#define DEFAULT_ALLOCATOR_H_INCLUDED

#include "allocator.h"

//#include <map>
#include <unordered_map>

namespace alloc
{
    class DefaultAllocator final : public Allocator
    {
        private:
            //std::map< void *, std::size_t > _allocated_blocks;
            std::unordered_map< void *, std::size_t > _allocated_blocks;

        public:
            DefaultAllocator( std::size_t block_size = 0, void * block_start = 0 );
            ~DefaultAllocator();

            void * allocateBlock( std::size_t size, uint8_t align ) override;
            void deallocateBlock( void * block ) override;

            void clear();

        private:
            DefaultAllocator( const DefaultAllocator & ) = delete;
            DefaultAllocator & operator = ( const DefaultAllocator & ) = delete;
    };

    DefaultAllocator::DefaultAllocator( std::size_t block_size, void * block_start )
    : Allocator( block_size, block_start )
    { }

    DefaultAllocator::~DefaultAllocator()
    {
        assert( _allocated_blocks.empty() );
    }

    void *
    DefaultAllocator::allocateBlock( std::size_t size, uint8_t align )
    {
        assert( size > 0 );

        if( _block_size > 0 && _block_size - size < _used_memory )
            return nullptr;

        _incrementAllocations( size );

        void * block = malloc( size );

        _allocated_blocks.insert( std::pair<void*,std::size_t>(block, size) );

        return block;
    }

    void
    DefaultAllocator::deallocateBlock( void * block )
    {
        assert( block != nullptr );

        auto it = _allocated_blocks.find( block );

        assert( it != _allocated_blocks.end() );

        _decrementAllocations( it->second );

        _allocated_blocks.erase( it );

        free( block );
    }

    void
    DefaultAllocator::clear()
    {
        while( _allocated_blocks.size() )
        {
            deallocateBlock( _allocated_blocks.begin()->first );
        }
    }
}

#endif // DEFAULT_ALLOCATOR_H_INCLUDED
