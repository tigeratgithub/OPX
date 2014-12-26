#include "modbus.h"

static char bin2asc[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };





/*
#define MODBUS_STATE_INITIDLE	0
#define	MODBUS_STATE_IDLE 		1
#define MODBUS_STATE_EMIT		2
#define MODBUS_STATE_RECV		3
#define MODBUS_STATE_CTRL		4
*/
uint8_t	asc2bin(unsigned char * ch);

/**
	Modbus client state machine process
	author￡o tiger
	crate ￡o2011-02-13
**/
MODBUS_ERR modbusProcess(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame, MODBUS_ASCFRAME *emitFrame)
{
	uint16_t i;
	uint8_t	 chkStart;
	uint8_t* framePtr;
	switch (chanelx->state)
	{
		case MODBUS_STATE_INITIDLE: 
		{
			//do nothing can add something hardware or data struct
			break;
		}
		case MODBUS_STATE_IDLE:
		{
			// 1 funcode != 0 -> makeEmitContent
			// 2 recvChar == ':' -> Reception
			
			if (chanelx->mFunCode != 0)
			{
				emitFrame->frameLen = 0;
				chanelx->state = MODBUS_STATE_EMIT;
			} 
			else 
			{
				chkStart = 0;
				for (i = recvFrame->chkStartPos; i < recvFrame->frameLen; i ++)
				{
					if (recvFrame->frame[i] == ':')
					{
						recvFrame->frameStart = i;
						recvFrame->chkStartPos = i;
						recvFrame->chkEndPos = i;
						chanelx->state = MODBUS_STATE_RECV;
						chkStart = 1;
						break;
					}
				}
				if (chkStart == 0)
				{
					recvFrame->chkStartPos = recvFrame->frameLen;
					recvFrame->chkEndPos = recvFrame->frameLen;	
				} 
			}
			break;
		}
		case MODBUS_STATE_RECV:
		{
			/**
				1 判断帧结束，或者帧数据超过250
				2 校验帧 LRC or CRC
				3 物理层到逻辑层
				4 请求是否合法（站地址，内存地址范围检查）
				5 错误帧组建
				6 等待正确回应
			**/			
			for (i = recvFrame->chkEndPos; i < recvFrame->frameLen; i ++)
			{
			 	if (recvFrame->frame[i] == 0x0D)
				{
				 	chanelx->state = MODBUS_STATE_WAITEND;
					recvFrame->chkEndPos = i;
					break;	
				}
			}
			if (chanelx->state != MODBUS_STATE_WAITEND)
			{
				recvFrame->chkEndPos = recvFrame->frameLen;
				break;
			}
		}
		case MODBUS_STATE_WAITEND:
		{
			if (recvFrame->frameLen > recvFrame->chkEndPos)
			{
				if (recvFrame->frame[recvFrame->chkEndPos + 1] == 0x0A)
				{
				 	//1 check lrc
					//2 check station address
					//3 push to chanelx->data
					if (MODBUS_LRC_CHK(recvFrame) == MODBUS_LRC_OK)
					{
						framePtr = &(recvFrame->frame[recvFrame->chkStartPos]);
						framePtr ++;
						if (chanelx->StationAddr == asc2bin(framePtr)) 
						{
							//normal process recv
							
							chanelx->recvComplete = 1;		
						}
					}
					else
					{
					 	//lrc error
					}
					chanelx->state = MODBUS_STATE_IDLE;
				}
				else
				{
				 	//reload recv task
					chanelx->state = MODBUS_STATE_IDLE; 
				}	
			}
			break;
		}
		case MODBUS_STATE_EMIT:
		{
			modbusASCEmit(chanelx, emitFrame);
			break;
		}
		case MODBUS_STATE_SEND:
		{
		 	/* process by hardware:clear emit buffer, send emit buffer */
			break;
		}
		default:
		{
			//unknow state -> IDLE
			chanelx->state = MODBUS_STATE_IDLE;
			chanelx->chanelErr = MODBUS_ERR_OK;
		}
	}
	return chanelx->chanelErr;	
}

