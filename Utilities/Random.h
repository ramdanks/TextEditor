#pragma once
#include <stdint.h>

namespace Util
{
    class Random
    {
    public:
        // http://xoroshiro.di.unimi.it/xoroshiro128plus.c
        static double UniformDouble()
        {
            return next() * ( 1.0 / 18446744073709551616.0 );
        }

        static int Uniform()
        {
            return next();
        }

        // https://en.wikipedia.org/wiki/Xorshift
        static int Xorshf96()
        {
            static unsigned long x = 123456789, y = 362436069, z = 521288629;
            int t;

            x ^= x << 16;
            x ^= x >> 5;
            x ^= x << 1;

            t = x;
            x = y;
            y = z;
            z = t ^ x ^ y;

            return z;
        }

    private:
        static inline uint64_t rotl( const uint64_t x, int k ) 
        {
            return ( x << k ) | ( x >> ( 64 - k ) );
        }

        static uint64_t next( void )
        {
            static uint64_t s[2] = { 0x41, 0x29837592 };

            const uint64_t s0 = s[0];
            uint64_t s1 = s[1];
            const uint64_t result = s0 + s1;

            s1 ^= s0;
            s[0] = rotl( s0, 55 ) ^ s1 ^ ( s1 << 14 ); // a, b
            s[1] = rotl( s1, 36 ); // c

            return result;
        }
    };

    inline int FastMod( const int input, const int ceil ) {
        // apply the modulo operator only when needed
        // (i.e. when the input is greater than the ceiling)
        return input >= ceil ? input % ceil : input;
        // NB: the assumption here is that the numbers are positive
    }
}