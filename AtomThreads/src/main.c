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
#include <atom.h>
#include <atomtimer.h>
#include "atomport-private.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Constants */

/*
* Idle thread stack size
*
* This needs to be large enough to handle any interrupt handlers
* and callbacks called by interrupt handlers (e.g. user-created
* timer callbacks) as well as the saving of all context when
* switching away from this thread.
*
* In this case, the idle stack is allocated on the BSS via the
* idle_thread_stack[] byte array.
*/
#define IDLE_STACK_SIZE_BYTES       64


/*
* Main thread stack size
*
* Note that this is not a required OS kernel thread - you will replace
* this with your own application thread.
*
* In this case the Main thread is responsible for calling out to the
* test routines. Once a test routine has finished, the test status is
* printed out on the UART and the thread remains running in a loop
* flashing a LED.
*
* The Main thread stack generally needs to be larger than the idle
* thread stack, as not only does it need to store interrupt handler
* stack saves and context switch saves, but the application main thread
* will generally be carrying out more nested function calls and require
* stack for application code local variables etc.
*
* With all OS tests implemented to date on the STM8, the Main thread
* stack has not exceeded 256 bytes. To allow all tests to run we set
* a minimum main thread stack size of 204 bytes. This may increase in
* future as the codebase changes but for the time being is enough to
* cope with all of the automated tests.
*/
#define MAIN_STACK_SIZE_BYTES       128


/*
* Startup code stack
*
* Some stack space is required at initial startup for running the main()
* routine. This stack space is only temporarily required at first bootup
* and is no longer required as soon as the OS is started. By default
* Cosmic sets this to the top of RAM and it grows down from there.
*
* Because we only need this temporarily you may reuse the area once the
* OS is started, and are free to use some area other than the top of RAM.
* For convenience we just use the default region here.
*/


/* Linker-provided startup stack location (usually top of RAM) */
extern int _stack;


/* Local data */

/* Application threads' TCBs */
static ATOM_TCB main_tcb;
static ATOM_TCB greenLED_tcb;
/* Main thread's stack area (large so place outside of the small page0 area on STM8) */
NEAR static uint8_t main_thread_stack[MAIN_STACK_SIZE_BYTES];

/* Idle thread's stack area (large so place outside of the small page0 area on STM8) */
NEAR static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];

static void main_thread_func (uint32_t param);
static void greenLED_thread_func (uint32_t param);

static void CLK_Config(void)
{
  CLK_DeInit();
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      //f_Master = HSI/1 = 16MHz
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);            //f_CPU = f_Master/1 = 16MHz
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);     //wait until HSI ready
}


int putchar (char c)
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
  
  return (c);
}

char getchar (void)
{
  char c = 0;
  /* Loop until the Read data register flag is SET */
  while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
  c = UART1_ReceiveData8();
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

void main(void)
{
  
  int8_t status;
  
  CLK_Config();
  
  UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  
  
  GPIO_DeInit(GPIOD); 
  GPIO_Init(GPIOD, GPIO_PIN_3 , GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, GPIO_PIN_2 , GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, GPIO_PIN_4 , GPIO_MODE_IN_PU_IT);
  
  /* Initialise the OS before creating our threads */
  status = atomOSInit(&idle_thread_stack[IDLE_STACK_SIZE_BYTES - 1], IDLE_STACK_SIZE_BYTES);
  if (status == ATOM_OK)
  {
    /* Enable the system tick timer */
    archInitSystemTickTimer();
    
    /* Create an application thread */
    status = atomThreadCreate(&main_tcb,
                              3, main_thread_func, 0,
                              &main_thread_stack[MAIN_STACK_SIZE_BYTES - 1],
                              MAIN_STACK_SIZE_BYTES);
    status = atomThreadCreate(&greenLED_tcb,
                              2, greenLED_thread_func, 0,
                              &main_thread_stack[IDLE_STACK_SIZE_BYTES - 1],
                              IDLE_STACK_SIZE_BYTES);
    if (status == ATOM_OK)
    {
      /**
      * First application thread successfully created. It is
      * now possible to start the OS. Execution will not return
      * from atomOSStart(), which will restore the context of
      * our application thread and start executing it.
      *
      * Note that interrupts are still disabled at this point.
      * They will be enabled as we restore and execute our first
      * thread in archFirstThreadRestore().
      */
      putstring("atomOSStart()\n");
      atomOSStart();
    }
  }
  
  /* There was an error starting the OS if we reach here */
  while (1)
  {
  }
}

static void main_thread_func (uint32_t param)
{
  
  /* Compiler warnings */
  param = param;
  
  
  /* Test finished, flash slowly for pass, fast for fail */
  while (1)
  {
    putstring("main_thread_func()\n");
    //greenLED_tcb.suspended = TRUE;
    //greenLED_tcb.suspend_timo_cb = NULL;
    /* Toggle LED on pin B0 (STM8L mini board-specific) */
    GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
    
    /* Sleep then toggle LED again */
    atomTimerDelay(SYSTEM_TICKS_PER_SEC*5);
    
    putstring("Hello()\n");
    
    atomTimerDelay(SYSTEM_TICKS_PER_SEC*2);
  }
}

static void greenLED_thread_func (uint32_t param)
{
  /* Compiler warnings */
  param = param;
  putstring("greenLED_thread_func()\n");
  /* Test finished, flash slowly for pass, fast for fail */
  while (1)
  {
    /* Toggle LED on pin B0 (STM8L mini board-specific) */
    GPIO_WriteReverse(GPIOD, GPIO_PIN_2);
    
    /* Sleep then toggle LED again */
    atomTimerDelay(SYSTEM_TICKS_PER_SEC*4);
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



