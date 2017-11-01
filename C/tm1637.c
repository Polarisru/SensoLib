#include "tm1637.h"
#include "stm32f10x_gpio.h"

uint8_t TM1637_PointFlag;
uint8_t TM1637_DispCtrl;

const int8_t TM1637_Font[] = 
{
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,        //0
  SEG_B|SEG_C,                                //1
  SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,              //2
  SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,	            //3
  SEG_B|SEG_C|SEG_F|SEG_G,                    //4
  SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,              //5
  SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,        //6
  SEG_A|SEG_B|SEG_C,                          //7
  SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,  //8
  SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G,        //9
  SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G,        //A
  SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,              //b
  SEG_A|SEG_D|SEG_E|SEG_F,                    //C
  SEG_B|SEG_C|SEG_D|SEG_E|SEG_G,              //d
  SEG_A|SEG_D|SEG_E|SEG_F|SEG_G,              //E
  SEG_A|SEG_E|SEG_F|SEG_G                     //F
};

//------------------------------------------------------------------------------
// TM1637_Delay - display delay routine
//------------------------------------------------------------------------------
void TM1637_Delay(void)
{
  int i;
  
  for (i=0; i<10; i++)
    __nop();
}
//------------------------------------------------------------------------------
// TM1637_SetDataPin - set data pin mode (output or input)
//------------------------------------------------------------------------------
void TM1637_SetDataPin(uint8_t Out)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  TM1637_PIN_DIO;
  if (Out==TM1637_OUTPUT)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  else
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(TM1637_GPIO, &GPIO_InitStructure);
}

//------------------------------------------------------------------------------
// TM1637_WriteByte - write byte to display
//------------------------------------------------------------------------------
void TM1637_WriteByte(int8_t wr_data)
{
  uint8_t i, count1;
  
  for (i=0; i<8; i++)        //sent 8bit data
  {
    GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_RESET);
    TM1637_Delay();
    if (wr_data & 0x01)
      GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_SET);
    else 
      GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_RESET);
    wr_data >>= 1;      
    GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_SET);
    TM1637_Delay();
  }  
  //wait for ACK
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_RESET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_SET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_SET);
  TM1637_Delay();
  TM1637_SetDataPin(TM1637_INPUT);
  while (GPIO_ReadInputDataBit(TM1637_GPIO, TM1637_PIN_DIO)==Bit_SET)    
  { 
    count1++;
    if (count1>=200)
    {
      TM1637_SetDataPin(TM1637_OUTPUT);
      GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_RESET);
      count1 = 0;
    }
    TM1637_SetDataPin(TM1637_INPUT);
  }
  TM1637_SetDataPin(TM1637_OUTPUT);
}

//------------------------------------------------------------------------------
// TM1637_Start - send start signal to TM1637
//------------------------------------------------------------------------------
void TM1637_Start(void)
{
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_SET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_SET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_RESET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_RESET);
  TM1637_Delay();
} 

//------------------------------------------------------------------------------
// TM1637_Stop - end of transmission
//------------------------------------------------------------------------------
void TM1637_Stop(void)
{
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_RESET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_RESET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_CLK, Bit_SET);
  TM1637_Delay();
  GPIO_WriteBit(TM1637_GPIO, TM1637_PIN_DIO, Bit_SET);
  TM1637_Delay();
}

//------------------------------------------------------------------------------
// TM1637_Coding - decode display data
//------------------------------------------------------------------------------
int8_t TM1637_Coding(int8_t DispData)
{
  uint8_t PointData;
  
  if (TM1637_PointFlag == TM1637_POINT_ON)
    PointData = 0x80;
  else 
    PointData = 0; 
  if (DispData == TM1637_DISP_EMPTY) 
    DispData = 0x00 + PointData;//The bit digital tube off
  else 
    DispData = TM1637_Font[DispData] + PointData;
  
  return DispData;
}

//------------------------------------------------------------------------------
// TM1637_Display - display data to segment
//------------------------------------------------------------------------------
void TM1637_Display(uint8_t BitAddr, int8_t DispData)
{
  int8_t SegData;
  
  SegData = TM1637_Coding(DispData);
  TM1637_Start();          //start signal sent to TM1637 from MCU
  TM1637_WriteByte(TM1637_ADDR_FIXED);
  TM1637_Stop();
  TM1637_Start();
  TM1637_WriteByte(BitAddr|TM1637_STARTADDR);
  TM1637_WriteByte(SegData);
  TM1637_Stop();
  TM1637_Start();
  TM1637_WriteByte(TM1637_DispCtrl);
  TM1637_Stop();
}

//------------------------------------------------------------------------------
// TM1637_Display - display data to segment
//------------------------------------------------------------------------------
void TM1637_ClearDisplay(void)
{
  TM1637_Display(0, TM1637_DISP_EMPTY);
  TM1637_Display(1, TM1637_DISP_EMPTY);
  TM1637_Display(2, TM1637_DISP_EMPTY);
  TM1637_Display(3, TM1637_DISP_EMPTY);  
}

//------------------------------------------------------------------------------
// TM1637_SetCmd - set cmd to take effect the next time it displays
//------------------------------------------------------------------------------
void TM1637_SetCmd(uint8_t brightness)
{
  //Cmd_SetData = SetData;
  //Cmd_SetAddr = SetAddr;
  TM1637_DispCtrl = TM1637_ADDR_BRIGHT + brightness;//Set the brightness and it takes effect the next time it displays.
}

//------------------------------------------------------------------------------
// TM1637_SetPoint - set clock point ":", takes effect the next time it displays
//------------------------------------------------------------------------------
void TM1637_SetPoint(uint8_t PointFlag)
{
  TM1637_PointFlag = PointFlag;
}

//------------------------------------------------------------------------------
// TM1637_Init - initialize display hardware
//------------------------------------------------------------------------------
void TM1637_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
	GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  TM1637_PIN_CLK | TM1637_PIN_DIO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(TM1637_GPIO, &GPIO_InitStructure);
  
  TM1637_PointFlag = TM1637_POINT_ON;
  TM1637_SetCmd(TM1637_BRIGHT_TYPICAL);
  
  TM1637_ClearDisplay();
}
