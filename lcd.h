#include "stm8s.h"
#include "stm8s_gpio.h"

#ifndef USE_LCD1602
#define USE_LCD1602
void lcd1602_init(void);
void lcd1602_write_char(uint8_t x,uint8_t y,uint8_t data);
void lcd1602_write_str(uint8_t x, uint8_t y, uint8_t *str);
void lcd1602_display(void);
void delay_us(uint32_t xus);

#endif