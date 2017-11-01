#ifndef _HMC5883L_H
#define _HMC5883L_H

#include "defines.h"
#include "platform.h"

#define HMC5883L_I2C_ADDR           0x3C
#define HMC5883L_I2C_SPEED	  	    (100000)

#define HMC5883L_RA_CONFIG_A        0x00
#define HMC5883L_RA_CONFIG_B        0x01
#define HMC5883L_RA_MODE            0x02
#define HMC5883L_RA_DATAX_H         0x03
#define HMC5883L_RA_DATAX_L         0x04
#define HMC5883L_RA_DATAZ_H         0x05
#define HMC5883L_RA_DATAZ_L         0x06
#define HMC5883L_RA_DATAY_H         0x07
#define HMC5883L_RA_DATAY_L         0x08
#define HMC5883L_RA_STATUS          0x09
#define HMC5883L_RA_ID_A            0x0A
#define HMC5883L_RA_ID_B            0x0B
#define HMC5883L_RA_ID_C            0x0C

#define HMC5883L_RATE_0P75          0x00
#define HMC5883L_RATE_1P5           0x01
#define HMC5883L_RATE_3             0x02
#define HMC5883L_RATE_7P5           0x03
#define HMC5883L_RATE_15            0x04
#define HMC5883L_RATE_30            0x05
#define HMC5883L_RATE_75            0x06

#define HMC5883L_CRB_RATE_OFFS      2
#define HMC5883L_CRB_RATE_LENGTH    3

#define HMC5883L_BIAS_NORMAL        0x00
#define HMC5883L_BIAS_POSITIVE      0x01
#define HMC5883L_BIAS_NEGATIVE      0x02

#define HMC5883L_CRB_BIAS_OFFS      0
#define HMC5883L_CRB_BIAS_LENGTH    2

#define HMC5883L_AVERAGING_1        0x00
#define HMC5883L_AVERAGING_2        0x01
#define HMC5883L_AVERAGING_4        0x02
#define HMC5883L_AVERAGING_8        0x03

#define HMC5883L_CRB_AVERAGE_OFFS   5
#define HMC5883L_CRB_AVERAGE_LENGTH 2

#define HMC5883L_GAIN_1370          0x00
#define HMC5883L_GAIN_1090          0x01
#define HMC5883L_GAIN_820           0x02
#define HMC5883L_GAIN_660           0x03
#define HMC5883L_GAIN_440           0x04
#define HMC5883L_GAIN_390           0x05
#define HMC5883L_GAIN_330           0x06
#define HMC5883L_GAIN_220           0x07

#define HMC5883L_CRB_GAIN_OFFS      5
#define HMC5883L_CRB_GAIN_LENGTH    3

#define HMC5883L_MODE_CONTINUOUS    0x00
#define HMC5883L_MODE_SINGLE        0x01
#define HMC5883L_MODE_IDLE          0x02

#define HMC5883L_STATUS_LOCK_BIT    1
#define HMC5883L_STATUS_READY_BIT   0

#define HMC5883L_ID_SIGN_0          'H'
#define HMC5883L_ID_SIGN_1          '4'
#define HMC5883L_ID_SIGN_2          '3'

#define PI    3.14159f

typedef struct
{
  uint8_t Port;
  int16_t X;
  int16_t Y;
  int16_t Z;
  float ScaledX;
  float ScaledY;
  float ScaledZ;
  float Heading;
  uint8_t			Error;								// if Sensor read or write fails
} _HMC5883L_SENSOR __attribute__ ((aligned (4)));

extern void HMC5883L_Init(uint8_t Port);
extern void HMC5883L_Read(void);

#endif
