#ifndef PTR_ALIAS_H_INCLUDED
#define PTR_ALIAS_H_INCLUDED

#include <memory>

#include "stl_adapter.h"

namespace alloc
{
    namespace ptr
    {
        template < typename PtrType >
        class deleter_functor
        {
            private:
                Allocator * _allocator;

            public:
                deleter_functor( Allocator * a )
                : _allocator( a )
                {
                    assert( a != nullptr );
                }

                inline
                void
                operator()( PtrType * ptr )
                {
                    assert( ptr != nullptr );
                    _allocator->deallocate< PtrType >( ptr );
                }
        };

        namespace alias
        {
            template < typename PtrType >
            using shared_ptr = std::shared_ptr< PtrType >;
        }

        template < typename PtrType >
        class shared_ptr : public alias::shared_ptr< PtrType >
        {
            public:
                shared_ptr()
                : alias::shared_ptr< PtrType >( nullptr ) // explicitly construct empty
                { }

                shared_ptr( std::nullptr_t )
                : alias::shared_ptr< PtrType >( nullptr ) // explicitly construct empty
                { }

                shared_ptr( PtrType * ptr, Allocator * a )
                : alias::shared_ptr< PtrType >( ptr, deleter_functor< PtrType >( a ), stl_adapter< PtrType >( a ) )
                { }

            private:
                using alias::shared_ptr< PtrType >::operator=; // TODO(dean): figure out if this is technically necessary for safety.
        };


        namespace alias
        {
            template < typename PtrType >
            using weak_ptr = std::weak_ptr< PtrType >;
        }

        template < typename PtrType >
        class weak_ptr : public alias::weak_ptr< PtrType >
        {
            public:
                weak_ptr()
                : alias::weak_ptr< PtrType >() // explicitly construct empty
                { }

                weak_ptr( const shared_ptr< PtrType >& r )
                : alias::weak_ptr< PtrType >( r )
                { }

                template < typename DerrivedPtrType >
                weak_ptr( const shared_ptr< DerrivedPtrType >& r )
                : alias::weak_ptr< PtrType >( r )
                { }

                template < typename DerrivedPtrType >
                weak_ptr( const weak_ptr< DerrivedPtrType >& r )
                : alias::weak_ptr< PtrType >( r )
                { }
        };

    }
}

#endif // PTR_ALIAS_H_INCLUDED
