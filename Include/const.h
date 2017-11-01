#ifndef _CONST_H
#define _CONST_H

//max length of the serial number string
#define SERIAL_NUM_MAX_LEN	    (6)
#define MODE_OUT_LEN	          (16)
//max length of the PCB ID string
#define PCB_ID_MAX_LEN		      (10)

enum DFORMAT_MODE
{
  DFORMAT_MODE_EUR,
	DFORMAT_MODE_USA,
	DFORMAT_MODE_CHN
};

enum DISPLAY_MODE
{
  DISPLAY_MODE_TIME,
  DISPLAY_MODE_DATE,
  DISPLAY_MODE_YEAR,
  DISPLAY_MODE_LAST
};

enum DEBUG_MODE
{
  DEBUG_MODE_NONE,
  DEBUG_MODE_GPS,
  DEBUG_MODE_BT,
  DEBUG_MODE_LAST
};

//------------------------------------------------------------------------------
// Device
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Control
//------------------------------------------------------------------------------

#endif
