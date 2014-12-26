#include "stm8s.h"

#ifndef _MODBUS_H
#define _MODBUS_H


#define MODBUS_RTU	0
#define MODBUS_ASC	1

#define ASC_BEGIN	':'
#define ASC_ENDX	(0x0A)
#define ASC_ENDY	(0x0D)

#define FRAME_OK			0
#define FRAME_NOK			1

#define MODBUS_STATE_INITIDLE	0
#define	MODBUS_STATE_IDLE 		1	//
#define MODBUS_STATE_RECV		4	//logic wait
#define MODBUS_STATE_WAITEND	3	//physic wait
#define MODBUS_STATE_EMIT		2	//logic send (include physic send)
#define MODBUS_STATE_SEND		5	//physic send

#define MODBUS_STATE	uint8_t

#define MODBUS_ERR	uint8_t			//modbus result
#define MODBUS_ERR_OK			0	//OK
#define MODBUS_ERR_NOCN			1	//no connect
#define MODBUS_ERR_CHK			2	//LRC,CRC error

#define MODBUS_LRC_OK			0
#define MODBUS_LRC_NOK			1
#define MODBUS_CRC_OK			0
#define MODBUS_CRC_NOK			1

#define MODBUS_RECV_TIMEOUT		1
#define MODBUS_EMIT_TIMEOUT		1
#define MODBUS_NOT_TIMEOUT		0

#define MODBUS_3P5_TIME			3	//3.5 t baudrate > 19.2k is 3ms

#define MODBUS_ASC_MAXFRAMELEN	513

// 9600,19200,and so    
// RTU or ASC           
// workstation Address  
// about 3              
// user specific 10ms OK
typedef struct  
{
	uint32_t	BaudRate; 			
	uint8_t		Mode;				
	uint8_t		StationAddr;			
	uint8_t		RetryTimes;			
	uint8_t		Timeout;			
} 
MODBUS_CLI_CFG;

typedef struct {
	uint8_t		frame[520];			//org frame data
	uint16_t	frameStart;			//recv buffer valid start 
	uint16_t	frameLen;				//org frame length
	uint16_t	chkStartPos;		//start position
	uint16_t	chkEndPos;			//end position
	uint8_t		frameOK;				//frame ok
} MODBUS_ASCFRAME;

typedef struct {
	uint32_t		BaudRate; 			// 9600,19200,and so
	uint8_t			Mode;				// RTU or ASC
	uint8_t			StationAddr;		// workstation Address
	uint8_t			RetryTimes;			// about 3 
	uint8_t			Timeout;			// user specific 10ms OK ?
	uint8_t			enabled;			// enabled;
	uint8_t			slave;				// slaved or master
	MODBUS_STATE	state;
	uint8_t			t1_5;			//alaway compare with 2ms
	uint8_t			recvT3_5;		//recv t3.5 char timer compare with 3ms
	uint8_t			recvTimeout;	//recv timeout flag
	uint8_t			emitT3_5;		//emit t3.5 char timer compare with 3ms
	uint8_t			emitTimeout;	//emit timeout flag						
	uint8_t			targetAddr;		//frame target addr
 	uint8_t			mFunCode;		//master function code
	uint8_t			sFunCode;		//slave function code 
	uint16_t		startAddr;		//data startAddr
	uint16_t 		dataLen;		//data Length
	uint8_t			data[256];		//ok
	uint8_t			errCode;		//modbus error code
	MODBUS_ERR		chanelErr;		//chanel error
	uint8_t			recvComplete;	//recv completed	
	uint8_t			sendComplete;	//send completed 1, not 0		
} MODBUS_CHANEL;

#define MODBUS_FUN_READ_COILS		0x01
#define MODBUS_FUN_READ_INPUTS		0x02
#define MODBUS_FUN_READ_HOLDREGS	0x03
#define MODBUS_FUN_READ_INPUTREGS	0x04
#define MODBUS_FUN_WRITE_COIL	   	0x05
#define MODBUS_FUN_WRITE_REG		0x06
#define MODBUS_FUN_WRITE_MCOILS		0x0f
#define MODBUS_FUN_WRITE_MREGS		0x10

#define MODBUS_ERR_ILLEFUN			0x01
#define MODBUS_ERR_ILLEADDR			0x02
#define MODBUS_ERR_ILLEVAL			0x03
#define MODBUS_ERR_ILLEOTHER		0x04 	//all other error 

#define MODBUS_SLAVE				0x01
#define MODBUS_MASTER				0x00	


MODBUS_ERR modbusProcess(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame, MODBUS_ASCFRAME *emitFrame);
MODBUS_STATE modbusASCInitIdle(MODBUS_CHANEL *chanelx);
MODBUS_STATE modbusASCIdle(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame, MODBUS_ASCFRAME *emitFrame);
MODBUS_STATE modbusASCRecv(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame);
MODBUS_STATE modbusASCEmit(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *emitFrame);
MODBUS_STATE modbusASCWaitEnd(MODBUS_CHANEL *chanelx, MODBUS_ASCFRAME *recvFrame);
uint16_t MODBUS_LRC_CAL(MODBUS_ASCFRAME *emitFrame);
uint8_t MODBUS_LRC_CHK(MODBUS_ASCFRAME *recvFrame);


//UTILE FUN




#endif
