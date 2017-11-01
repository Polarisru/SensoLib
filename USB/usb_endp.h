/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_ENDP_H
#define __USB_ENDP_H

#include "defines.h"

#define USB_TX_DATA_SIZE   2048

extern uint8_t USB_Tx_Buffer[USB_TX_DATA_SIZE];
extern uint32_t USB_Tx_ptr_in;
extern uint32_t USB_Tx_ptr_out;
extern uint32_t USB_Tx_length;

#endif
