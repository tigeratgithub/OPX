#include "stm8s.h"
#include "modstm8.h"


#ifdef USE_FULL_ASSERT
void t_assert_failed(u8* file, u32 line);
#endif

extern _Bool TX_Pin;

uint8_t *DI, *COILS, *M, *V;

/* Table of CRC values for high–order byte */ 
const unsigned char auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/* Table of CRC values for low–order byte */ 
const char auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
}; 

Mod_Master_Frame_TypeDef modFrame;



void modbusRTUInit(uint32_t baud)
{
	modFrame.baud = baud;
	modFrame.retryCount = 0;
	modFrame.timeout = 60; //60ms
	modFrame.modRole = Mod_Rol_Master;
	modFrame.modState = Mod_State_Init;
	modFrame.modEvent = Mod_Event_No;
	modFrame.dataLen = 0;
	modFrame.txCursor = 0;
	modFrame.rxCursor = 0;
	modFrame.txLen = 0;
	modFrame.rxLen = 0;
	modFrame.txOK = FALSE;
	modFrame.rxOK = FALSE;
	modFrame.rxBufOver = FALSE;
	modFrame.rxOver = FALSE;
	modFrame.errCode = 0;
	
	/*
	ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_1);
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_TX, ITC_PRIORITYLEVEL_1);
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_RX, ITC_PRIORITYLEVEL_1);
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_TX, ITC_PRIORITYLEVEL_1);
	*/
	setINTPri();
	
	MOD_UART_Config(modFrame.baud);
	MOD_TIM_Config(&modFrame);
	modFrame.modState = Mod_State_Idle;
}

void MOD_UART_Config(uint32_t baud)
{
	/* UART1 and UART3 configured as follow:
        - BaudRate = 230400 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
	*/
	UART2_DeInit();
	UART2_Init(baud, UART2_WORDLENGTH_9D, UART2_STOPBITS_1, 
				UART2_PARITY_EVEN,
				UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);

	UART2_ClearITPendingBit(UART2_IT_RXNE);
	//UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
	//UART2_ITConfig(UART2_IT_TXE, ENABLE);
	UART2_Cmd(ENABLE);	
	//UART2_ClearITPendingBit(UART2_IT_IDLE);
	//UART2_ITConfig(UART2_IT_IDLE, ENABLE);
}

void MOD_TIM_Config(Mod_Master_Frame_TypeDef* aFrame)
{


	TIM2_DeInit();
	
	TIM2_TimeBaseInit(0x0F, 1500); //2ms comm response time
	TIM2_UpdateRequestConfig(TIM2_UPDATESOURCE_REGULAR);
	TIM2_GenerateEvent(TIM2_EVENTSOURCE_UPDATE);
	TIM2_Cmd(DISABLE);
	TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	
	TIM4_DeInit();
	
	TIM4_TimeBaseInit(0x07, 126);	//128 * 126 = 16128 = 2ms (8MHz), frame check
	TIM4_UpdateRequestConfig(TIM4_UPDATESOURCE_REGULAR);
	TIM4_GenerateEvent(TIM4_EVENTSOURCE_UPDATE);
	TIM4_Cmd(DISABLE);
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	
}

void setTimeoutCheck(Mod_Timer_Check_TypeDef act)
{
    /* Set the Autoreload value */
	//uint16_t arr;
	//arr = aFrame->timeout * 25;
	
	if (act == MOD_TIMER_START)
	{
		TIM2_Cmd(DISABLE);

		TIM2_SetCounter(0);

		TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	
		TIM2_Cmd(ENABLE);
	} else if (act == MOD_TIMER_PAUSE) {
		TIM2_Cmd(DISABLE);
	} else if (act == MOD_TIMER_RESUM) {
		TIM2_Cmd(ENABLE);
	} else if (act == MOD_TIMER_STOP) {
		TIM2_Cmd(DISABLE);
		TIM2_SetCounter(0);
	}
	
}

