#ifndef PRIMEID_INL_INCLUDED
#define PRIMEID_INL_INCLUDED

std::unordered_map<uintmax_t, uintmax_t> PrimeID::issuedPrimes = std::unordered_map<uintmax_t, uintmax_t>();
FILE* PrimeID::primeFile = NULL;

uintmax_t
PrimeID::nextPrime()
{
    if(primeFile == NULL)
    {//init primes
        primeFile = fopen(PRIME_FILENAME, "r");
        assert(primeFile != NULL);
    }

    //return the next prime
    int nextPrime = 0;
    fread((void*) &nextPrime, sizeof(int), 1, primeFile);//change to sizeof(uintmax_t)
    assert(nextPrime != 0);

    return (uintmax_t) nextPrime;
}

template <class T>
uintmax_t
PrimeID::id(const uintmax_t superID)
{
    assert(superID != 0);

    if(issuedPrimes.count(typeid(T).hash_code()) == 0)//if id has not been issued yet
    {
        //issue a new id into issuedPrimes
        issuedPrimes[typeid(T).hash_code()] = nextPrime();
    }

    uintmax_t id = issuedPrimes[typeid(T).hash_code()];

    id *= superID;

    return id;

}

#endif // PRIMEID_INL_INCLUDED
