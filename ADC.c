#include <lpc17xx.h>
#include <stdio.h>

void ADC_Init(void);
uint8_t ADC_Read(uint8_t channel);
void LCD_Init(void);
void LCD_Command(unsigned char cmd);
void LCD_Data(unsigned char data);
void LCD_DisplayString(char *str);
void delay_ms(uint32_t ms);

#define ADC_DATA_MASK 0xFF  // Mask for ADC Data lines (P0.0 to P0.7)
#define ADC_ALE (1 << 8)    // Address Latch Enable on P0.8
#define ADC_START (1 << 9)  // Start Conversion on P0.9
#define ADC_EOC (1 << 10)   // End of Conversion on P0.10
#define ADC_OE (1 << 11)    // Output Enable on P0.11

#define LCD_RS (1 << 15)    // LCD RS on P0.12
#define LCD_RW (1 << 16)    // LCD RW on P0.13
#define LCD_EN (1 << 17)    // LCD EN on P0.14

int main(void) {
    uint16_t adc_value;
    char buffer[16];

    // Initialize ADC and LCD
    ADC_Init();
    LCD_Init();

    while (1) {
        // Read ADC values from different channels and display on LCD
        LCD_Command(0x80); // Move cursor to the first line

        for (uint8_t channel = 0; channel < 4; channel++) {
            adc_value = ADC_Read(channel);

            sprintf(buffer, "CH%d: %d   ", channel, adc_value);
            LCD_DisplayString(buffer);

            delay_ms(1000); // Delay between channel readings
        }
    }
}

void ADC_Init(void) {
    LPC_GPIO0->FIODIR &= ~ADC_DATA_MASK;       // Set P0.0-P0.7 as input (ADC Data Bus)
    LPC_GPIO0->FIODIR |= ADC_ALE | ADC_START | ADC_OE; // Control signals as output
    LPC_GPIO0->FIODIR &= ~ADC_EOC;            // EOC as input
}

uint8_t ADC_Read(uint8_t channel) {
    uint8_t result;

    // Set channel address on data lines
    LPC_GPIO0->FIODIR |= ADC_DATA_MASK;       // Set P0.0-P0.7 as output
    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~ADC_DATA_MASK) | (channel & 0x07); // Set lower 3 bits for channel selection

    // Start conversion process
    LPC_GPIO0->FIOSET = ADC_ALE; // ALE = 1
    delay_ms(1);
    LPC_GPIO0->FIOCLR = ADC_ALE; // ALE = 0

    LPC_GPIO0->FIOSET = ADC_START; // START = 1
    delay_ms(1);
    LPC_GPIO0->FIOCLR = ADC_START; // START = 0

    // Wait for EOC (End of Conversion)
    while (!(LPC_GPIO0->FIOPIN & ADC_EOC));

    // Enable output and read data
    LPC_GPIO0->FIOSET = ADC_OE; // OE = 1
    delay_ms(1);
    result = LPC_GPIO0->FIOPIN & ADC_DATA_MASK; // Read 8-bit result
    LPC_GPIO0->FIOCLR = ADC_OE; // OE = 0

    return result;
}

void LCD_Init(void) {
    LPC_GPIO0->FIODIR |= ADC_DATA_MASK; // Set P0.0-P0.7 as output (data lines)
    LPC_GPIO0->FIODIR |= LCD_RS | LCD_RW | LCD_EN; // RS, RW, EN as output

    delay_ms(20);        // Wait for power stabilization
    LCD_Command(0x38);   // 8-bit, 2-line, 5x7 dots
    LCD_Command(0x0C);   // Display on, cursor off
    LCD_Command(0x06);   // Increment cursor
    LCD_Command(0x01);   // Clear display
    delay_ms(5);
}

void LCD_Command(unsigned char cmd) {
    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~ADC_DATA_MASK) | cmd; // Place command on data lines
    LPC_GPIO0->FIOCLR = LCD_RS; // RS = 0 for command
    LPC_GPIO0->FIOCLR = LCD_RW; // RW = 0 for write
    LPC_GPIO0->FIOSET = LCD_EN; // EN = 1
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN; // EN = 0
    delay_ms(2);
}

void LCD_Data(unsigned char data) {
    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~ADC_DATA_MASK) | data; // Place data on data lines
    LPC_GPIO0->FIOSET = LCD_RS; // RS = 1 for data
    LPC_GPIO0->FIOCLR = LCD_RW; // RW = 0 for write
    LPC_GPIO0->FIOSET = LCD_EN; // EN = 1
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN; // EN = 0
    delay_ms(2);
}

void LCD_DisplayString(char *str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++) {
        __NOP(); // No operation (simple delay)
    }
}
