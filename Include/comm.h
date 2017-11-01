#ifndef _COMM_H
#define _COMM_H

#include "defines.h"
#include "platform.h"

extern void COMM_Init(void);
//extern void COMM_Task(void);
extern void COMM_Task(void *pvParameters);

#endif
