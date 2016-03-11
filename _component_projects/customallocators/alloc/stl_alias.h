#ifndef STL_ALIAS_H_INCLUDED
#define STL_ALIAS_H_INCLUDED

#include <vector>
#include <unordered_map>
#include <map>
#include <deque>

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

                inline bool clear_all_memory() { this->clear(); return this->size() == 0; }
                //inline bool clear_all_memory() { return false; }
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

                //inline bool clear_all_memory() { this->clear(); this->shrink_to_fit(); return this->size() == 0; }
                inline bool clear_all_memory() { this->clear(); this->shrink_to_fit(); return false; }
        };



    }
}

#endif // STL_ALIAS_H_INCLUDED