void setFrameCheck(Mod_Timer_Check_TypeDef act)
{
	if (act == MOD_TIMER_START)
	{
		TIM4_Cmd(DISABLE);
	
		//TIM4_UpdateDisableConfig(ENABLE);
		TIM4_SetCounter(0);
		//TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
		//TIM4_ClearFlag(TIM4_FLAG_UPDATE);
		//TIM4_UpdateDisableConfig(DISABLE);
		
		TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	
		TIM4_Cmd(ENABLE);
	} else if (act == MOD_TIMER_PAUSE) {
		TIM4_Cmd(DISABLE);
	} else if (act == MOD_TIMER_RESUM) {
		TIM4_Cmd(ENABLE);
	} else if (act == MOD_TIMER_STOP) {
		TIM4_Cmd(DISABLE);
		TIM4_SetCounter(0);
	}	

}

void setRX()
{
	//disableInterrupts();
	
	UART2_ITConfig(UART2_IT_TXE | UART2_IT_TC, DISABLE);
	UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
	
	//enableInterrupts();
}

void setTX()
{
	//disableInterrupts();
	
	UART2_ITConfig(UART2_IT_RXNE_OR, DISABLE);
	UART2_ITConfig(UART2_IT_TXE | UART2_IT_TC, DISABLE);
	UART2_ITConfig(UART2_IT_TXE, ENABLE);
	
	//enableInterrupts();	
}

void sendFrame(uint8_t ch, Mod_Master_Frame_TypeDef* aFrame)
{
	if (ch != 2) return;
	
	aFrame->request = FALSE;
	aFrame->txCursor = 0;
	aFrame->txOK = FALSE;


	//disableInterrupts();
	setTimeoutCheck(MOD_TIMER_STOP);
	setFrameCheck(MOD_TIMER_STOP);
	setTX();
	
	aFrame->modState = Mod_State_Sending;
	for (ch = 0; ch < 100; ch ++)
		TX_Pin = TRUE;

	UART2_SendData9(aFrame->txframe[0]);


	
	//enableInterrupts();
	
	return;
}

void modProcessRely(Mod_Master_Frame_TypeDef* aFrame)
{
	//for ProcessErr, Idle, waitforreply
	uint16_t crc,val;
	if (aFrame->modState == Mod_State_ProcessErr)
	{
		if (aFrame->modEvent == Mod_Event_RespTimeout)
		{
			if (aFrame->retryCount < MOD_MAX_RETRYS) //exceed max retry count
			{
				//resend , count ++
				aFrame->retryCount ++;
				aFrame->modState = Mod_State_Sending;
				sendFrame(2, aFrame);
				return;
			} else {
				//commucation error
				aFrame->retryCount = 0;
				aFrame->linkFail = TRUE;
				aFrame->modState = Mod_State_Idle;
				
				setTimeoutCheck(MOD_TIMER_STOP);
				setFrameCheck(MOD_TIMER_STOP);
				setTX();


				return;
			}			
		} else {
			//process exception code
			//todo 
		}
	}
	if (aFrame->modState == Mod_State_Idle)
	{
		if (aFrame->request == TRUE)
		{
			sendFrame(2, aFrame);
		}
	}
	//PROCESS REPLY
	if (aFrame->modState == Mod_State_ProcessReply)
	{		
		if (aFrame->rxCursor < 2) 
			aFrame->rxCursor = 10;
		crc = CRC16(aFrame->rxframe, aFrame->rxCursor - 2);
		val = ((uint16_t)(aFrame->rxframe[aFrame->rxCursor - 1]) << 8) + 
			aFrame->rxframe[aFrame->rxCursor - 2];
		if (crc != val)
		{
			if (aFrame->retryCount < MOD_MAX_RETRYS) //exceed max retry count
			{
				//resend , count ++
				aFrame->retryCount ++;
				aFrame->modState = Mod_State_Sending;
				sendFrame(2, aFrame);
				return;
			} else {
				//commucation error
				aFrame->retryCount = 0;
				aFrame->linkFail = TRUE;
				aFrame->modState = Mod_State_Idle;
				aFrame->request = FALSE;
				
				return;
			}
		} else {
			//process reply
			frameProcessData(aFrame);
			aFrame->retryCount = 0;
			aFrame->linkFail = FALSE;
			aFrame->modState = Mod_State_Idle;
			aFrame->request = FALSE;
			return;
		}
	}
	
}

