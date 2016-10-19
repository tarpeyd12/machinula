#ifndef DEFAULT_ALLOCATOR_H_INCLUDED
#define DEFAULT_ALLOCATOR_H_INCLUDED

#include "allocator.h"

#include <map>
#include <unordered_map>

namespace alloc
{
    class DefaultAllocator : public Allocator
    {
        private:
            //std::map< uintptr_t, std::size_t > _allocated_blocks; // std::map is technically slower than unordered_map, but has a smaller memory footprint
            std::unordered_map< uintptr_t, std::size_t > _allocated_blocks; // faster than std::map

        public:
            DefaultAllocator( std::size_t block_size = 0, void * block_start = nullptr );
            ~DefaultAllocator();

            virtual void * allocateBlock( std::size_t size, uint8_t align = 0 ) override;
            virtual void deallocateBlock( void * block ) override;

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const override;

            inline void clear();

        private:
            DefaultAllocator() = delete;
            DefaultAllocator( const DefaultAllocator & ) = delete;
            DefaultAllocator & operator = ( const DefaultAllocator & ) = delete;
    };
}

#include "default_allocator.inl"

#endif // DEFAULT_ALLOCATOR_H_INCLUDED
