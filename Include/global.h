#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "defines.h"
#include "timedate.h"

typedef struct
{
  uint8_t timeTick:1;
  uint8_t curentError;
  uint8_t debugMode;
} TDeviceStatus;

extern volatile TDeviceStatus globalDeviceStatus;

//device type
extern uint8_t globalDeviceType;

//check if OS is started
extern uint8_t globalOSStarted;

//what to display
extern uint8_t globalDisplayMode;

extern TTimeDate globalTimeDate;

#endif
