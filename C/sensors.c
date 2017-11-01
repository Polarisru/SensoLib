#include <string.h>
#include "const.h"
#include "global.h"
#include "rtos.h"
#include "sensors.h"
#include "utils.h"

//------------------------------------------------------------------------------
// SENSORS_Init - init sensors module
//------------------------------------------------------------------------------
void SENSORS_Init(void)
{

}

//------------------------------------------------------------------------------
// INPUTS_Task - inputs RTOS task
//------------------------------------------------------------------------------
//__task void SENSORS_Task(void)
void SENSORS_Task(void const *arg)
{
  
	for (;;) 
	{
		//os_dly_wait(50);                 // wait interval
    //osDelay(50);
    vTaskDelay(50);
  }
}
