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
#define CCR3_Val  ((uint16_t)5250/2)
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint16_t a;
uint16_t b;

static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      //f_Master = HSI/1 = 16MHz
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);            //f_CPU = f_Master/1 = 16MHz
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);     //wait until HSI ready
}

void ADC_Config(void)
{
  //Config GPIO for ADC1
  GPIO_Init(GPIOC, GPIO_PIN_4 , GPIO_MODE_IN_FL_NO_IT);   //PC4 AIN2 Channel 2
  GPIO_Init(GPIOD, GPIO_PIN_2 , GPIO_MODE_IN_FL_NO_IT);   //PD2 AIN3 Channel 3
  
 /* Init timer 1 */
  //TIM1_DeInit();                                        //deinit first configuration
  //TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 530, 0);    //set frequency of timer 1
  //TIM1_SelectOutputTrigger(TIM1_TRGOSOURCE_UPDATE);     //set TRGO active when UPDATE event

  /* Init ADC */
  ADC1_DeInit();                                          //deinit first configuration of ADC
//  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,             //single conversion mode
//            ADC1_CHANNEL_2,                             //in scan mode, this is the last channel to conversion
//            ADC1_PRESSEL_FCPU_D4,                       //set prescaler to 4 (16MHz / 4 = 4MHz [MAX 6MHz])
//            ADC1_EXTTRIG_TIM,                           //select external trigger from timer 1
//            DISABLE,                                    //enable external trigger
//            ADC1_ALIGN_RIGHT,                           //align ADC data to right
//            ADC1_SCHMITTTRIG_CHANNEL0,                  //any Schmit trigger
//            DISABLE);                                   //because it's disable
//  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,             //single conversion mode
//            ADC1_CHANNEL_3,                             //in scan mode, this is the last channel to conversion
//            ADC1_PRESSEL_FCPU_D4,                       //set prescaler to 4 (16MHz / 4 = 4MHz [MAX 6MHz])
//            ADC1_EXTTRIG_TIM,                           //select external trigger from timer 1
//            DISABLE,                                    //enable external trigger
//            ADC1_ALIGN_RIGHT,                           //align ADC data to right
//            ADC1_SCHMITTTRIG_CHANNEL0,                  //any Schmit trigger
//            DISABLE);  
//  ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS, 
//                        ADC1_CHANNEL_2, 
//                        ADC1_ALIGN_RIGHT);
  
  ADC1_ScanModeCmd(DISABLE);                                //enable scan mode                            //enable buffers for ADC
  ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);                    //enable End Of Conversion IT
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2,
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3, ENABLE);
  ADC1_Cmd(ENABLE);
}

void UART_Config(void){
  //PD5 UART TX
  //PD6 UART RX
  UART1_DeInit();
  UART1_Init((uint32_t)115200, 
             UART1_WORDLENGTH_8D, 
             UART1_STOPBITS_1, 
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, 
             UART1_MODE_TXRX_ENABLE);
  //UART1_ITConfig(UART1_IT_RXNE, ENABLE);
}

int putchar (char c)
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

  return (c);
}

void putstring(char* string)
{
	while(*string) // Het chuoi ky tu thi thoat
		{
		putchar(*string);
		string ++; // Lay ky tu tiep theo
		}
	putchar(0);
}

void UartSendInt(unsigned int n)
{
     unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
    	 putchar('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
    	 putchar(buffer[j]);
     }
}

void UartSendFloat(float x, unsigned char coma)
{
	unsigned long temp;
	if(coma>4)coma=4;
	// de trong 1 ki tu o dau cho hien thi dau
	if(x<0)
	{
		putchar('-');			//In so am
		x*=-1;
	}
	temp = (unsigned long)x;									// Chuyan thanh so nguyen.

	UartSendInt(temp);

	x=((float)x-temp);//*mysqr(10,coma);
	if(coma!=0) 
              putchar('.');	// In ra dau "."
	while(coma>0)
	{
		x*=10;
		coma--;
	}
        if(x<1000)
          putchar('0');
	temp=(unsigned long)(x+0.5);	//Lam tron
	UartSendInt(temp);
}




static void TIM1_Config(void)

{
   TIM1_DeInit();
  /* Time Base configuration */
  /*
  TIM1_Period = 4095
  TIM1_Prescaler = 0
  TIM1_CounterMode = TIM1_COUNTERMODE_UP
  TIM1_RepetitionCounter = 0
  */
  TIM1_TimeBaseInit(2, TIM1_COUNTERMODE_DOWN, 5250, 0);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  /*

  TIM1_OCMode = TIM1_OCMODE_PWM2

  TIM1_OutputState = TIM1_OUTPUTSTATE_ENABLE

  TIM1_OutputNState = TIM1_OUTPUTNSTATE_ENABLE

  TIM1_Pulse = CCR1_Val

  TIM1_OCPolarity = TIM1_OCPOLARITY_LOW

  TIM1_OCNPolarity = TIM1_OCNPOLARITY_HIGH

  TIM1_OCIdleState = TIM1_OCIDLESTATE_SET

  TIM1_OCNIdleState = TIM1_OCIDLESTATE_RESET
  */
  
  /*TIM1_Pulse = CCR3_Val*/

  TIM1_OC3Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE,
               CCR3_Val, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
               TIM1_OCNIDLESTATE_RESET);


  /*TIM1_Pulse = CCR4_Val*/

//  TIM1_OC4Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, CCR3_Val, TIM1_OCPOLARITY_LOW,
//               TIM1_OCIDLESTATE_SET);

  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);

  /* TIM1 Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
}

void main(void)
{

  CLK_Config();
  
  //LED PD3 Config
  GPIO_DeInit(GPIOD); 
  GPIO_Init(GPIOD, GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
  
  ADC_Config();         //PC4 AIN2 Channel 2    PD2 AIN3 Channel 3

  UART_Config();        //PD5 UART TX           PD6 UART RX
  
  TIM1_Config();        //PC3
    
  enableInterrupts();
  
  float Votage, Current, Resistance;
  uint16_t maxADC;
  uint16_t rawADC;
  /* Infinite loop */
  while (1)
  {
    //putstring("Starting meter....\n");
    GPIO_WriteHigh(GPIOD, GPIO_PIN_3);       //Blink Led PD3
    maxADC=0;
    delay_ms(200);
    for(uint16_t i=0; i<450; i++){
      /* start conversion */
      ADC1_StartConversion();
      /* wait conversion complete */
      while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == SET);
      /* get conversion value */
      rawADC = ADC1_GetConversionValue();
      
      if(maxADC<rawADC)
        maxADC=rawADC;
      delay_ms(20);
    }
    Votage=maxADC*(3.3/1024.0);
   
    GPIO_WriteLow(GPIOD, GPIO_PIN_3);       //Blink Led PD3
    maxADC=0;
    delay_ms(200);
    for(uint16_t i=0; i<450; i++){
      /* start conversion */
      ADC1_StartConversion();
      /* wait conversion complete */
      while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == SET);
      /* get conversion value */
      rawADC = ADC1_GetConversionValue();
      
      if(maxADC<rawADC)
        maxADC=rawADC;
      delay_ms(20);
    }
    
    Current=maxADC*(3.3/1024.0);
    
    Resistance=(Votage/Current)*0.05-0.042;
    
    putstring("Max Votage:  ");   UartSendFloat(Votage, 4);
    //putstring("\n");
    putstring("    Max Current: ");   UartSendFloat(Current, 4);
    //putstring("\n");
    putstring("    Resistance:  ");   UartSendFloat(Resistance, 4);
    putstring("\n"); 
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



