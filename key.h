#include "stm8s.h" 
#ifndef _KEY_H_
#define _KEY_H_



typedef struct keyxstruct
{
	uint8_t	old;
	uint8_t cur;
	uint8_t ok;
	uint8_t	count;
	_Bool	changed;
} T_Keys_TypeDef;

void scankeys(T_Keys_TypeDef *keys);

#define MAX_SCANCOUNT 2

#endif
