#ifndef PRIMEID_INL_INCLUDED
#define PRIMEID_INL_INCLUDED

#include <typeinfo>

namespace primeid
{
    extern std::unordered_map< uintmax_t, uintmax_t > issuedPrimes;

    template< class T >
    uintmax_t
    getID( const uintmax_t superID )
    {
        assert( superID != 0 );

        if( issuedPrimes.count( typeid(T).hash_code() ) == 0 ) //if id has not been issued yet
        {
            //issue a new id into issuedPrimes
            issuedPrimes[ typeid(T).hash_code() ] = nextPrime();
        }

        uintmax_t id = issuedPrimes[ typeid(T).hash_code() ];

        id *= superID;

        return id;
    }

    template< class T >
    PrimeIDType
    type()
    {
        return getID<T>( PRIMEID_NO_SUPER_ID );
    }

    template< class T >
    bool
    isType( PrimeIDType type )
    {
        return (T::_prime_id % type == 0);
    }
}

#endif // PRIMEID_INL_INCLUDED
