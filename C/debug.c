#include <stdio.h>
#include "stm32f10x.h"
#include "core_cm3.h"

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

/*int fputc(int ch, FILE *f) {
  return (ITM_SendChar((unsigned int)ch));
}*/

/*
unsigned char backspace_called;
unsigned char last_char_read;
int r;

int fgetc(FILE *f)
{
    // if we just backspaced, then return the backspaced character
    // otherwise output the next character in the stream
    if (backspace_called == 1)
    {
      backspace_called = 0;
    }
    else {
        do {
            r = ITM_ReceiveChar();
        } while (r == -1);
        
        last_char_read = (unsigned char)r;

#ifdef ECHO_FGETC
        ITM_SendChar(r);
#endif
    }

    return last_char_read;
}*/
