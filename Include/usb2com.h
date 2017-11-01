#ifndef _USB2COM_H
#define _USB2COM_H

#include "defines.h"

extern void USB2COM_Init(void);
extern void USB2COM_Send(char *Buff);
//extern void USB2COM_Task(void);
extern void USB2COM_Task(void *pvParameters);

#endif