MODBUS_STATE modbusASCInitIdle(MODBUS_CHANEL *chanelx)
{
	//setup uart baudrate 1, 7, e, 1 
	//do nothing
	chanelx->state = MODBUS_STATE_IDLE;
	chanelx->t1_5 = 0;
	chanelx->recvT3_5 = 0;
	chanelx->emitT3_5 = 0;
	chanelx->targetAddr = 0;
	chanelx->mFunCode = 0;
	chanelx->sFunCode = 0;
	chanelx->startAddr = 0;
	chanelx->dataLen = 0;
	chanelx->errCode = 0;				//modbus protocol errcodeB
	chanelx->chanelErr = MODBUS_ERR_OK;	//for user

	return MODBUS_STATE_IDLE;
}

MODBUS_STATE modbusASCIdle(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame, MODBUS_ASCFRAME *emitFrame)
{

	return chanelx->state;
}


MODBUS_STATE modbusASCRecv(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame)
{
	return MODBUS_STATE_INITIDLE;
}

//#define MODBUS_FUN_READ_COILS		0x01
//#define MODBUS_FUN_READ_INPUTS		0x02
//#define MODBUS_FUN_READ_HOLDREGS	0x03
//#define MODBUS_FUN_READ_INPUTREGS	0x04
//#define MODBUS_FUN_WRITE_COIL	   	0x05
//#define MODBUS_FUN_WRITE_REG		0x06
//#define MODBUS_FUN_WRITE_MCOILS		0x0f
//#define MODBUS_FUN_WRITE_MREGS		0x10
//
//#define MODBUS_ERR_ILLEFUN			0x01
//#define MODBUS_ERR_ILLEADDR			0x02
//#define MODBUS_ERR_ILLEVAL			0x03
//#define MODBUS_ERR_ILLEOTHER		0x04 	//all other error 

#define MODHI(a)	((a & 0xf0) >> 4)
#define MODLO(a)	(a & 0x0f)


