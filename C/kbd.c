#include "const.h"
#include "global.h"
#include "kbd.h"
#include "rtos.h"
#include "stm32f10x_gpio.h"

//------------------------------------------------------------------------------
// KBD_Init - initialize keyboard
//------------------------------------------------------------------------------
void KBD_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  KBD_PIN_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KBD_GPIO, &GPIO_InitStructure);
}

//------------------------------------------------------------------------------
// KBD_Task - keyboard aquisition RTOS task
//------------------------------------------------------------------------------
//__task void KBD_Task(void)
//void KBD_Task(void const *arg)
void KBD_Task(void *pvParameters)
{
  uint8_t State = KBD_STATE_UNPRESSED;
  
  for (;;)
  {
    //os_dly_wait(50);
    //osDelay(50);
    vTaskDelay(50);
    if ((GPIO_ReadInputDataBit(KBD_GPIO, KBD_PIN_1)==Bit_SET)&&(State==KBD_STATE_UNPRESSED))
    {
      State = KBD_STATE_PRESSED;
      globalDisplayMode++;
      if (globalDisplayMode>=DISPLAY_MODE_LAST)
        globalDisplayMode = DISPLAY_MODE_TIME;
      //os_evt_set(0x0001, t_comm);
      //osSignalSet(t_comm, 0x0001);
		  evtCOMM |= 0x0001;
			xSemaphoreGive(semCOMM);
    }
    if ((!GPIO_ReadInputDataBit(KBD_GPIO, KBD_PIN_1)==Bit_SET)&&(State==KBD_STATE_PRESSED))
    {
      State = KBD_STATE_UNPRESSED;
    }
  }
}
