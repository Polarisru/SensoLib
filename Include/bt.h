#ifndef _BT_H
#define _BT_H

#include "defines.h"

#define BT_PIN_EN     GPIO_Pin_2
#define BT_EN_GPIO    GPIOA

//extern void BT_Task(void);
extern void BT_Task(void *pvParameters);
extern void BT_Init(int Port);
extern void BT_SendCmd(char *cmd);
extern void BT_Switch(int On);

#endif
