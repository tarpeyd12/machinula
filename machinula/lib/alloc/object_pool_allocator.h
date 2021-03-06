#ifndef OBJECT_POOL_ALLOCATOR_H_INCLUDED
#define OBJECT_POOL_ALLOCATOR_H_INCLUDED

#include "pool_allocator.h"

namespace alloc
{
    template < typename Type >
    class ObjectPoolAllocator : public PoolAllocator
    {
        public:
            ObjectPoolAllocator( std::size_t block_size, void * block_start );

        private:
            ObjectPoolAllocator( const ObjectPoolAllocator & ) = delete;
            ObjectPoolAllocator & operator = ( const ObjectPoolAllocator & ) = delete;
    };

    template < typename Type >
    inline
    ObjectPoolAllocator<Type>::ObjectPoolAllocator( std::size_t block_size, void * block_start )
    : PoolAllocator( sizeof(Type), alignof(Type), block_size, block_start )
    {  }
}

#endif // OBJECT_POOL_ALLOCATOR_H_INCLUDED
