#ifndef PTR_ALIAS_H_INCLUDED
#define PTR_ALIAS_H_INCLUDED

#include <memory>

#include "stl_adapter.h"

namespace alloc
{
    namespace ptr
    {
        template < typename PtrType >
        struct deleter_functor
        {
            Allocator * _allocator;

            deleter_functor( Allocator * a )
            : _allocator( a )
            { assert( a != nullptr ); }

            inline
            void
            operator()( PtrType * ptr )
            {
                _allocator->deallocate<PtrType>( ptr );
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
                shared_ptr( PtrType * ptr, Allocator * a )
                : alias::shared_ptr< PtrType >( ptr, deleter_functor<PtrType>( a ) , stl_adapter< PtrType >( a ) )
                { }
        };

    }
}

#endif // PTR_ALIAS_H_INCLUDED
