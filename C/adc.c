#include "adc.h"
#include "global.h"
#include "rtos.h"
#include "stm32f10x_adc.h"
#include "utils.h"

__IO uint16_t ADC_Buffer[ADC_COUNTS_NUM*ADC_CHANNELS_NUMBER];
uint16_t ADC_Values[ADC_CHANNELS_NUMBER];
//int TIM_Counter = 0;
//int DMA_Counter = 0;

//------------------------------------------------------------------------------
// ADC_DMA_Init - initialize ADC DMA
//------------------------------------------------------------------------------
void ADC_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

  // DMA1 channel1 configuration
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_COUNTS_NUM*ADC_CHANNELS_NUMBER;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
  // Enable DMA1 channel1
  DMA_Cmd(DMA1_Channel1, ENABLE);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC1)==SET)
	{	
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		DMA_ClearFlag(DMA1_FLAG_TC1);
    //DMA_Counter++;
		//isr_evt_set(0x0001, t_adc);
    //osSignalSet(t_adc, 0x0001);
    if (globalOSStarted)
      xSemaphoreGiveFromISR(semADC, NULL);
	}
}

//------------------------------------------------------------------------------
// TIM3_IRQHandler - timer interrupt, start new ADC conversion
//------------------------------------------------------------------------------
/*void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    //TIM_Counter++;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  }
}*/

//------------------------------------------------------------------------------
// ADC_TIM_Init - initialize timer to start ADC conversion periodically
//------------------------------------------------------------------------------
void ADC_TIM_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // Enable the TIM2 global Interrupt
  /*NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / ADC_TIMER_FREQ) - 1;
  TIM_TimeBaseStructure.TIM_Period = (uint16_t)(ADC_TIMER_FREQ/ADC_TIMER_INT_RATE);
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);*/
  
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 2 / ADC_TIMER_FREQ) - 1;  // APB1 = SystemCoreClock/2
  TIM_TimeBaseStructure.TIM_Period = (uint16_t)(ADC_TIMER_FREQ/ADC_TIMER_INT_RATE) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T2_TRGO
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

//------------------------------------------------------------------------------
// ADC_Init - initialize ADC module
//------------------------------------------------------------------------------
void ADC_Initialize(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_DeInit(ADC1);

	GPIO_StructInit(&GPIO_InitStructure);
  // Configure pins as analog inputs (AIN0-AIN1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure pins as analog inputs (AIN8-AIN9)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	// ADC1 configuration
	ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//ENABLE;
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNELS_NUMBER;
  ADC_Init(ADC1, &ADC_InitStructure);
  
	// ADC1 regular channels configuration
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);   // 239.5 cycles + 12.5 cycles = 252 cycles   
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);    
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_239Cycles5);    
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_239Cycles5);    
  
	// Enable ADC1 DMA
  ADC_DMACmd(ADC1, ENABLE);

  // Enable ADC1
  ADC_Cmd(ADC1, ENABLE);
	
  // Enable ADC1 reset calibration register
  ADC_ResetCalibration(ADC1);
  // Check the end of ADC1 reset calibration register
  while (ADC_GetResetCalibrationStatus(ADC1));
  // Start ADC1 calibration
  ADC_StartCalibration(ADC1);
  // Check the end of ADC1 calibration
  while (ADC_GetCalibrationStatus(ADC1));

  // Start ADC1 Software Conversion
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  
  ADC_DMA_Init();
  
  ADC_TIM_Init();
	
	//ADC_Start();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

//------------------------------------------------------------------------------
// ADC_Task - ADC RTOS task
//------------------------------------------------------------------------------
//__task void ADC_Task(void)
//void ADC_Task(void const *arg)
void ADC_Task(void *pvParameters)
{
  int i, j;
  uint16_t TempValue;
  uint32_t Summ;
  
  //vTaskSetApplicationTaskTag(NULL, (void *)1);
	
	for (;;) 
	{
		//ADC_Start();
		//os_dly_wait(2);    // wait interval
    //osDelay(2);
		//os_evt_wait_or(0x0001, 0xffff);	// wait for conversion complete
    //osSignalWait(0x0001, osWaitForever);
    xSemaphoreTake(semADC, portMAX_DELAY);
    
		//collect values from buffer
    for (i=0; i<ADC_CHANNELS_NUMBER; i++)
    {
      Summ = 0;
      for (j=0; j<ADC_COUNTS_NUM; j++)
        Summ += ADC_Buffer[i + j*ADC_CHANNELS_NUMBER];
      TempValue = Summ/ADC_COUNTS_NUM;
      ADC_Values[i] = TempValue;
		}
 	}
}

//------------------------------------------------------------------------------
// ADC_GetValue - get current ADC value for channel
//------------------------------------------------------------------------------
uint8_t ADC_GetValue(int Num)
{  
  if (Num>=ADC_CHANNELS_NUMBER)
    return 0; 
    
  return ADC_Values[Num];
}
