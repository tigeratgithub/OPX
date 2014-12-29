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
#include "stdio.h"
#include "main.h"
#include "modstm8.h"
#include "key.h"

s32 adc1_sum = 0;
s32 adc1_sumcount = 0;
_Bool adc1_update = FALSE;

extern Mod_Master_Frame_TypeDef modFrame;
extern _Bool KeyLL, KeyLT, KeyRT, KeyRR;
static void ADC_Config(void);
static void GPIO_Config(void);
static void TIM_Config(void);

uint16_t v1, v2, v3, v4;
extern T_Keys_TypeDef tkeys;

_Bool TX_Pin @GPIOD_BaseAddress:7;

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void GPIO_Config()
{
	GPIO_Init(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_OD_LOW_FAST);
	GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_OD_LOW_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 
		GPIO_MODE_IN_FL_NO_IT);
		
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

void CLK_Config(void)
{
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, 				CLK_CURRENTCLOCKSTATE_DISABLE);
	//CLK_HSECmd(ENABLE);
}

void TIM_Config(void)
{
	//Prity = 1
	
	TIM1_DeInit();
	TIM1_TimeBaseInit(800, TIM1_COUNTERMODE_UP, 50, 0); 
	//main cycle
	TIM1_GenerateEvent(TIM1_EVENTSOURCE_UPDATE);	//20 = 20 * 100us = 2ms

	TIM1_UpdateRequestConfig(TIM1_UPDATESOURCE_REGULAR); 
	TIM1_UpdateDisableConfig(ENABLE);
	//update only interrupt

	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
	TIM1_ClearFlag(TIM1_FLAG_UPDATE);	
	TIM1_UpdateDisableConfig(DISABLE);
	TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
	TIM1_Cmd(ENABLE);
}

void main(void)
{
	s32 sum, sumcount, icount = 0;
	u16 tick1 = 0, tick2 = 0, tt = 0;
	float val = 0.0;
	uint8_t x;
	char str[40] = {0};

	CLK_Config();
	GPIO_Config();
	TIM_Config();
	
	ADC_Config();


	
	lcd1602_init();

	modbusRTUInit(57600);
	//mod_master_send(12, ReadHoldRegs, 100, 4);
	//sendFrame(2, &modFrame);
	//uart2SendStr("Hello World!");

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
		
		

			sprintf(str, "val=%3.2f%%", val);
			lcd1602_write_str(0, 0, str);

			v1 = (KeyLL) ? 1 : 0;
			v2 = (KeyLT) ? 1 : 0;
			v3 = (KeyRT) ? 1 : 0;
			v4 = (KeyRR) ? 1 : 0;
			
			sprintf(str, "len=%d %d %d %d", v1, v2, v3, v4);
			lcd1602_write_str(0, 1, str);
			/*
			if (uart2pipe.rxCursor == 0)
			{
				lcd1602_write_str(0, 1, str);
			} else {
				//x = uart2pipe.rxCursor;
				lcd1602_write_str(0, 1, uart2pipe.rxBuf);
				if (uart2pipe.rxBuf[0] == 'c')
					uart2pipe.rxCursor = 0;
			}
			//lcd1602_write_str(0, 1, uart2pipe.rxBuf);
			//uart2pipe.rxCursor = 0; 
			*/


		//com process
		
		

	}
	
  
}

//void wirteA5Param(uint16

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
