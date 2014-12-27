/*********************************************************************
微 雪 电 子 WaveShare http://www.waveShare.net 
目 的: 建立LCD1602操作库
 目标系统: 基于STM8单片机 
 应用软件: Cosmic CxSTM8 
说 明: 若用于商业用途，请保留此段文字或注明代码来源
 深 圳 市 微 雪 电 子 有 限 公 司 保 留 所 有 的 版 权 
*********************************************************************/ 


/*
#define PORT_busy GPIOC->DDR=0x00;PB_CR1=0xff;
#define busy PB_IDR

#define PORT_OUT PB_DDR=0xff;PB_CR1=0xff;
#define PORT PB_ODR
*/

#include "lcd.h"


/*
#define EN_SET sbi(GPIOC->DDR,5);sbi(GPIOC->DDR,5);sbi(GPIOC->DDR,5);
#define EN_CLR sbi(PC_DDR,5);sbi(PC_CR1,5);cbi(PC_ODR,5);

#define RW_SET sbi(PC_DDR,6);sbi(PC_CR1,6);sbi(PC_ODR,6);
#define RW_CLR sbi(PC_DDR,6);sbi(PC_CR1,6);cbi(PC_ODR,6);

#define RS_SET sbi(PE_DDR,5);sbi(PE_CR1,5);sbi(PE_ODR,5);
#define RS_CLR sbi(PE_DDR,5);sbi(PE_CR1,5);cbi(PE_ODR,5);
*/

unsigned char reverse_byte(unsigned char x);

void sbi(uint8_t *r, uint8_t n) 
{ 
	disableInterrupts();
	(*r) |= (uint8_t)(1 << n); 
	enableInterrupts();
}

void sbi(uint8_t *r, uint8_t n);
void cbi(uint8_t *r, uint8_t n) ;
void cbi(uint8_t *r, uint8_t n) 
{ 
	disableInterrupts();
	(*r) &= (uint8_t)~(1 << n); 
	enableInterrupts();
}

void PORT_busy(void);
void PORT_busy()
{
	GPIOC->DDR = 0x00;
	GPIOC->CR1 = 0xff;
	GPIOE->DDR = 0x00;
	GPIOE->CR1 = 0xff;
}

uint8_t busy(void);
uint8_t busy(void)
{
	uint8_t val = 0;
	val = GPIOC->IDR & 0xfe;
	val |= (GPIOE->IDR & 0x20) == 0x20 ? 1 : 0;
	return reverse_byte(val);
}

void PORT_OUT(void);
void PORT_OUT(void)
{
	GPIOC->DDR = 0xff;
	GPIOC->CR1 = 0xff;
	GPIOE->DDR = 0xff;
	GPIOE->CR1 = 0xff;
}

void setData(uint8_t setVal);
void setData(uint8_t setVal)
{
	setVal = reverse_byte(setVal);
	GPIOC->ODR = setVal;
	GPIOE->ODR = (setVal << 0x05);
}

void EN_SET(void);
void EN_SET()
{
	sbi(&(GPIOD->DDR), 2);sbi(&(GPIOD->CR1),2);sbi(&(GPIOD->ODR),2);
}

void EN_CLR(void);
void EN_CLR()
{
	sbi(&(GPIOD->DDR), 2);sbi(&(GPIOD->CR1),2);cbi(&(GPIOD->ODR),2);
}

void RW_SET(void);
void RW_SET()
{
	sbi(&(GPIOD->DDR), 0);sbi(&(GPIOD->CR1),0);sbi(&(GPIOD->ODR),0);
}

void RW_CLR(void);
void RW_CLR()
{
	sbi(&(GPIOD->DDR), 0);sbi(&(GPIOD->CR1),0);cbi(&(GPIOD->ODR),0);
}

void RS_SET(void);
void RS_SET()
{
	sbi(&(GPIOD->DDR), 3);sbi(&(GPIOD->CR1),3);sbi(&(GPIOD->ODR),3);
}

void RS_CLR(void);
void RS_CLR()
{
	sbi(&(GPIOD->DDR), 3);sbi(&(GPIOD->CR1),3);cbi(&(GPIOD->ODR),3);
}

void delay_us(uint32_t xus);
/**us延时函数**/
void delay_us(uint32_t xus)
{
	uint8_t i;
	while(xus--)
	{
		i=7;
		while(i--);
	}
}

bool lcd1602_busy(void);
bool lcd1602_busy(void)
{
	bool result;
	RS_CLR();
	RW_SET();
	EN_SET();
	delay_us(5);
	PORT_busy();
	result=(busy() & 0x80);
	EN_CLR();
	return result;
}

void lcd1602_write_com(u8 com);
void lcd1602_write_com(u8 com)
{
	while(lcd1602_busy());
	RS_CLR();
	RW_CLR();
	EN_CLR();
	PORT_OUT();
	setData(com);
	EN_SET();
	delay_us(5);
	EN_CLR();
}

void lcd1602_write_data(u8 dat);
void lcd1602_write_data(u8 dat)
{
	while(lcd1602_busy());
	RS_SET();
	RW_CLR();
	EN_CLR();
	PORT_OUT();
	setData(dat);
	EN_SET();
	delay_us(5);
	EN_CLR();
}

void lcd1602_xy(u8 x,u8 y);
void lcd1602_xy(u8 x,u8 y)
{
	u8 add;
	if(y==0)
		add=0x80+x;
	else
		add=0xc0+x;
	lcd1602_write_com(add);
	
}

void lcd1602_init(void);
void lcd1602_init(void)
{
	delay_us(100);
	lcd1602_write_com(0x38);
	lcd1602_write_com(0x0c);
	lcd1602_write_com(0x06);
	lcd1602_write_com(0x01);
}

void lcd1602_write_char(uint8_t x,uint8_t y,uint8_t data);
/**LCD1602显示单个字符函数**/
void lcd1602_write_char(uint8_t x,uint8_t y,uint8_t data)
{
	lcd1602_xy(x,y);
	lcd1602_write_data(data);
}

void lcd1602_write_str(uint8_t x, uint8_t y, uint8_t *str);
void lcd1602_write_str(uint8_t x, uint8_t y, uint8_t *str)
{
	lcd1602_xy(x, y);
	while(*str)
	{
		lcd1602_write_data(*str);
		str ++;
	}
	
}

char table[12] = {0x20};
void lcd1602_display(void);
void lcd1602_display(void)
{
	uint8_t num;
	for(num=0;num<12;num++)
		{
			lcd1602_write_data(table[num]);
			delay_us(5);
		}
}


unsigned char reverse_byte(unsigned char x)  
{  
    static const unsigned char table[] = {  
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,  
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,  
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,  
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,  
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,  
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,  
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,  
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,  
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,  
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,  
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,  
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,  
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,  
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,  
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,  
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,  
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,  
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,  
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,  
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,  
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,  
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,  
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,  
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,  
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,  
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,  
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,  
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,  
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,  
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,  
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,  
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,  
    };  
    return table[x];  
} 
