#ifndef RANDOM_FUNCTOR_H_INCLUDED
#define RANDOM_FUNCTOR_H_INCLUDED

#include <algorithm>
#include <random>
#include <mutex>

namespace Rand
{
    class RandomFunctor
    {
        public:
            virtual ~RandomFunctor() {};

            // NOTE(dean): the compiler will give a warning that Float(double,double) is used but not defined because it's inline, but removing the inline flag here will slow it down significantly.
            inline virtual double Float( double min, double max ) = 0; // [min,max)
            inline virtual unsigned Int( unsigned min, unsigned max ) = 0; // [min,max]

            inline
            double
            operator()( double min = -1.0, double max = 1.0 )
            {
                return Float( min, max );
            }

    };


    // TODO(dean): template the generator type
    class Random_Safe : public RandomFunctor
    {
        private:
            std::mutex randLock;

            std::mt19937 generator;
            //std::mt19937_64 generator;

        public:
            Random_Safe()
            : randLock(), generator()
            {
                std::random_device rd;
                generator.seed( rd() );
            }

            Random_Safe( unsigned _seed )
            : randLock(), generator( _seed )
            {

            }

            virtual ~Random_Safe() {}

            inline
            double
            Float( double min = 0.0, double max = 1.0 ) override
            {
                std::lock_guard<std::mutex> lock( randLock );
                return std::uniform_real_distribution<double>( min, max )( generator );
            }

            inline
            unsigned
            Int( unsigned min, unsigned max ) override
            {
                std::lock_guard<std::mutex> lock( randLock );
                return std::uniform_int_distribution<unsigned>( min, max )( generator );
            }
    };


    extern Random_Safe __random;

    inline
    double
    Float( double min = 0.0, double max = 1.0 )
    {
        return __random.Float( min, max );
    }

    inline
    unsigned
    Int( unsigned min = 0, unsigned max = RAND_MAX-1 )
    {
        return __random.Int( min, max );
    }
}

#endif // RANDOM_FUNCTOR_H_INCLUDED
