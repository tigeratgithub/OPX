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
	ADC1_StartConversion();
}

void TIM_Config(void)
{
	//设置TIM1定时器，预先分频32767，计数值80
	TIM1_DeInit();
	TIM1_TimeBaseInit(60000, TIM1_COUNTERMODE_UP, 40, 0);
	TIM1_Cmd(ENABLE);
}

s32 adc1_sum = 0;
s32 adc1_sumcount = 0;
_Bool adc1_update = FALSE;

void main(void)
{
	s32 sum, sumcount, icount = 0;
	float val = 0.0;
	
	char str[40] = {0};
	GPIO_Config();
	ADC_Config();
	//TIM_Config();
	/* Infinite loop */
	lcd1602_init(); 
	lcd1602_write_str(0, 1, "hello world!\0");
	while (1) 
	{
		val += 0.1 ;
		/*
		if (adc1_update != FALSE)
		{
			adc1_update = FALSE;
			ADC1->CR1 &= 0xfe;
			disableInterrupts();
			sum = adc1_sum;
			sumcount = adc1_sumcount;
			enableInterrupts();
			ADC1->CR1 &= 0xfe;
			
			if (sumcount >= 1000)
			{
				adc1_sum = 0; 
				adc1_sumcount = 1000;
			}

			val = (sum * 100.0) / (1024.0 * sumcount);
		}
		*/
		
		if (TIM1_GetFlagStatus(TIM1_FLAG_UPDATE) == SET)
		{
			val += 0.1;

			sprintf(str, "val=%3.2f%%", val);
			lcd1602_write_str(0, 0, str);
			/*
			sprintf(str, "count=%ld", icount ++);
			lcd1602_write_str(0, 1, str);
			TIM1_ClearFlag(TIM1_FLAG_UPDATE);
			*/
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
