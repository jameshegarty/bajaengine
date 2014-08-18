#include "random.h"
#include <time.h>
#include "endian.hpp"

void TRandomMersenne::RandomInit(long int seed) {
  // re-seed generator
  unsigned long s = (unsigned long)seed;
  for (mti=0; mti<N; mti++) {
    s = s * 29943829 - 1;
    mt[mti] = s;}}


unsigned long TRandomMersenne::BRandom() {
  // generate 32 random bits
  unsigned long y;

  if (mti >= N) {
    // generate N words at one time
    const unsigned long LOWER_MASK = (1u << R) - 1; // lower R bits
    const unsigned long UPPER_MASK = -1 << R;       // upper 32-R bits
    int kk, km;
    for (kk=0, km=M; kk < N-1; kk++) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
      mt[kk] = mt[km] ^ (y >> 1) ^ (-(signed long)(y & 1) & MATRIX_A);
      if (++km >= N) km = 0;}

    y = (mt[N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ (-(signed long)(y & 1) & MATRIX_A);
    mti = 0;}

  y = mt[mti++];

  // Tempering (May be omitted):
  y ^=  y >> TEMU;
  y ^= (y << TEMS) & TEMB;
  y ^= (y << TEMT) & TEMC;
  y ^=  y >> TEML;

  return y;}

  
double TRandomMersenne::Random() {
  // output random float number in the interval 0 <= x < 1

  // get 32 random bits and convert to float
  unsigned long r = BRandom();

  //slower method that works on all architectures
  return (double)r * (1./((double)(unsigned int)(-1L)+1.));
  
}

  
long TRandomMersenne::IRandom(long min, long max) {
  // output random integer in the interval min <= x <= max
  long r;
  r = long((max - min + 1) * Random()) + min; // multiply interval with random and truncate
  if (r > max) r = max;
  if (max < min) return 0x80000000;
  return r;
}


TRandomMersenne randomGen(time(0));
