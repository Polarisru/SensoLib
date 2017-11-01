#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdint.h>

#ifndef false
  #define false 0x00
#endif
#ifndef true
  #define true  0xff
#endif

//==================================================================
// Common types
//==================================================================

typedef unsigned char TBool;

#ifndef HIBYTE
  #define HIBYTE(a) (*((uint8_t*)&a + 1))
#endif

#ifndef LOBYTE
  #define LOBYTE(a) (*((uint8_t*)&(a)))
#endif

#endif
