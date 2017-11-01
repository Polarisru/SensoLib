#ifndef _RTC_H
#define _RTC_H

#include "defines.h"
#include "timedate.h"

extern void RTC_SetDateTime(TTimeDate *dt);
extern void RTC_GetDateTime(TTimeDate *dt);
extern void RTC_Init(void);

#endif
