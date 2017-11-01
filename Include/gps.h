#ifndef _GPS_H
#define _GPS_H

#include "defines.h"

#define GPS_BUF_LEN		(80)

enum GPS_COMMANDS
{
  GPS_CMD_RMC,
  GPS_CMD_GGA,
  GPS_CMD_GLL,

  GPS_CMD_NULL,
  GPS_NUM_COMMANDS
};

typedef struct
{
  //string
  const char *itemStr;
  //format
  const char *inFmt;
} TParsedItem;

extern void GPS_Init(int Port);
//extern void GPS_Task(void);
extern void GPS_Task(void *pvParameters);

#endif
