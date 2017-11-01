#include <ctype.h>
#include <string.h>
#include "const.h"
#include "global.h"
#include "gps.h"
#include "nmea.h"
#include "rs232.h"
#include "rtos.h"
#include "usb2com.h"
#include "utils.h"

const char parserCmdStart[]="$GP";

const char parserCmdRMC[]="RMC";
const char parserInRMC[]="";

const char parserCmdGGA[]="GGA";
const char parserInGGA[]="";

const char parserCmdGLL[]="GLL";
const char parserInGLL[]="";

const char parserCmdNull[]="";

const TParsedItem GPS_Items[GPS_NUM_COMMANDS] =
{
  {parserCmdRMC,        parserCmdRMC},
  {parserCmdGGA,        parserInGGA},
  {parserCmdGLL,        parserInGLL},
  {parserCmdNull,       NULL}
  
};

int GPS_Port;

//------------------------------------------------------------------------------
// GPS_Callback - callback function for RS232 driver
//------------------------------------------------------------------------------
void GPS_Callback(void)
{
  if (globalOSStarted)
    //isr_evt_set(0x0001, t_gps);
    //osSignalSet(t_gps, 0x0001);
    xSemaphoreGiveFromISR(semGPS, NULL);
}

//------------------------------------------------------------------------------
// GPS_Init - initialize GPS module
//------------------------------------------------------------------------------
void GPS_Init(int Port)
{
  RS232_InitStructDef RS232_InitStructure;
  
  if (Port>=USART_LAST)
    return;
  
  RS232_InitStructure.Baudrate = 9600;
  RS232_InitStructure.Mode = RS232_MODE_RECEIVE;
  RS232_InitStructure.EndSign = CHAR_CR;
  RS232_InitStructure.EscSign = CHAR_START;
  RS232_InitStructure.Error = 0;  
  RS232_Init(Port, &RS232_InitStructure, GPS_Callback);
  
  GPS_Port = Port;
}

//------------------------------------------------------------------------------
// GPS_GetIndex  - search for an index in the string array
//------------------------------------------------------------------------------
signed char GPS_GetIndex(TParsedItem const *items, char *buffer)
{
  int i;

  UTILS_StrUpr(buffer);
  i = 0;
  for (i=0; i<GPS_NUM_COMMANDS; i++)
    if (0==strncmp(buffer, items[i].itemStr, strlen(items[i].itemStr)))
      return i;
  return -1;
}

//------------------------------------------------------------------------------
// GPS_Task
//------------------------------------------------------------------------------
//__task void GPS_Task(void)
//void GPS_Task(void const *arg)
void GPS_Task(void *pvParameters)
{
  char *cmd;
  int pos;
  //char buff[128];
  signed char index;
  int cmd_length;
  uint8_t crc;
  char crcStr[2];
  TDataPoint Point;

  for (;;)
  {
	  //os_mbx_wait(ParsBox, (void**)&msg, 0xffff); - don't use
	  //os_evt_wait_or(0x0001, 0xffff);	// wait for reply command
    //osSignalWait(0x0001, osWaitForever);
    xSemaphoreTake(semGPS, portMAX_DELAY);
    
		cmd = RS232_Buffer[GPS_Port];
    
		pos = 0;

    while ((CHAR_CR!=cmd[pos])&&(CHAR_NL!=cmd[pos])&&cmd[pos])
      pos++;
    
    if (pos>=GPS_BUF_LEN)
      pos = GPS_BUF_LEN-1;
    cmd[pos] = '\0';
    
    if (globalDeviceStatus.debugMode==DEBUG_MODE_GPS)
      USB2COM_Send(cmd);

    //find command - skip blanks
    while (*cmd&&isspace(*cmd))
      cmd++;
    pos = 0;
    while (cmd[pos])//&&(cmd[pos]!=0x20))
      pos++;
    cmd[pos] = '\0';
    
    if (!NMEA_CalcCRC(&crc, (uint8_t*)cmd, pos))
      continue;
    
    UTILS_DecToHex(crc, crcStr);
    if (strncmp(&cmd[pos-2], crcStr, 2)!=0)
      continue;
    
    if (strncmp(cmd, parserCmdStart, strlen(parserCmdStart))!=0)
      continue;

    cmd += strlen(parserCmdStart);
    //check command and get its index
    index = GPS_GetIndex(GPS_Items, cmd);
    if (index>=0)
    {  
      cmd_length=strlen(GPS_Items[index].itemStr);
      if (strlen(cmd)>cmd_length)
      {
        cmd = &cmd[cmd_length];
      } else cmd = &cmd[pos];
    }
    //errMsg = NULL;

    while (*cmd==0x20)
      cmd++;
    
    if ((index==-1)||(index>=GPS_CMD_NULL))
    {
      //errMsg = parserErrWrongCommand;
    } else
    {  
      switch (index)
      {
        case GPS_CMD_RMC:
          NMEA_ParseRMC(cmd, &Point);
          break;
        case GPS_CMD_GGA:
          NMEA_ParseGGA(cmd, &Point);
          break;
        case GPS_CMD_GLL:
          NMEA_ParseGLL(cmd, &Point);
          break;
        case GPS_CMD_NULL:
        default:
          break;
      }
    }
  }
}