void frameProcessData(Mod_Master_Frame_TypeDef* aFrame)
{
	//uint16_t 	*wp;
	//uint8_t 	*bp;
	uint16_t	i;
	uint8_t		*target;
	
	aFrame->fromAddr = aFrame->rxframe[0];
	//todo process
	if ((aFrame->rxframe[1] & 0x80) == 0x80) 
	{
		aFrame->errCode = aFrame->rxframe[2];
		//add err process FOR errCode
		return;
	} else {
		if (aFrame->cmdCode != aFrame->rxframe[1]) 
		{
			aFrame->errCode = Mod_Err_Unknow;
			//add err process for cmdCode err

			return;
		} else {
			//start process 
			switch (aFrame->cmdCode)
			{
				case 0x03:
				{
					aFrame->dataLen = aFrame->rxframe[2];
					target = M; //
					for (i = 0; i < aFrame->dataLen; i ++)
					{
						aFrame->data[i] = aFrame->rxframe[i + 3];
						//此处需要特别留意，移植的时候切记 start 
						*target = aFrame->data[i];
						target ++;						
						//此处需要特别留意，移植的时候切记 end
					}
					aFrame->errCode = Mod_Err_No;
					break;
				}
				case 0x10:
				{
					aFrame->dataAddr = READ_WORD(aFrame->rxframe[2]);
					aFrame->dataLen = READ_WORD(aFrame->rxframe[4]);
					aFrame->errCode = Mod_Err_No;
					break;
				}
				default :
				{
					//todo add cmd function process
				}
			}
			
			
		}
	}
	aFrame->modState = Mod_State_Idle;
	aFrame->request = FALSE;
	
}

/*
void modCore(Mod_Master_Frame_TypeDef* aFrame)
{
	int16_t crc, val;
	
	switch (aFrame->modState)
	{
		case Mod_State_Init:
		{
			//enable;
			
			break;
		}
		case Mod_State_Sending:
		{
			break;
		}
		case Mod_State_Idle:
		{
			switch (aFrame->modEvent)
			{
				case Mod_Event_Send2Slave:
				{
					sendFrame(2, aFrame);
					break;
				}
				case Mod_Event_bc_Req:
				{
					while(1);
					break;
				}
				default:
				{
					//do some err flag 
					while(1)
					{}
				}
			}
			break;
		}
		case Mod_State_WaitForReply:
		{
			switch (aFrame->modEvent)
			{
				case Mod_Event_UnSlave:
				{
					disableInterrupts();
					aFrame->rxCursor = 0;
					aFrame->rxBufOver = FALSE;
					aFrame->rxOver = FALSE;
					aFrame->rxOK = FALSE;
					
					enableInterrupts();
					break;
				}
				case Mod_Event_OKSlave:
				{
					//check crc, and adjust ok or fail 
					//crc = CRC16(aFrame->frame, aFrame->rxCursor - 2);
					if ( crc == (uint16_t)aFrame->rxframe[aFrame->rxCursor - 2])
					{
						//do for process;
					}
					break;
				}
				case Mod_Event_RespTimeout:
				{
					disableInterrupts();
					aFrame->rxCursor = 0;
					aFrame->rxBufOver = FALSE;
					aFrame->rxOver = FALSE;
					aFrame->rxOK = FALSE;
					UART2_Cmd(DISABLE);
					aFrame->modState = Mod_State_ProcessErr;
					enableInterrupts();
					break;
				}
				default:
				{
					//do some err flag
				}
			}
			break;
		}
		case Mod_State_ProcessReply:
		{
			switch (aFrame->modEvent)
			{
				case Mod_Event_EndReply:
				{
					
					break;
				}
				case Mod_Event_FrameError:
				{
					
					break;
				}
				default:			//noevent
				{
					
					//do some err flag
				}
			}
			break;
		}
		case Mod_State_ProcessErr:
		{
			switch (aFrame->modEvent)
			{
				case Mod_Event_EndErrProcess:
				{
					//add process
					aFrame->modState = Mod_State_Idle;
					break;
				}
				default:
				{
					//do some err flag
				}
			}
			break;
		}
	}
	
	


}
*/

