#ifndef _LM75A_H
#define _LM75A_H

#include "defines.h"
#include "Platform.h"

#define LM75_I2C_ADDR   (0x90>>1)

#define LM75_REG_TEMP   (0x00)
#define LM75_REG_CONF   (0x01)
#define LM75_REG_THYST  (0x02)
#define LM75_REG_TOS    (0x03)

typedef struct {
  int       Port;                 // I2C port number
	float 		Temperature;					// Sensor Temperature
	uint8_t			ID[4];								// Sensor HW ID
	uint8_t			Error;								// if Sensor read or write fails
} _LM75A_SENSOR __attribute__ ((aligned (4)));

extern void LM75A_Init(int Port);
extern float LM75A_GetTemp(void);

#endif
