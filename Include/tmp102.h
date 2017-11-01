#ifndef _TMP102_H
#define _TMP102_H

#include "defines.h"
#include "platform.h"

#define TMP102_I2C_ADDRESS			  (0x90)

#define TMP102_CMD_READ           (0x00)

#define TMP102_COUNTER            (4)

#define TMP102_ERRORS_NUMBER      (10)

typedef struct {
  int       Port;                 // I2C port number
	float 		Temperature;					// Sensor Temperature
	uint8_t			ID[4];								// Sensor HW ID
	uint8_t			Error;								// if Sensor read or write fails
} _TMP102_SENSOR __attribute__ ((aligned (4))); 

extern void TMP102_Task(void);
extern int TMP102_GetTemp(float *Temp);
extern void TMP102_Init(int Port);
  
#endif