/* The function returns the CRC as a unsigned short type  */
/* message to calculate CRC upon  */ 
/* quantity of bytes in message   */ 
unsigned short CRC16 (uint8_t *puchMsg, uint8_t usDataLen )  	 
{ 
	uint8_t uchCRCHi = 0xFF ;  					/* high byte of CRC initialized   */ 
	uint8_t uchCRCLo = 0xFF ;  					/* low byte of CRC initialized   */ 
	uint8_t uIndex ;  							/* will index into CRC lookup table   */ 
	 
	while (usDataLen--)  						/* pass through message buffer   */ 
	{ 
		uIndex = uchCRCLo ^ *puchMsg++ ;   			/* calculate the CRC   */ 
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ; 
		uchCRCHi = auchCRCLo[uIndex] ; 
	} 
	return (uchCRCHi << 8 | uchCRCLo) ; 
}



//所有状态机的处理
//void mod_machine{	
//}


void mod_master_recive()
{
	
}

void mod_master_send(uint8_t wsAddr, Mod_Cmd_Code_TypeDef cmdCode, 
		uint16_t dataAddr, uint8_t dataLen)
{
	uint8_t i, j, len;
	uint16_t crc;
	
	modFrame.toAddr = wsAddr;
	modFrame.cmdCode = cmdCode;
	modFrame.dataAddr = dataAddr;
	modFrame.dataLen = dataLen;
	switch (cmdCode)
	{
		case	ReadCoils:	
		case	ReadDInput:
		case 	ReadHoldRegs:
		case 	ReadInputRegs:
		{
			
			modFrame.txframe[0] = wsAddr;
			modFrame.txframe[1] = cmdCode;
			modFrame.txframe[2] = (uint8_t)(modFrame.dataAddr >> 8);
			modFrame.txframe[3] = (uint8_t)(modFrame.dataAddr & 0x00ff);
			modFrame.txframe[4] = (uint8_t)(modFrame.dataLen >> 8);
			modFrame.txframe[5] = (uint8_t)(modFrame.dataLen & 0x00ff);
			len = 5;
			
			crc = CRC16(modFrame.txframe, 6);
			modFrame.txframe[6] = (uint8_t)(crc & 0x00ff);
			modFrame.txframe[7] = (uint8_t)(crc >> 8);
			modFrame.txLen = 8;
			
			modFrame.request = TRUE;
			
			break;
		}
		case 	WriteSingleCoil:
		{
			modFrame.txframe[0] = wsAddr;
			modFrame.txframe[1] = cmdCode;
			modFrame.txframe[2] = (uint8_t)(modFrame.dataAddr >> 8);
			modFrame.txframe[3] = (uint8_t)(modFrame.dataAddr & 0x00ff);
			if (modFrame.data[0] == 0)
			{
				modFrame.txframe[4] = 0xff;
				modFrame.txframe[5] = 0x00;	
			} else {
				modFrame.txframe[4] = 0x00;
				modFrame.txframe[5] = 0x00;
			}
			crc = CRC16(modFrame.txframe, 6);
			modFrame.txframe[6] = (uint8_t)(crc & 0x00ff);
			modFrame.txframe[7] = (uint8_t)(crc >> 8);
			modFrame.txLen = 8;
			
			modFrame.request = TRUE;
			break;
		}
		case 	WriteSingleReg:
		{
			modFrame.txframe[0] = wsAddr;
			modFrame.txframe[1] = cmdCode;
			modFrame.txframe[2] = (uint8_t)(modFrame.dataAddr >> 8);
			modFrame.txframe[3] = (uint8_t)(modFrame.dataAddr & 0x00ff);
			modFrame.txframe[4] = modFrame.data[0];//或者倒过来
			modFrame.txframe[5] = modFrame.data[1];	

			crc = CRC16(modFrame.txframe, 6);
			modFrame.txframe[6] = (uint8_t)(crc & 0x00ff);
			modFrame.txframe[7] = (uint8_t)(crc >> 8);
			modFrame.txLen = 8;

			modFrame.request = TRUE;
			break;
			
		}
		case	WriteMultiCoils:
		{
			modFrame.txframe[0] = wsAddr;
			modFrame.txframe[1] = cmdCode;
			modFrame.txframe[2] = (uint8_t)(modFrame.dataAddr >> 8);
			modFrame.txframe[3] = (uint8_t)(modFrame.dataAddr & 0x00ff);
			modFrame.txframe[4] = (uint8_t)(modFrame.dataLen >> 8);
			modFrame.txframe[5] = (uint8_t)(modFrame.dataLen & 0x00ff);
			len = modFrame.dataLen;
			modFrame.txframe[6] = ((len % 8) == 1) ? (len % 8) : (len % 8) + 1;
			j = 6;
			while (len > 0)
			{
				for (i = 0; i < 8; i ++)
				{
					if (modFrame.data[i] == 0)
					{
						modFrame.txframe[j] &= (uint8_t)(~(1 << i)); 
					} else {
						modFrame.txframe[j] |= (uint8_t)(1 << i);
					}
					len --;
					if (len <= 0)
						break;
				}
				if (len <= 0) break;
				j ++;
			}
			crc = CRC16(modFrame.txframe, j + 1);
			
			modFrame.txframe[j + 1] = (uint8_t)(crc & 0x00ff);
			modFrame.txframe[j + 2] = (uint8_t)(crc >> 8);	
			modFrame.txLen = j + 2 + 1;

			modFrame.request = TRUE;
			break;
		}
		case 	WriteMultiRegs:
		{
			modFrame.txframe[0] = wsAddr;
			modFrame.txframe[1] = cmdCode;
			modFrame.txframe[2] = (uint8_t)(modFrame.dataAddr >> 8);
			modFrame.txframe[3] = (uint8_t)(modFrame.dataAddr & 0x00ff);
			modFrame.txframe[4] = (uint8_t)(modFrame.dataLen >> 8);
			modFrame.txframe[5] = (uint8_t)((modFrame.dataLen >> 1) & 0x00ff);
			modFrame.txframe[6] = (modFrame.dataLen);
			
			j = 7;
			for (i = 0; i < modFrame.dataLen; i ++)
			{
				modFrame.txframe[j] = modFrame.data[i];
				j ++;
			}
			crc = CRC16(modFrame.txframe, j);
			
			modFrame.txframe[j ++] = (uint8_t)(crc & 0x00ff);
			modFrame.txframe[j ++] = (uint8_t)(crc >> 8);	
			modFrame.txLen = j ++;

			modFrame.request = TRUE;
			
			break;
		}
		default:
		{
			break;
		}
	}
}
//////////////////////////////////////////////////
///////all interrupt function/////////////////////
//////////////////////////////////////////////////
void setmembuf(uint8_t *di, uint8_t *coils, uint8_t *m, uint8_t *v)
{
	DI = di;
	COILS = coils;
	M = m;
	V = v;
}

