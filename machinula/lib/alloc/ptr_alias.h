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
                deleter_functor()
                : _allocator( nullptr )
                { }

                deleter_functor( Allocator * a )
                : _allocator( a )
                {
                    assert( _allocator != nullptr );
                }

                deleter_functor( const deleter_functor& df )
                : _allocator( df._allocator )
                { }

                inline
                void
                operator()( PtrType * ptr )
                {
                    assert( ptr != nullptr );
                    _allocator->deallocate< PtrType >( ptr );
                }

                deleter_functor&
                operator=( const deleter_functor& df )
                {
                    _allocator = df._allocator;
                    return *this;
                }
        };

        // NOTE: also used for deallocating arrays
        template < typename PtrType >
        class deleter_block_functor
        {
            private:
                Allocator * _allocator;

            public:
                deleter_block_functor()
                : _allocator( nullptr )
                { }

                deleter_block_functor( Allocator * a )
                : _allocator( a )
                {
                    assert( _allocator != nullptr );
                }

                deleter_block_functor( const deleter_block_functor& df )
                : _allocator( df._allocator )
                { }

                inline
                void
                operator()( PtrType * ptr )
                {
                    assert( ptr != nullptr );
                    _allocator->deallocateBlock( ptr );
                }

                deleter_block_functor&
                operator=( const deleter_block_functor& df )
                {
                    _allocator = df._allocator;
                    return *this;
                }
        };

        namespace alias
        {
            template < typename PtrType >
            using shared_ptr = std::shared_ptr< PtrType >;
        }

        template < typename PtrType >
        class shared_ptr final : public alias::shared_ptr< PtrType >
        {
            public:
                constexpr shared_ptr()
                : alias::shared_ptr< PtrType >( nullptr ) // explicitly construct empty
                { }

                constexpr shared_ptr( std::nullptr_t )
                : alias::shared_ptr< PtrType >( nullptr ) // explicitly construct empty
                { }

                shared_ptr( PtrType * ptr, Allocator * a )
                : alias::shared_ptr< PtrType >( ptr, deleter_functor< PtrType >( a ), stl_adapter< PtrType >( a ) )
                { }

                template< class DerrivedPtrType >
                inline
                void
                reset( DerrivedPtrType * ptr, Allocator * a )
                {
                    alias::shared_ptr< PtrType >::reset( ptr, deleter_functor< PtrType >( a ), stl_adapter< PtrType >( a ) );
                }

            private:
                using alias::shared_ptr< PtrType >::operator=; // TODO(dean): figure out if this is technically necessary for safety.
                using alias::shared_ptr< PtrType >::reset;
                using alias::shared_ptr< PtrType >::swap; // TODO(dean): figure out if this is technically necessary for safety.
        };


        namespace alias
        {
            template < typename PtrType >
            using weak_ptr = std::weak_ptr< PtrType >;
        }

        template < typename PtrType >
        class weak_ptr final : public alias::weak_ptr< PtrType >
        {
            public:
                constexpr weak_ptr()
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

            private:
                using alias::weak_ptr< PtrType >::swap; // TODO(dean): figure out if this is technically necessary for safety.
        };


        namespace alias
        {
            template < typename PtrType >
            using unique_ptr = std::unique_ptr< PtrType, deleter_functor< PtrType > >;
        }

        // TODO(dean): implement the array specialized unique_ptr<T[]>
        template < typename PtrType >
        class unique_ptr final : public alias::unique_ptr< PtrType >
        {
            public:
                // TODO(dean): implement the other constructors for unique_ptr
                constexpr unique_ptr()
                : alias::unique_ptr< PtrType >( nullptr )
                { }

                constexpr unique_ptr( std::nullptr_t )
                : alias::unique_ptr< PtrType >( nullptr )
                { }

                unique_ptr( PtrType * ptr, Allocator * a )
                : alias::unique_ptr< PtrType >( ptr, deleter_functor< PtrType >( a ) )
                { }

            private:
                ;
        };


        template < typename PtrType >
        class unique_ptr< PtrType[] > final : public alias::unique_ptr< PtrType[] >
        {
            public:
                // TODO(dean): implement the other constructors for unique_ptr
                constexpr unique_ptr()
                : alias::unique_ptr< PtrType >( nullptr )
                { }

                constexpr unique_ptr( std::nullptr_t )
                : alias::unique_ptr< PtrType >( nullptr )
                { }

                unique_ptr( PtrType * ptr, Allocator * a )
                : alias::unique_ptr< PtrType >( ptr, deleter_block_functor< PtrType >( a ) )
                { }

            private:
                ;
        };


        //template< class T, typename = std::enable_if/*_t*/<!std::is_array<T>::value> > // disable for array

    }
}

#endif // PTR_ALIAS_H_INCLUDED
