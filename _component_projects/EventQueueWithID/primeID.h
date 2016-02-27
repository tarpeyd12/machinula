#ifndef PRIMEID_H_INCLUDED
#define PRIMEID_H_INCLUDED
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <cassert>

#define PRIMEID_NO_SUPER_ID 1

/*
#define __primeid_isType(_class) bool isType( PrimeIDType type ) { return primeid::isType<_class>(type); }
#define __primeid_isType_operator(_class) bool operator==( PrimeIDType type ) { return primeid::isType<_class>(type); }

#define primeid_Identifier(_class) static primeid::PrimeIDType _prime_id; __primeid_isType(_class) __primeid_isType_operator(_class)
#define primeid_initIdentifier(_class) primeid::PrimeIDType _class::_prime_id = primeid::getID<_class>();
#define primeid_initIdentifier(_class,_parent) primeid::PrimeIDType _class::_prime_id = primeid::getID<_class>(_parent::_prime_id);
*/

namespace primeid
{
    typedef uintmax_t PrimeIDType;

    PrimeIDType nextPrime();

    // pass the ID of the "superclass", null if none
    template< class T > PrimeIDType getID( const PrimeIDType superID = PRIMEID_NO_SUPER_ID );

    template< class T > inline PrimeIDType type();
    template< class T > inline bool isType( PrimeIDType type );
}


#include "primeID.inl"

#endif // PRIMEID_H_INCLUDED