MODBUS_STATE modbusASCEmit(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *emitFrame)
{
	uint16_t i, j;
	uint16_t tmp = 0;
	uint8_t x = 0;
	uint8_t lrc = 0;

	if (chanelx->errCode == 0)
	{
		switch (chanelx->mFunCode)
		{
			case MODBUS_FUN_READ_COILS:
			{}
			case MODBUS_FUN_READ_INPUTS:
			{
				emitFrame->frame[0] = ':';
				emitFrame->frame[1] = bin2asc[MODHI(chanelx->targetAddr)];
				emitFrame->frame[2] = bin2asc[MODLO(chanelx->targetAddr)];
				emitFrame->frame[3] = bin2asc[MODHI(chanelx->mFunCode)];
				emitFrame->frame[4] = bin2asc[MODLO(chanelx->mFunCode)];
				emitFrame->frame[5] = bin2asc[MODHI(chanelx->startAddr)];
				emitFrame->frame[6] = bin2asc[MODLO(chanelx->startAddr)];
				emitFrame->frame[7] = bin2asc[MODHI(chanelx->dataLen)];
				emitFrame->frame[8] = bin2asc[MODLO(chanelx->dataLen)];

				lrc += (uint8_t)(chanelx->targetAddr);
				lrc += (uint8_t)(chanelx->mFunCode);
				lrc += (uint8_t)(chanelx->dataLen);
				lrc += (uint8_t)(chanelx->startAddr);
				lrc += (uint8_t)(chanelx->dataLen);
			}
			case MODBUS_FUN_READ_HOLDREGS:
			{}
			case MODBUS_FUN_READ_INPUTREGS:
			{}
			case MODBUS_FUN_WRITE_COIL:
			{}
			case MODBUS_FUN_WRITE_MCOILS:
			{}
			case MODBUS_FUN_WRITE_REG:
			{}
			case MODBUS_FUN_WRITE_MREGS:
			{}
			default:
			{}

		}

		switch (chanelx->sFunCode)
		{
			case MODBUS_FUN_READ_COILS:
			{ }
			case MODBUS_FUN_READ_INPUTS:
			{
				emitFrame->frame[0] = ':';
				emitFrame->frame[1] = bin2asc[MODHI(chanelx->targetAddr)];
				emitFrame->frame[2] = bin2asc[MODLO(chanelx->targetAddr)];
				emitFrame->frame[3] = bin2asc[MODHI(chanelx->sFunCode)];
				emitFrame->frame[4] = bin2asc[MODLO(chanelx->sFunCode)];
				if (chanelx->dataLen % 8 == 0)
				{
					emitFrame->frame[5] = bin2asc[MODHI(chanelx->dataLen >> 3)];
					emitFrame->frame[6] = bin2asc[MODLO(chanelx->dataLen >> 3)];
				} 
				else
				{
					emitFrame->frame[5] = bin2asc[MODHI((chanelx->dataLen >> 3) + 1)];
					emitFrame->frame[6] = bin2asc[MODLO((chanelx->dataLen >> 3) + 1)];
				}
				j = 7;
				lrc += (uint8_t)(chanelx->targetAddr);
				lrc += (uint8_t)(chanelx->sFunCode);
				lrc += (uint8_t)(chanelx->dataLen);
				for (i = 0; i < chanelx->dataLen; i ++)
				{
					x = (i & 0x07);
					if (chanelx->data[i] == 1) tmp |= (1 << x);
					if (x == 7)
					{
						emitFrame->frame[j ++] = bin2asc[MODHI(tmp)];
						emitFrame->frame[j ++] = bin2asc[MODLO(tmp)];
						lrc += tmp; 
						tmp = 0;
					}
				}
				if (x != 7) 
				{
					emitFrame->frame[j ++] = bin2asc[MODHI(tmp)];
					emitFrame->frame[j ++] = bin2asc[MODLO(tmp)];
					lrc += tmp;
				}
				if (lrc == 0) 
					lrc = 0;
				else
					lrc = (256 - lrc);
				emitFrame->frame[j ++] = bin2asc[MODHI(lrc)];
				emitFrame->frame[j ++] = bin2asc[MODLO(lrc)];
	
				emitFrame->frame[j ++] = 0x0D;
				emitFrame->frame[j ++] = 0x0A;
				break;
			}
			case MODBUS_FUN_READ_HOLDREGS:
			{ }
			case MODBUS_FUN_READ_INPUTREGS:
			{
				emitFrame->frame[0] = ':';
				emitFrame->frame[1] = bin2asc[MODHI(chanelx->targetAddr)];
				emitFrame->frame[2] = bin2asc[MODLO(chanelx->targetAddr)];
				emitFrame->frame[3] = bin2asc[MODHI(chanelx->sFunCode)];
				emitFrame->frame[4] = bin2asc[MODLO(chanelx->sFunCode)];
				emitFrame->frame[5] = bin2asc[MODHI(chanelx->dataLen << 1)];
				emitFrame->frame[6] = bin2asc[MODLO(chanelx->dataLen << 1)];
				j = 7;
				lrc += (uint8_t)(chanelx->targetAddr);
				lrc += (uint8_t)(chanelx->sFunCode);
				lrc += (uint8_t)(chanelx->dataLen);
				for (i = 0; i < chanelx->dataLen; i ++)
				{
				 	tmp = ((chanelx->data[i] & 0xf000) >> 12);
					emitFrame->frame[j ++] = bin2asc[tmp];
				 	tmp = ((chanelx->data[i] & 0xf00) >> 8);
					emitFrame->frame[j ++] = bin2asc[tmp];
				 	tmp = ((chanelx->data[i] & 0xf0) >> 4);
					emitFrame->frame[j ++] = bin2asc[tmp];
				 	tmp = (chanelx->data[i] & 0xf);
					emitFrame->frame[j ++] = bin2asc[tmp];
					lrc += tmp;
				}
				if (lrc == 0) 
					lrc = 0;
				else
					lrc = (256 - lrc);
				emitFrame->frame[j ++] = bin2asc[MODHI(lrc)];
				emitFrame->frame[j ++] = bin2asc[MODLO(lrc)];
	
				emitFrame->frame[j ++] = 0x0D;
				emitFrame->frame[j ++] = 0x0A;
				break;
			}
			case MODBUS_FUN_WRITE_COIL:
			{ }
			case MODBUS_FUN_WRITE_MCOILS:
			{ }
			case MODBUS_FUN_WRITE_REG:
			{ }
			case MODBUS_FUN_WRITE_MREGS:
			{
				emitFrame->frame[0] = ':';
				emitFrame->frame[1] = bin2asc[MODHI(chanelx->targetAddr)];
				emitFrame->frame[2] = bin2asc[MODLO(chanelx->targetAddr)];
				emitFrame->frame[3] = bin2asc[MODHI(chanelx->sFunCode)];
				emitFrame->frame[4] = bin2asc[MODLO(chanelx->sFunCode)];
	
				tmp = ((chanelx->startAddr & 0xf000) >> 12);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = ((chanelx->startAddr & 0xf00) >> 8);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = ((chanelx->startAddr & 0xf000) >> 4);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = (chanelx->startAddr & 0xf000);
				emitFrame->frame[j ++] = bin2asc[tmp];
	
				tmp = ((chanelx->dataLen & 0xf000) >> 12);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = ((chanelx->dataLen & 0xf00) >> 8);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = ((chanelx->dataLen & 0xf000) >> 4);
				emitFrame->frame[j ++] = bin2asc[tmp];
				tmp = (chanelx->dataLen & 0xf000);
				emitFrame->frame[j ++] = bin2asc[tmp];
	
				lrc += (uint8_t)(chanelx->targetAddr);
				lrc += (uint8_t)(chanelx->sFunCode);
				lrc += (uint8_t)((chanelx->startAddr & 0xff00) >> 8);
				lrc += (uint8_t)(chanelx->startAddr & 0xff);
	
				if (lrc == 0) 
					lrc = 0;
				else
					lrc = (256 - lrc);
				emitFrame->frame[j ++] = bin2asc[MODHI(lrc)];
				emitFrame->frame[j ++] = bin2asc[MODLO(lrc)];
	
				emitFrame->frame[j ++] = 0x0D;
				emitFrame->frame[j ++] = 0x0A;
			 	break;
			}
			default:
			{
			 	break;
			}
		}
	}
	else
	{
		emitFrame->frame[0] = ':';
		emitFrame->frame[1] = bin2asc[MODHI(chanelx->targetAddr)];
		emitFrame->frame[2] = bin2asc[MODLO(chanelx->targetAddr)];
		if (chanelx->slave == MODBUS_SLAVE)
			x = chanelx->sFunCode;
		else 
			x = chanelx->mFunCode;
		emitFrame->frame[3] = bin2asc[MODHI(x + 0x80)];
		emitFrame->frame[4] = bin2asc[MODLO(x + 0x80)];

		emitFrame->frame[5] = bin2asc[MODHI(chanelx->errCode)];
		emitFrame->frame[6] = bin2asc[MODLO(chanelx->errCode)];
		
		lrc += (uint8_t)(chanelx->targetAddr);
		lrc += (uint8_t)(x + 0x80);
		lrc += (uint8_t)(chanelx->errCode);

		if (lrc == 0) 
			lrc = 0;
		else
			lrc = (256 - lrc);
		emitFrame->frame[j ++] = bin2asc[MODHI(lrc)];
		emitFrame->frame[j ++] = bin2asc[MODLO(lrc)];

		emitFrame->frame[j ++] = 0x0D;
		emitFrame->frame[j ++] = 0x0A;

	}
	return MODBUS_STATE_INITIDLE;
}

