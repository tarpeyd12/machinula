#include "primeID.h"

#include "primenumbers.h"

namespace primeid
{
    std::unordered_map< uintmax_t, uintmax_t > issuedPrimes;

    uintmax_t nextPrimeIndex = 0;

    PrimeIDType
    nextPrime()
    {
        uintmax_t next_prime_value = __primeNumberData[nextPrimeIndex++];
        assert( next_prime_value != 0 );
        return uintmax_t( next_prime_value );
    }
}
