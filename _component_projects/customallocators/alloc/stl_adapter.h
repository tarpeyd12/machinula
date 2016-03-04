#ifndef STL_ADAPTER_H_INCLUDED
#define STL_ADAPTER_H_INCLUDED

namespace alloc
{
    template < typename Type >
    class stl_adapter : public std::allocator< Type >
    {
        private:
            Allocator * _allocator;

        public:
            typedef std::size_t size_type;
            typedef Type * pointer;
            typedef const Type * const_pointer;

            template < typename OtherType >
            struct rebind
            {
                typedef stl_adapter< OtherType > other;
            };

            pointer allocate( size_type n, const void * hint = 0 );
            void deallocate( pointer p, size_type n );

            stl_adapter( Allocator * _al ) throw();
            stl_adapter( const stl_adapter< Type > &o ) throw();

            template < class OtherType >
            stl_adapter( const stl_adapter< OtherType > &o ) throw() : std::allocator<Type>(o) { _allocator = o._allocator; }

            ~stl_adapter() throw();

    };

    template < typename Type >
    stl_adapter< Type >::stl_adapter( Allocator * _al ) throw()
    : std::allocator<Type>()
    {
        _allocator = _al;
        fprintf(stderr, "Hello allocator!\n");
    }

    template < typename Type >
    stl_adapter< Type >::stl_adapter( const stl_adapter< Type > &o ) throw()
    : std::allocator<Type>(o)
    {
        _allocator = o._allocator;
    }

    template < typename Type >
    stl_adapter< Type >::~stl_adapter() throw()
    {
        _allocator = nullptr;
    }

    template < typename Type >
    typename stl_adapter< Type >::pointer
    stl_adapter< Type >::allocate( stl_adapter< Type >::size_type n, const void * hint )
    {
        //assert( n % sizeof(Type) == 0 );
        fprintf(stderr, "Alloc %d bytes.\n", n*sizeof(Type));
        //return std::allocator<Type>::allocate( n, hint );
        //return _allocator->allocate<Type>( n );
        return reinterpret_cast<Type*>( _allocator->allocateBlock( n, alignof(Type) ) );
    }

    template < typename Type >
    void
    stl_adapter< Type >::deallocate( pointer p, stl_adapter< Type >::size_type n )
    {
        //assert( n % sizeof(Type) == 0 );
        fprintf(stderr, "Dealloc %d bytes (%p).\n", n*sizeof(Type), p);
        //return std::allocator<T>::deallocate(p, n);
        //return _allocator->deallocate<Type>( p );
        return _allocator->deallocateBlock( p );
    }


}

#endif // STL_ADAPTER_H_INCLUDED
