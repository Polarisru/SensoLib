#ifndef _ADC_H
#define _ADC_H

#include "const.h"
#include "platform.h"
#include "defines.h"

#define ADC1_DR_Address     ((uint32_t)(&ADC1->DR))

#define ADC_CHANNELS_NUMBER	(4)

#define ADC_COUNTS_NUM      (16)

#define ADC_MAX_LIMIT       (0xF00)
#define ADC_MIN_LIMIT       (0x800)

#define ADC_MAX_VALUE       (0xFF)
#define ADC_MIN_VALUE       (0x00)

#define ADC_TIMER_FREQ		  (10000L)
#define ADC_TIMER_INT_RATE	(5*33L)//(3*100L)

#define ADC_MAX_WINDOW      (60*ADC_TIMER_INT_RATE/MIN_SPEED_VALUE + 1)

extern void ADC_Initialize(void);
//extern void ADC_Task(void);
extern void ADC_Task(void *pvParameters);
extern uint8_t ADC_GetValue(int Num);

#endif
