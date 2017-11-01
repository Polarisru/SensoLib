#ifndef _TM1637_H
#define _TM1637_H

#include <stdint.h>
#include "defines.h"

#define SEG_A   0x01
#define SEG_B   0x02
#define SEG_C   0x04
#define SEG_D   0x08
#define SEG_E   0x10
#define SEG_F   0x20
#define SEG_G   0x40

//************definitions for TM1637*********************
#define TM1637_ADDR_AUTO    0x40
#define TM1637_ADDR_FIXED   0x44
#define TM1637_ADDR_BRIGHT  0x88
#define TM1637_STARTADDR    0xC0

/**** definitions for the clock point of the digit tube *******/
#define TM1637_POINT_ON   1
#define TM1637_POINT_OFF  0

#define TM1637_OUTPUT     1
#define TM1637_INPUT      0

/**************definitions for brightness***********************/
#define TM1637_BRIGHT_DARKEST 0
#define TM1637_BRIGHT_TYPICAL 2
#define TM1637_BRIGHTEST      7

#define TM1637_DISP_EMPTY     0x7F

#define TM1637_PIN_DIO    GPIO_Pin_2
#define TM1637_PIN_CLK    GPIO_Pin_3
#define TM1637_GPIO       GPIOA

extern void TM1637_Display(uint8_t BitAddr, int8_t DispData);
extern void TM1637_SetPoint(uint8_t PointFlag);
extern void TM1637_Init(void);

#endif
