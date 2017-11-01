#ifndef _SPI_H
#define _SPI_H

#include "defines.h"

#define SPI_DUMMY_BYTE    (0xFF)

enum SPI_NUM
{
  SPI_NUM_1,
  SPI_NUM_2,
  SPI_NUM_LAST
};

enum SPI_MODE
{
  SPI_MODE_0,
  SPI_MODE_1,
  SPI_MODE_2,
  SPI_MODE_3,
  SPI_MODE_LAST
};

extern void SPI_Initialize(uint8_t Num, uint8_t Mode, uint16_t Prescaler);
extern void SPI_ReadData(uint8_t Num, uint8_t* Data, uint16_t Len);
extern void SPI_WriteData(uint8_t Num, uint8_t* Data, uint16_t Len);

#endif