void setINTPri(void)
{
	disableInterrupts();
	ITC_SetSoftwarePriority(ITC_IRQ_TIM1_OVF, ITC_PRIORITYLEVEL_3);
	ITC_SetSoftwarePriority(ITC_IRQ_ADC1, ITC_PRIORITYLEVEL_1);
	ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_2);
	ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_2);
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_TX, ITC_PRIORITYLEVEL_3);
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_RX, ITC_PRIORITYLEVEL_3);
	enableInterrupts();
}

void mod_int_rx()
{
	uint16_t crc;
	uint8_t val;

	if (UART2_GetITStatus(UART2_IT_RXNE) == SET)
	{
		UART2_ClearFlag (UART2_FLAG_RXNE);
		if (modFrame.rxCursor < (sizeof(modFrame.rxframe) - 1))
		{
			if (modFrame.rxCursor == 11) 
				modFrame.rxframe[0] = 1; 
			modFrame.rxframe[modFrame.rxCursor] = UART2_ReceiveData8();
			modFrame.rxCursor ++;
			setFrameCheck(MOD_TIMER_START);

		}
		else 
		{
			//buffer is over
			setTimeoutCheck(MOD_TIMER_STOP);
			setFrameCheck(MOD_TIMER_STOP);
			modFrame.modState = Mod_State_ProcessReply;

			setTX();
	
			modFrame.rxBufOver = TRUE;
		}

	}
	if (UART2_GetITStatus(UART2_IT_OR) == SET)
	{
		//sr = UART2->SR;
		//sr = UART2->DR;
		UART2_ClearFlag (UART2_FLAG_RXNE);
		UART2_ClearITPendingBit(UART2_IT_RXNE);
		val = UART2->SR;
		val = UART2->DR;
		modFrame.modState = Mod_State_ProcessReply;

		setTimeoutCheck(MOD_TIMER_STOP);
		setFrameCheck(MOD_TIMER_STOP);
		setTX();


		modFrame.rxOver = TRUE;
	}
}

