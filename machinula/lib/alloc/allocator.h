#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

//#include <iostream>

#include <type_traits>
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

            std::size_t _max_used_memory;
            std::size_t _max_num_allocations;

        public:

            Allocator( std::size_t block_size, void * block_start )
            : _block_start(block_start), _block_size(block_size), _used_memory(0), _num_allocations(0), _max_used_memory(0), _max_num_allocations(0)
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
            inline void deallocate( void * block ) final { if( std::is_destructible<Type>() ) { ((Type*)block)->~Type(); } this->deallocateBlock( block ); }

            virtual inline void *      getBlock() const { return _block_start; }
            virtual inline std::size_t getSize()  const { return _block_size; }

            virtual inline std::size_t   usedMemory()   const { return _used_memory; }
            virtual inline std::size_t unusedMemory()   const { return _block_size - _used_memory; }
            virtual inline std::size_t numAllocations() const { return _num_allocations; }

            virtual inline std::size_t maxUsedMemory()     const { return _max_used_memory; }
            virtual inline std::size_t maxNumAllocations() const { return _max_num_allocations; }

            virtual void printDebugInfo( std::ostream& out = std::cerr ) const = 0;

        protected:
            inline void _incrementAllocations( std::size_t size ) { assert( size > 0 ); _max_num_allocations = std::max( _max_num_allocations, ++_num_allocations ); _max_used_memory = std::max( _max_used_memory, _used_memory += size ); }
            inline void _decrementAllocations( std::size_t size ) { assert( size > 0 ); --_num_allocations; _used_memory -= size; }
        private:
            Allocator( const Allocator & ) = delete;
            Allocator & operator = ( const Allocator & ) = delete;
    };
}

#endif // ALLOCATOR_H_INCLUDED
