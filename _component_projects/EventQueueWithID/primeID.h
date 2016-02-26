#ifndef PRIMEID_H_INCLUDED
#define PRIMEID_H_INCLUDED
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <cassert>

#define PRIME_FILENAME "primes.num"
#define NO_SUPER_ID 1

class PrimeID{
private:
    static std::unordered_map<uintmax_t, uintmax_t> issuedPrimes;

    static uintmax_t nextPrime();

    static FILE* primeFile;

public:
    template <class T> static uintmax_t id(const uintmax_t superID);//pass the ID of the superclass, null if none
    template <class T> static inline uintmax_t type() { return id<T>(NO_SUPER_ID); };
	template <class T> static bool isType(uintmax_t type) { return (T::id % type == 0); };
};

#include "primeID.inl"

#endif // PRIMEID_H_INCLUDED
