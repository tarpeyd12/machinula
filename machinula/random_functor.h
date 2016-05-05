#ifndef RANDOM_FUNCTOR_H_INCLUDED
#define RANDOM_FUNCTOR_H_INCLUDED

#include <algorithm>
#include <random>

namespace Rand
{
    // TODO: Figure out how to make this templated with the mutex working in Random_FastSafe

    class RandomFunctor
    {
        public:
            virtual ~RandomFunctor() {};

            inline virtual double Float(void) = 0; // [0,1)
            inline virtual unsigned Int( unsigned min, unsigned max ) = 0; // [min,max]

            inline
            double
            operator()()
            {
                //return Float()-Float();
                return Float()*2.0-1.0;
            }

    };


    class Random_FastSafe : public RandomFunctor
    {
        private:
            //static std::mutex randLock;

            std::mt19937 generator;
            //std::mt19937_64 generator;

        public:
            Random_FastSafe() : generator()
            {
                std::random_device rd;
                generator.seed( rd() );
            }

            Random_FastSafe( unsigned _seed ) : generator(_seed)
            {

            }

            virtual ~Random_FastSafe() {}

            inline
            double
            Float(void)
            {
                return std::uniform_real_distribution<double>(0.0,1.0)( generator );
            }

            inline
            unsigned
            Int( unsigned min, unsigned max )
            {
                return std::uniform_int_distribution<unsigned>( min, max )( generator );
            }
    };


    extern Random_FastSafe __random;

    inline
    double
    Float()
    {
        return __random.Float();
    }

    inline
    unsigned
    Int( unsigned min = 0, unsigned max = RAND_MAX-1 )
    {
        return __random.Int( min, max );
    }
}

#endif // RANDOM_FUNCTOR_H_INCLUDED
