#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include <cassert>

namespace alloc
{
    class Allocator
    {
        protected:
            void *      _block_start;
            std::size_t _block_size;

            std::size_t _used_memory;
            std::size_t _num_allocations;

        public:

            Allocator( std::size_t block_size, void * block_start )
            : _block_start(block_start), _block_size(block_size), _used_memory(0), _num_allocations(0)
            { }

            virtual ~Allocator()
            {
                assert( _used_memory == 0 && _num_allocations == 0 );
                _block_size = 0;
                _block_start = nullptr;
            }

            virtual void * allocateBlock( std::size_t size, uint8_t align = 0 ) = 0;
            virtual void deallocateBlock( void * block ) = 0;

            template < class Type >
            inline Type * allocate() final { return (Type*)this->allocateBlock( sizeof(Type), alignof(Type) ); }

            template < class Type >
            inline void deallocate( void * block ) final { ((Type*)block)->~Type(); deallocateBlock( block ); }

            inline void *      getBlock() const { return _block_start; }
            inline std::size_t getSize()  const { return _block_size; }

            inline std::size_t   usedMemory()   const { return _used_memory; }
            inline std::size_t unusedMemory()   const { return _block_size - _used_memory; }
            inline std::size_t numAllocations() const { return _num_allocations; }
    };
}

#endif // ALLOCATOR_H_INCLUDED
