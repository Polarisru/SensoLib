#ifndef _I2C_H
#define _I2C_H

#include "platform.h"
#include "defines.h"

enum I2C_NUM
{
  I2C_NUM_1,
  I2C_NUM_2,
  I2C_NUM_LAST
};

enum I2C_ERR
{
	I2C_ERR_OK,
	I2C_ERR_PORT,
	I2C_ERR_START,
	I2C_ERR_SENDADDR,
	I2C_ERR_SEND,
	I2C_ERR_RECEIVE,
  I2C_ERR_BUSY,
	I2C_ERR_LAST
};

extern void I2C_Initialize(int Num, int Address, int Speed);
extern int I2C_WriteData(int Num, int Address, char *pBuffer, int Len);
extern int I2C_ReadData(int Num, int Address, int Start, char *pBuffer, int Len);

#endif