MODBUS_STATE modbusASCWaitEnd(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame)
{
	
	return MODBUS_STATE_INITIDLE;
}

uint16_t MODBUS_LRC_CAL(MODBUS_ASCFRAME *emitFrame)
{
	return 0;
}

uint8_t MODBUS_LRC_CHK(MODBUS_ASCFRAME *recvFrame)
{
	uint16_t i;
	uint8_t lrc = 0;
	for (i = recvFrame->chkStartPos; i < recvFrame->chkEndPos - 3;i ++)
	{
		lrc += recvFrame->frame[i];
	}
	if (lrc != 0)
	{
		lrc = (256 - lrc);
	}
	if ((bin2asc[MODHI(lrc)]) == recvFrame->frame[recvFrame->chkEndPos - 3] &&
		(bin2asc[MODLO(lrc)]) == recvFrame->frame[recvFrame->chkEndPos - 2])
	{
		return MODBUS_LRC_OK;
	}
	else
	{
		return MODBUS_LRC_NOK;
	}
}

uint8_t	asc2bin(unsigned char * ch)
{
	int tmp = 0;
	if (*ch >= 0x30 && *ch < 0x3A)
	 	tmp = *ch - 0x30;
	else
		tmp = *ch - 0x37;
	tmp <<= 4;
	ch ++;
	if (*ch >= 0x30 && *ch < 0x3A)
	 	tmp |= *ch - 0x30;
	else
		tmp |= *ch - 0x37;
	return tmp;
}
