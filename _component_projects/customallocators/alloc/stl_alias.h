#ifndef STL_ALIAS_H_INCLUDED
#define STL_ALIAS_H_INCLUDED

#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <queue>

#include "stl_adapter.h"

namespace alloc
{
    namespace stl
    {


        namespace alias
        {
            template < typename Type >
            using vector = std::vector< Type, stl_adapter<Type> >;
        }

        template < typename Type >
        class vector : public alias::vector< Type >
        {
            public:
                vector( Allocator * a = nullptr )
                : std::vector< Type,stl_adapter<Type> >( stl_adapter<Type>(a) )
                { }

                // clear the data, attempt to shrink the array to fit. return true if successful. shrink_to_fit may do nothing depending on the library.
                inline bool clear_all_memory() { this->clear(); this->shrink_to_fit(); return this->capacity() == 0; }
        };



        namespace alias
        {
            template < typename KeyType, typename ValueType >
            using unordered_map = std::unordered_map< KeyType, ValueType, std::hash<KeyType>, std::equal_to<KeyType>, stl_adapter< std::pair< const KeyType, ValueType > > >;
        }

        template < typename KeyType, typename ValueType >
        class unordered_map : public alias::unordered_map< KeyType, ValueType >
        {
            public:
                unordered_map( Allocator * a = nullptr )
                : alias::unordered_map< KeyType, ValueType >( 1, std::hash<KeyType>(), std::equal_to<KeyType>(), stl_adapter< std::pair< const KeyType, ValueType > >(a) )
                { }

                // clear the table, return false. unordered_map keeps the hash table and does not free it until deconstruction.
                inline bool clear_all_memory() { this->clear(); return false; }
        };



        namespace alias
        {
            template < typename KeyType, typename ValueType >
            using map = std::map< KeyType, ValueType, std::less<KeyType>, stl_adapter< std::pair< const KeyType, ValueType > > >;
        }

        template < typename KeyType, typename ValueType >
        class map : public alias::map< KeyType, ValueType >
        {
            public:
                map( Allocator * a = nullptr )
                : alias::map< KeyType, ValueType >( std::less<KeyType>(), stl_adapter< std::pair< const KeyType, ValueType > >(a) )
                { }

                // clear all, then return size of internal tree, map *should* scale directly in memory usage with tree size.
                inline bool clear_all_memory() { this->clear(); return this->size() == 0; }
        };



        namespace alias
        {
            template < typename Type >
            using deque = std::deque< Type, stl_adapter< Type > >;
        }

        template < typename Type >
        class deque : public alias::deque< Type >
        {
            public:
                deque( Allocator * a = nullptr )
                : alias::deque< Type >( stl_adapter< Type >(a) )
                { }

                // clear as much data as we can, then return false, deque apparently keeps a small table at all times after construction.
                inline bool clear_all_memory() { this->clear(); this->shrink_to_fit(); return false; }
        };



        namespace alias
        {
            template < typename Type >
            using queue = std::queue< Type, std::deque< Type, stl_adapter< Type > > >;
        }

        template < typename Type >
        class queue : public alias::queue< Type >
        {
            public:
                queue( Allocator * a = nullptr )
                : alias::queue< Type >( stl_adapter< Type >(a) )
                { }

                // clear nothing then return false. queue should not be used for clear-able lists, use deque instead.
                inline bool clear_all_memory() { return false; }
        };


    }
}

#endif // STL_ALIAS_H_INCLUDED
