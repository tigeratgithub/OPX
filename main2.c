/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "lcd.h"
#include "stdio.h"

static void ADC_Config(void);
static void GPIO_Config(void);
static void TIM_Config(void);

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void GPIO_Config()
{
	GPIO_Init(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);
}

void ADC_Config()
{
	ADC1_DeInit();

	//连续转换，通道号， 预分疲, 
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_0, 
		ADC1_PRESSEL_FCPU_D18, ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT,  
		ADC1_SCHMITTTRIG_CHANNEL0, DISABLE);
	ADC1_DataBufferCmd(ENABLE); //10 DBUF
	ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
	enableInterrupts();
	ADC1_StartConversion();
}

void CLK_Config(void)
{
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
	//CLK_HSECmd(ENABLE);
}

void TIM_Config(void)
{
	//设置TIM1定时器，预先分频32767，计数值80
	TIM1_DeInit();
	TIM1_TimeBaseInit(40000, TIM1_COUNTERMODE_UP, 200, 0);
	TIM1_Cmd(ENABLE);
}


void UART_Config()
{
  /* UART1 and UART3 configured as follow:
        - BaudRate = 230400 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
  */
  UART2_DeInit();
  UART2_Init((uint32_t)230400, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

  UART2_DeInit();
  UART2_Init((uint32_t)230400, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO,
              UART3_MODE_TXRX_ENABLE);
   
  /* Enable UART1 Half Duplex Mode*/
  UART1_HalfDuplexCmd(ENABLE);
	
}

s32 adc1_sum = 0;
s32 adc1_sumcount = 0;
_Bool adc1_update = FALSE;

void main(void)
{
	s32 sum, sumcount, icount = 0;
	u16 tick1 = 0, tick2 = 0, tt = 0;
	float val = 0.0;
	
	char str[40] = {0};
	GPIO_Config();
	CLK_Config();
	ADC_Config();
	TIM_Config();
	 
	lcd1602_init(); 
	
	while (1) 
	{
 
		if (adc1_update != FALSE)
		{
			adc1_update = FALSE;
			
			ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
			if (adc1_sumcount >= 300)
			{
				sum = adc1_sum;
				sumcount = adc1_sumcount;
				adc1_sum = 0;
				adc1_sumcount = 0;
			}
			ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
			if (sumcount >= 300)
				val = (sum * 100.0) / (1024.0 * sumcount);
		}
		

		if (TIM1_GetFlagStatus(TIM1_FLAG_UPDATE) == SET)
		{
			tick1 = TIM2_GetCounter();
			sprintf(str, "val=%3.2f%%", val);
			lcd1602_write_str(0, 0, str);
			tick2 = TIM2_GetCounter();
			tt = tick2 - tick1;

			sprintf(str, "c=%5d=", tt);
			lcd1602_write_str(0, 1, str);
			TIM1_ClearFlag(TIM1_FLAG_UPDATE);
		}

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

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
