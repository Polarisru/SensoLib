#ifndef _BMP180_H
#define _BMP180_H

#include "defines.h"
#include "Platform.h"

#define BMP180_I2C_ADDR  (0xEE)

/* BMP180 Pressure calculation constants */
#define BMP180_PARAM_MG                 3038
#define BMP180_PARAM_MH                -7357
#define BMP180_PARAM_MI                 3791

#define BMP180_EEPROM_DATA_LEN  22   // E2PROM length

/* Registers */
#define	BMP180_REG_CONTROL 	    0xF4
#define	BMP180_REG_RESULT 		  0xF6
#define BMP180_REG_EEPROM		    0xAA
#define BMP180_REG_RESET        0xE0 // Soft reset control
#define BMP180_REG_CHIPID       0xD0 // Chip ID
#define BMP180_REG_VERSION      0xD1 // Version

/* Commands */
#define	BMP180_CMD_TEMPERATURE 	0x2E
#define	BMP180_CMD_PRESSURE_0 	0x34
#define	BMP180_CMD_PRESSURE_1 	0x74
#define	BMP180_CMD_PRESSURE_2 	0xB4
#define	BMP180_CMD_PRESSURE_3 	0xF4

/* Multiple is faster than divide */
#define BMP180_1_16     ((float) 0.0625)
#define BMP180_1_256    ((float) 0.00390625)
#define BMP180_1_2048   ((float) 0.00048828125)
#define BMP180_1_4096   ((float) 0.000244140625)
#define BMP180_1_8192   ((float) 0.0001220703125)
#define BMP180_1_32768  ((float) 0.000030517578125)
#define BMP180_1_65536  ((float) 0.0000152587890625)
#define BMP180_1_101325 ((float) 0.00000986923266726)

#define BMP180_DELAY_TEMPERATURE	5
#define BMP180_DELAY_PRESSURE_0		5
#define BMP180_DELAY_PRESSURE_1		8
#define BMP180_DELAY_PRESSURE_2		13
#define BMP180_DELAY_PRESSURE_3		25

#define OSS 3

/* Calibration parameters structure */
typedef struct {
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
	int32_t B5;
} _BMP180_CALIBR;

typedef struct {
  uint8_t   Port;                 // I2C port number
	float 		Temperature;					// Sensor Temperature
  int       Pressure;
	uint8_t			ID[4];								// Sensor HW ID
	uint8_t			Error;								// if Sensor read or write fails
} _BMP180_SENSOR __attribute__ ((aligned (4)));

extern void BMP180_Init(uint8_t Port);
extern void BMP180_Read(void);

#endif
