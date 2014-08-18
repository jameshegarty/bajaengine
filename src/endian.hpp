#ifdef _WIN32
inline short endian( short s ){return s;}
inline unsigned short endian( unsigned short s ){return s;}
inline int endian( int s ){return s;}
inline float endian( float s ){return s;}
inline unsigned int endian( unsigned int s ){return s;}
inline unsigned long endian( unsigned long s ){return s;}
inline unsigned char endian(unsigned char f){return f;}
inline double endian(double f){return f;}
inline bool endian(bool f){return f;}
#else

#ifdef __LITTLE_ENDIAN__
inline short endian( short s ){return s;}
inline unsigned short endian( unsigned short s ){return s;}
inline int endian( int s ){return s;}
inline float endian( float s ){return s;}
inline unsigned int endian( unsigned int s ){return s;}
inline unsigned long endian( unsigned long s ){return s;}
inline unsigned char endian(unsigned char f){return f;}
inline double endian(double f){return f;}
inline bool endian(bool f){return f;}
#else

inline bool endian(bool f){return f;}

inline short endian( short s ){
  unsigned char b1, b2;
  
  b1 = s & 255;
  b2 = (s >> 8) & 255;

  return (b1 << 8) + b2;
}

inline unsigned short endian(unsigned short s ){
  unsigned char b1, b2;
  
  b1 = s & 255;
  b2 = (s >> 8) & 255;

  return (b1 << 8) + b2;
}


inline int endian (int i){
  unsigned char b1, b2, b3, b4;

  b1 = i & 255;
  b2 = ( i >> 8 ) & 255;
  b3 = ( i>>16 ) & 255;
  b4 = ( i>>24 ) & 255;

  return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

inline unsigned int endian (unsigned int i){
  unsigned char b1, b2, b3, b4;

  b1 = i & 255;
  b2 = ( i >> 8 ) & 255;
  b3 = ( i>>16 ) & 255;
  b4 = ( i>>24 ) & 255;

  return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

inline unsigned long endian (unsigned long i){
  unsigned char b1, b2, b3, b4;

  b1 = i & 255;
  b2 = ( i >> 8 ) & 255;
  b3 = ( i>>16 ) & 255;
  b4 = ( i>>24 ) & 255;

  return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}


inline float endian( float f ){
  union
  {
    float f;
    unsigned char b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  return dat2.f;
}


inline double endian( double f ){
  union
  {
    double f;
    unsigned char b[8];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[7];
  dat2.b[1] = dat1.b[6];
  dat2.b[2] = dat1.b[5];
  dat2.b[3] = dat1.b[4];
  dat2.b[4] = dat1.b[3];
  dat2.b[5] = dat1.b[2];
  dat2.b[6] = dat1.b[1];
  dat2.b[7] = dat1.b[0];

  return dat2.f;
}


inline unsigned char endian(unsigned char f){ return f; }
#endif
#endif

