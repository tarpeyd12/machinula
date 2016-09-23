#ifndef STL_ADAPTER_H_INCLUDED
#define STL_ADAPTER_H_INCLUDED

#define ___ALLOC_STL_ADAPTER_DEBUGPRINT 0

namespace alloc
{
    namespace stl
    {

        // TODO(dean): instead of inheriting from std::allocator just implement the required functionality.
        template < typename Type >
        class adapter : public std::allocator< Type >
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
                    typedef adapter< OtherType > other;
                };

                adapter( Allocator * _al ) throw();
                adapter( const adapter< Type > &o ) throw();

                inline Allocator * __getInternalAllocator() const { return _allocator; }

                template < class OtherType >
                adapter( const adapter< OtherType > &o ) throw()
                : std::allocator<Type>(), _allocator( o.__getInternalAllocator() )
                {
                    #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
                    std::cerr << "stl::adapter< Type >::stl::adapter( const stl::adapter< OtherType > &o )\n";
                    #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
                }

                ~adapter() throw();

                adapter< Type > & operator= ( const adapter< Type > & o );

                pointer allocate( size_type n, const void * hint = 0 );
                void deallocate( pointer p, size_type n );

        };

        template < typename Type >
        adapter< Type >::adapter( Allocator * _al ) throw()
        : std::allocator<Type>(), _allocator( _al )
        {
            #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
            std::cerr << "stl::adapter< Type >::stl::adapter( Allocator * _al )\n";
            #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
        }

        template < typename Type >
        adapter< Type >::adapter( const adapter< Type > &o ) throw()
        : std::allocator<Type>(o), _allocator( o._allocator )
        {
            #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
            std::cerr << "stl::adapter< Type >::stl::adapter( const stl::adapter< Type > &o )\n";
            #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
        }

        template < typename Type >
        adapter< Type >::~adapter() throw()
        {
            #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
            std::cerr << "stl::adapter< Type >::~stl::adapter()\n";
            #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
            _allocator = nullptr;
        }

        template < typename Type >
        adapter< Type > &
        adapter< Type >::operator= ( const adapter< Type > & o )
        {
            _allocator = o.__getInternalAllocator();
            return *this;
        }

        template < typename Type >
        typename adapter< Type >::pointer
        adapter< Type >::allocate( adapter< Type >::size_type n, const void * /*hint*/ )
        {
            #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
            std::cerr << "Alloc " << n*sizeof(Type) << " bytes, over " << n << ".\n";
            #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
            //return std::allocator<Type>::allocate( n, hint );
            return reinterpret_cast<Type*>( _allocator->allocateBlock( n*sizeof(Type), alignof(Type) ) );
        }

        template < typename Type >
        void
        adapter< Type >::deallocate( pointer p, adapter< Type >::size_type /*n*/ )
        {
            #if ___ALLOC_STL_ADAPTER_DEBUGPRINT
            std::cerr << "Dealloc "  << n*sizeof(Type) << " bytes (" << p << ").\n";
            #endif // ___ALLOC_STL_ADAPTER_DEBUGPRINT
            //return std::allocator<T>::deallocate(p, n);
            return _allocator->deallocateBlock( p );
        }

        template< class T1, class T2 >
        bool
        operator==( const adapter<T1>& lhs, const adapter<T2>& rhs )
        {
            return lhs.__getInternalAllocator() == rhs.__getInternalAllocator();
        }

        template< class T1, class T2 >
        bool
        operator!=( const adapter<T1>& lhs, const adapter<T2>& rhs )
        {
            return lhs.__getInternalAllocator() != rhs.__getInternalAllocator();
        }

    }

    template < typename Type >
    using stl_adapter = stl::adapter< Type >;
}

#endif // STL_ADAPTER_H_INCLUDED
