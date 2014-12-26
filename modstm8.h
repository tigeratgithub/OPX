#include "stm8s.h"

#ifndef __MODSTM8
#define __MODSTM8

#define READ_WORD(X) *(uint16_t*)(X)
#define WRITE_WORD(X, word) (*(uint16_t*)(X) = word)

typedef enum MOD_ROL
{
	Mod_Rol_Slave = 0x00,
	Mod_Rol_Master = 0x01
} Mod_Rol_TypeDef;

typedef enum mod_timer_check
{
	MOD_TIMER_START = 0x01,
	MOD_TIMER_PAUSE = 0x02,
	MOD_TIMER_RESUM = 0x03,
	MOD_TIMER_STOP  = 0x04
} Mod_Timer_Check_TypeDef;

typedef enum mod_master_state
{
	Mod_State_Init				= 0x00,
	Mod_State_Idle				= 0x01,
	Mod_State_BroadcastDelay 	= 0x02,
	Mod_State_WaitForReply		= 0x03,
	Mod_State_ProcessReply		= 0x04,
	Mod_State_ProcessErr		= 0x05,
	Mod_State_Sending			= 0x06
} Mod_Master_State_TypeDef;

//events source is 
//1. from timer (timeout)
//2. TX.RX INT
//3. user request
typedef enum mod_master_event
{
	Mod_Event_No				= 0x0000,
	Mod_Event_bc_Req			= 0x1000,
	Mod_Event_bc_Timeout		= 0x2000,
	Mod_Event_Send2Slave		= 0x0001,
	Mod_Event_UnSlave			= 0x0010,
	Mod_Event_OKSlave			= 0x0020,
	Mod_Event_RespTimeout		= 0x0040,
	Mod_Event_FrameError		= 0x0100,
	Mod_Event_EndReply			= 0x0200,
	Mod_Event_EndErrProcess		= 0x4000
} Mod_Master_Event_TypeDef; 

typedef enum mod_errcode
{
	Mod_Err_No				= 0x0,
	Mod_Err_IllFun			= 0x01,
	Mod_Err_IllDAddr		= 0x02,
	Mod_Err_IllDVal			= 0x03,
	Mod_Err_Unknow			= 0x04
} Mod_Exception_TypeDef;

#define MOD_MAX_DATA_LEN	32
#define MOD_MAX_BUF_LEN		50
#define MOD_MAX_RETRYS		3
//modbus master
typedef struct mod_master_frame
{
	uint16_t				baud;		//9=9.6k 19=19.2k 38=38.4 56= 56k 115 = 115200
	uint8_t				fromAddr;			//addr of workstation
	uint8_t				toAddr;
	uint8_t				retryCount;		//
	uint8_t				timeout;		//unit ms
	Mod_Rol_TypeDef		modRole;
	Mod_Master_State_TypeDef	modState;
	Mod_Master_Event_TypeDef	modEvent;
	uint8_t				cmdCode;
	uint16_t			dataAddr;
	uint16_t			dataLen;
	uint8_t				data[MOD_MAX_DATA_LEN];
	uint8_t				txframe[MOD_MAX_BUF_LEN];
	uint8_t				rxframe[MOD_MAX_BUF_LEN];
	uint8_t				txCursor;
	uint8_t				txLen;
	uint8_t				rxCursor;
	uint8_t				rxLen;
	_Bool				request;
	_Bool				txOK;
	_Bool				rxOK;
	_Bool				rxBufOver;
	_Bool				rxOver;	
	_Bool				linkFail;
	_Bool				heartbeat;
	uint8_t				errCode;
} Mod_Master_Frame_TypeDef;

/*
typedef struct mod_mem_addr
{
	
} Mod_Mem_Addr_TypeDef;
*/

typedef enum mod_cmd_code
{
	ReadDInput		= 0x02,
	ReadCoils		= 0x01,
	ReadHoldRegs	= 0x03,
	ReadInputRegs	= 0x04,
	WriteSingleCoil	= 0x05,
	WriteSingleReg	= 0x06,
	ReadExcepStatus	= 0x07,
	Diagnostic		= 0x08,
	WriteMultiCoils	= 0x0f,
	WriteMultiRegs	= 0x10
} Mod_Cmd_Code_TypeDef;

//void modCore(Mod_Master_Frame_TypeDef* aFrame);
void setTimeoutCheck(Mod_Timer_Check_TypeDef act);
void setFrameCheck(Mod_Timer_Check_TypeDef act);
void modbusRTUInit(uint32_t baud);
void MOD_UART_Config(uint32_t baud);
void MOD_TIM_Config(Mod_Master_Frame_TypeDef* aFrame);
void mod_master_send(uint8_t wsAddr, Mod_Cmd_Code_TypeDef cmdCode, 
		uint16_t dataAddr, uint8_t dataLen);
void modProcessRely(Mod_Master_Frame_TypeDef* aFrame);
unsigned short CRC16 (uint8_t *puchMsg, uint8_t usDataLen );
void frameProcessData(Mod_Master_Frame_TypeDef* aFrame);

void mod_int_frame_timeout(void);
void setINTPri(void);
void mod_int_rx(void);
void mod_int_tx(void);
void mod_int_timeout(void);

void setRX(void);
void setTX(void);

#endif