void mod_int_tx(void)
{
	uint16_t val;
	UART2_ClearITPendingBit(UART2_IT_RXNE);
	if (UART2_GetITStatus(UART2_IT_TXE) == SET)
	{

		if (modFrame.txCursor < (modFrame.txLen - 1))
		{
			modFrame.txCursor ++;
			UART2_SendData9(modFrame.txframe[modFrame.txCursor]);
			if (modFrame.txCursor >= modFrame.txLen - 1)
			{
				UART2_ITConfig(UART2_IT_TXE, DISABLE);
				UART2_ITConfig(UART2_IT_TC, ENABLE);
			}
		}

	}
	if (UART2_GetITStatus(UART2_IT_TC) == SET)
	{
		//setRX();

		
		if (modFrame.txCursor >= (modFrame.txLen - 1))
		{
			modFrame.txOK = TRUE;
			UART2_ITConfig(UART2_IT_TC, DISABLE);

			UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
			
			modFrame.rxOK = FALSE;
			modFrame.rxCursor = 0;
			modFrame.rxLen = 0;
			modFrame.rxBufOver = FALSE;
			modFrame.rxOver = FALSE;
			
			disableInterrupts()

			
			setTimeoutCheck(MOD_TIMER_START);
			setFrameCheck(MOD_TIMER_STOP);
			UART2_ITConfig(UART2_IT_RXNE_OR | UART2_IT_TXE | UART2_IT_TC, DISABLE);
			UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
			
			modFrame.modState = Mod_State_WaitForReply;
			for (val = 0; val < 100; val ++)
				TX_Pin = FALSE;
			
			modFrame.modEvent = Mod_Event_No;	//reset Event

			enableInterrupts();
		}
 
	}
}

void mod_int_timeout()
{
	if (TIM2_GetITStatus(TIM2_IT_UPDATE) == SET)
	{
		TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
		//Do something for response timeout
		//events & states think for overload
		
		modFrame.modState = Mod_State_ProcessErr;
		modFrame.modEvent = Mod_Event_RespTimeout;
		setTimeoutCheck(MOD_TIMER_STOP);
	}
}

void mod_int_frame_timeout()
{
	//exception Slave or unexception Slave
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);


	setTimeoutCheck(MOD_TIMER_PAUSE);
	setFrameCheck(MOD_TIMER_STOP);
	if (modFrame.rxframe[0] != modFrame.toAddr)
	{
		modFrame.rxCursor = 0;
		modFrame.rxOK = FALSE;
		setTimeoutCheck(MOD_TIMER_RESUM);
		setFrameCheck(MOD_TIMER_STOP);
	} else {
		modFrame.rxOK = TRUE;
		modFrame.modState = Mod_State_ProcessReply;
		modFrame.modEvent = Mod_Event_No;
		setTimeoutCheck(MOD_TIMER_STOP);
		setFrameCheck(MOD_TIMER_STOP);
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
void t_assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif