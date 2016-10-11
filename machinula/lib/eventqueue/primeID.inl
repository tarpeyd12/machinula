#ifndef PRIMEID_INL_INCLUDED
#define PRIMEID_INL_INCLUDED

#include <typeinfo>

namespace primeid
{
    extern std::unordered_map< PrimeIDType, PrimeIDType > issuedPrimes;

    template< class T >
    PrimeIDType
    getID( const PrimeIDType superID )
    {
        assert( superID != 0 );

        PrimeIDType idType = PrimeIDType( typeid(T).hash_code() ); // TODO(dean): check if typeid(T).hash_code() is fast

        if( issuedPrimes.count( idType ) == 0 ) // if id has not been issued yet
        {
            // issue a new id into issuedPrimes
            return ( issuedPrimes[ idType ] = nextPrime() ) * superID;
        }

        return issuedPrimes[ idType ] * superID;
    }

    template< class T >
    PrimeIDType
    type()
    {
        return getID< T >( PRIMEID_NO_SUPER_ID );
    }

    template< class T >
    bool
    isType( PrimeIDType type )
    {
        return ( T::_prime_id % type == 0 );
    }
}

#endif // PRIMEID_INL_INCLUDED
