/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_endp.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "global.h"
#include "rtos.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Tx_Buffer[USB_TX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USB_Tx_length  = 0;

extern char USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern uint8_t USB_Data_Received_Count;
extern uint8_t  USB_Tx_State;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

// check if something to send, copy it to buffer
void Handle_USBAsynchXfer(void)
{
  uint16_t Tx_length;

  /*if (USB_Tx_State != 1)
  {
    if (USB_Tx_ptr_out == USB_TX_DATA_SIZE)
    {
      USB_Tx_ptr_out = 0;
    }
    
    if (USB_Tx_ptr_out == USB_Tx_ptr_in) 
    {
      USB_Tx_State = 0; 
      return;
    }
    
    if(USB_Tx_ptr_out > USB_Tx_ptr_in) // rollback
    { 
      USB_Tx_length = USB_TX_DATA_SIZE - USB_Tx_ptr_out;
    }
    else 
    {
      USB_Tx_length = USB_Tx_ptr_in - USB_Tx_ptr_out;
    }
    
    if (USB_Tx_length > VIRTUAL_COM_PORT_DATA_SIZE)
    {
      USB_Tx_ptr = USB_Tx_ptr_out;
      USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;
      
      USB_Tx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;	
      USB_Tx_length -= VIRTUAL_COM_PORT_DATA_SIZE;	
    }
    else
    {
      USB_Tx_ptr = USB_Tx_ptr_out;
      USB_Tx_length = USB_Tx_length;
      
      USB_Tx_ptr_out += USART_Tx_length;
      USB_Tx_length = 0;
    }
    USB_Tx_State = 1;
    UserToPMABufferCopy(&USB_Tx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, Tx_length);
    SetEPTxCount(ENDP1, Tx_length);
    SetEPTxValid(ENDP1); 
  }*/

  if (USB_Tx_State != 1)
  {
    if (USB_Tx_length==0)
    {
      USB_Tx_State = 0;
      return;
    }

    Tx_length = USB_Tx_length;
    USB_Tx_length = 0;
    USB_Tx_State = 0;
    UserToPMABufferCopy(&USB_Tx_Buffer[0], ENDP1_TXADDR, Tx_length);
    SetEPTxCount(ENDP1, Tx_length);
    SetEPTxValid(ENDP1); 
  }
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
	Handle_USBAsynchXfer();
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback - get data from USB
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  // Get the received data buffer and update the counter 
  USB_Data_Received_Count = USB_SIL_Read(EP3_OUT, (uint8_t*)USB_Rx_Buffer);

  //for (i=0; i<USB_Rx_Cnt; i++) {
	//  USB_SetLeds(USB_Rx_Buffer[i]);
  //}
  
  //isr_evt_set(0x0001, t_usb2com);
  //osSignalSet(t_usb2com, 0x0001);
  if (globalOSStarted)
    xSemaphoreGiveFromISR(semUSB2COM, NULL);
  
  // Enable the receive of data on EP3
  SetEPRxValid(ENDP3);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
  static uint32_t FrameCount = 0;
  
  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    {
      /* Reset the frame counter */
      FrameCount = 0;
      
      /* Check the data to be sent through IN pipe */

      Handle_USBAsynchXfer();

    }
  }  
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

