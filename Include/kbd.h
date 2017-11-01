#ifndef _KBD_H
#define _KBD_H

#include <stdint.h>
#include "platform.h"

enum KBD_STATE
{
  KBD_STATE_UNPRESSED,
  KBD_STATE_PRESSED
};

#define KBD_PIN_1       GPIO_Pin_4
#define KBD_GPIO        GPIOA

extern void KBD_Init(void);
//extern void KBD_Task(void);
extern void KBD_Task(void *pvParameters);

#endif
