/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  Le Hung
  * @version V2.2.0
  * @date    31/3/2017
  * @brief   Main program body
   ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s_conf.h"
#include "delay.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      //f_Master = HSI/1 = 16MHz
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);            //f_CPU = f_Master/1 = 16MHz
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);     //wait until HSI ready
}


void SPI_Config(void)
{
  SPI_DeInit();
  SPI_Init(SPI_FIRSTBIT_MSB,
           SPI_BAUDRATEPRESCALER_8,
           SPI_MODE_SLAVE,        
           SPI_CLOCKPOLARITY_LOW,
           SPI_CLOCKPHASE_1EDGE,  
           SPI_DATADIRECTION_2LINES_FULLDUPLEX, 
           SPI_NSS_SOFT,                        
           0x01                                 
          );
  SPI_ITConfig(SPI_IT_RXNE, ENABLE);
  //SPI_ITConfig(SPI_IT_TXE,  ENABLE);
  SPI_Cmd(ENABLE);
}

void main(void)
{

  CLK_Config();
  SPI_Config();
  GPIO_DeInit(GPIOD); 
  GPIO_Init(GPIOD, GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
  enableInterrupts();
  /* Infinite loop */
  while (1)
  {
    GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
    delay_ms(1000);
  }
  
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



