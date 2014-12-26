#include "stm8s.h"

typedef struct {
	_Bool		Enable;			
	_Bool 		Mode;			//master/slave
	uint8_t		StationAddr;	//station Address
	uint8_t		RetryTimes;		//retry times
	uint8_t		TimeOut;		//unit = 1ms
	MMState		state;			//Modbus Machine state
	
}
RTU_CHANNEL;
/* request rtu master task */
void rtu_master_cmd(RTU_CHANNEL* RTU_CHNx, RTU_FUN cmd, RTU_ADDR addr, 
					 uint8_t count, uint8_t* buff)
void rtu_master_logic();
void rtu_master_addr_range();
void rtu_master_return(RTU_CHANNEL* RTU_CHNx, RTU_FUN cmd, RTU_ADDR addr,
					 uint8_t* buff, RTU_RET_CODE retCode);

void rtu_slave_recv();
void rtu_slave_logic();			//user slave logic
void rtu_slave_addr_range();	//user addr range
void rtu_slave_return();

