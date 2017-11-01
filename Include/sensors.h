#ifndef _SENSORS_H
#define _SENSORS_H

#include "platform.h"
#include "defines.h"

#define SENSORS_NO_VALUE    (0xFF)
#define SENSORS_NO_LIMIT    (0xF0)

extern void SENSORS_Init(void);
extern int SENSORS_GetValues(int Basket, uint8_t *Values);
extern void SENSORS_Task(void const *arg);

#endif
