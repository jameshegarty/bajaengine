#ifndef RANDOM_H
#define RANDOM_H

class TRandomMersenne {                // encapsulate random number generator
#if 1
    // define constants for MT11213A
  enum constants {   
    N = 351, M = 175, R = 19, 
    TEMU = 11, TEMS = 7, TEMT = 15, TEML = 17};
  // long constants cannot be defined as enum in 16-bit compilers:
  #define MATRIX_A 0xE4BD75F5
  #define TEMB     0x655E5280
  #define TEMC     0xFFD58000
#else    
  enum constants {   
    // or constants for MT19937
    N = 624, M = 397, R = 31, 
    TEMU = 11, TEMS = 7, TEMT = 15, TEML = 18};
  #define MATRIX_A 0x9908B0DF
  #define TEMB     0x9D2C5680
  #define TEMC     0xEFC60000
#endif    
  public:
  TRandomMersenne(long int seed) {     // constructor
    RandomInit(seed);}
  void RandomInit(long int seed);      // re-seed
  long IRandom(long min, long max);    // output random integer
  double Random();                     // output random float
  unsigned long BRandom();             // output random bits
  private:
  unsigned long mt[N];                 // state vector
  int mti;                             // index into mt
  };    
  

extern TRandomMersenne randomGen;

#endif
