#include "key.h"
#include "stm8s.h"

_Bool KeyLL @(GPIOB_BaseAddress + 1) : 2;
_Bool KeyLT @(GPIOB_BaseAddress + 1) : 3;
_Bool KeyRT @(GPIOB_BaseAddress + 1) : 4;
_Bool KeyRR @(GPIOB_BaseAddress + 1) : 5;

uint8_t	oldkeys, curkeys, keys;
uint8_t scancount;
_Bool keyChange;

void scankeys(T_Keys_TypeDef *keys)
{
	keys->cur.b_0 = 1;
	/*
	if (KeyLL) 
		keys->cur |= 1; 
	else
		keys->cur &= 0;
	if (KeyLT)
		keys->cur |= (1 << 1);
	else
		keys->
	keys->cur =
	*/
	if (keys->old == keys->cur)
	{
		if (keys->count < MAX_SCANCOUNT)
			keys->count ++;
		if (keys->count >= MAX_SCANCOUNT)
		{
			if (keys->ok != key->cur)
			{
				keys->ok = keys->cur;
				keys->changed = TRUE;
			}
		}
	} else {
		keys->old = keys->cur;
	}
}
