#include "LPC17xx.h"

void delay_ms(uint32_t ms);
void displayDigits(unsigned char* digits);

unsigned char segment_codes[] = {
    0x3F, // 0
    0x06, // 1
    0x6B, // 2
    0x4F, // 3
    0x56, // 4
    0x5D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x5F, // 9
		0x77, //A
		0x7C, //b
		0x39, //C
		0x6E, //d
		0x79, //E
		0x71  //F
};

int main(void) 
{
    unsigned char digits[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    LPC_GPIO0->FIODIR |= 0xFF;
    LPC_GPIO0->FIODIR |= (1 << 19) | (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23) | (1 << 24);                  
    while (1) 
		{
        displayDigits(digits);                
    }
}

void displayDigits(unsigned char* digits) 
{
  for (int i = 0; i < 16; i++) 
		{
			LPC_GPIO0->FIOSET = (1 << 19) | (1 << 20) | (1 << 21) | 
                           (1 << 22) | (1 << 23) | (1 << 24); 
      LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~0xFF) | segment_codes[digits[i]];
			LPC_GPIO0->FIOCLR = (1 << 19) | (1 << 20) | (1 << 21) | 
                          (1 << 22) | (1 << 23) | (1 << 24);   
      delay_ms(500);
    }
}

void delay_ms(uint32_t ms) 
{
    uint32_t count = ms * 30000;
    while (count--) 
		{
        __NOP();
    }
}