/* Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#ifndef __FLUC_MERSENNETWISTER_HH__
#define __FLUC_MERSENNETWISTER_HH__

#include <inttypes.h>
#include <cstring>
#include <ctime>
#include <climits>


namespace iNA {

/** This is a 64-bit version of Mersenne Twister pseudo random number
 * generator.
 *
 * Before using, initialize the state by calling @c seed.
 * @ingroup math */
class MersenneTwister
{
private:
  /** Period constant. */
  static const size_t NN=312;
  /** Period constant. */
  static const size_t MM=156;

  /** Magic constant. */
  static const uint64_t MATRIX_A=0xB5026F5AA96619E9ULL;
  /** Most significant 33 bits */
  static const uint64_t UM=0xFFFFFFFF80000000ULL;
  /** Least significant 31 bits */
  static const uint64_t LM=0x7FFFFFFFULL;

  /** The array for the state vector */
  uint64_t mt[NN];
  /** mti==NN+1 means mt[NN] is not initialized */
  size_t mti;


public:
  /** Constructor with seed. */
  MersenneTwister(uint64_t seed)
    : mti(NN+1)
  {
    this->seed(seed);
  }

  /**
   * Constructor w/o seed, needs explicit call to @c seed.
   */
  MersenneTwister()
    : mti(NN+1)
  {
    // Pass...
  }

  /**
   * Copy-constructor. Also copies the internal state of the RNG.
   * The copy will then generate the same random numbers as the original.
   */
  MersenneTwister(const MersenneTwister &other)
    : mti(other.mti)
  {
    memcpy(this->mt, other.mt, sizeof(uint64_t)*NN);
  }

  /** Assignment operator, copies the state of the RHS. */
  const MersenneTwister &
  operator =(const MersenneTwister &other)
  {
    this->mti = other.mti;
    memcpy(this->mt, other.mt, sizeof(uint64_t)*NN);

    return *this;
  }

  /** Initializes the RNG with given seed. */
  void seed(uint64_t seed)
  {
    mt[0] = seed;
    for (mti=1; mti<NN; mti++) {
      mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
    }
  }

  /** Initializes the RNG with vector seed. */
  void seed(uint64_t init_key[], size_t key_length)
  {
    seed(19650218ULL);
    size_t i=1, j=0;
    size_t k = (NN>key_length ? NN : key_length);

    for (; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 3935559000370003845ULL))
          + init_key[j] + j; /* non linear */
      i++; j++;
      if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
      if (j>=key_length) j=0;
    }

    for (k=NN-1; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 2862933555777941757ULL))
          - i; /* non linear */
      i++;
      if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
    }

    mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */
  }

  /** Returns an unsigned 64bit integer random number. */
  inline uint64_t rand_int()
  {
    size_t i;
    uint64_t x;
    static const int64_t mag01[2] = {0ULL, static_cast<int64_t>(MATRIX_A)};

    if (mti >= NN) { /* generate NN words at one time */
      /* if init_genrand64() has not been called, */
      /* a default initial seed is used     */
      if (mti == NN+1)
        seed(5489ULL);

      for (i=0;i<NN-MM;i++) {
        x = (mt[i]&UM)|(mt[i+1]&LM);
        mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
      }
      for (;i<NN-1;i++) {
        x = (mt[i]&UM)|(mt[i+1]&LM);
        mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
      }
      x = (mt[NN-1]&UM)|(mt[0]&LM);
      mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

      mti = 0;
    }

    x = mt[mti++];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);

    return x;
  }

  /** Generates a random number within (0,1] interval. */
  inline double rand()
  {
    return 1.-((rand_int() >> 11) * (1.0/9007199254740992.0));
  }

  /** Generates a random number within [0,1] interval. */
  inline double rand_incl()
  {
    return (rand_int() >> 11) * (1.0/9007199254740991.0);
  }

  /** Generates a random number within (0,1) interval. **/
  inline double rand_excl()
  {
    return ((rand_int() >> 12) + 0.5) * (1.0/4503599627370496.0);
  }
};

}

#endif // MERSENNETWISTER_HH



