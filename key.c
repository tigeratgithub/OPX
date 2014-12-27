#include "key.h"
#include "stm8s.h"

_Bool KeyLL @(GPIOB_BaseAddress + 1) : 2;
_Bool KeyLT @(GPIOB_BaseAddress + 1) : 3;
_Bool KeyRT @(GPIOB_BaseAddress + 1) : 4;
_Bool KeyRR @(GPIOB_BaseAddress + 1) : 5;

T_Keys_TypeDef tkeys;

void scankeys(T_Keys_TypeDef *keys)
{
	keys->cur = 0;
	if (KeyLL) 
		keys->cur |= 1; 
	else
		keys->cur &= 0xFE;
	if (KeyLT)
		keys->cur |= (1 << 1);
	else
		keys->cur &= 0xFD;
	if (KeyRT) 
		keys->cur |= (1 << 2); 
	else
		keys->cur &= 0xFB;
	if (KeyRR)
		keys->cur |= (1 << 3);
	else
		keys->cur &= 0xF7;
		


	if (keys->old == keys->cur)
	{
		if (keys->count < MAX_SCANCOUNT)
			keys->count ++;
		if (keys->count >= MAX_SCANCOUNT)
		{
			if (keys->ok != keys->cur)
			{
				keys->ok = keys->cur;
				keys->changed = TRUE;
			}
		}
	} else {
		keys->old = keys->cur;
		keys->count ++;
	}
}
