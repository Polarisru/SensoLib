#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "Defines.h"
#include "stm32f10x.h"

#define RAM_KEY_ADDR        (0x20002000)
#define BOOTLOADER_KEY1     (0x12345678)
#define BOOTLOADER_KEY2     (0x43211234)
#define BOOTLOADER_SIGN     (0x11223344)

#define MCLK 48000000                       // Master Clock 48 MHz
#define TCLK       10                       // Timer Clock rate 10/s
#define TCNT (MCLK/TCLK/4)                  // Timer Counts

#endif
