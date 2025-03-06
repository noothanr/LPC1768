#include "LPC17xx.h"

// Define GPIO pins for the LCD (P0.0 to P0.5 for RS, EN, D4-D7)
#define LCD_RS     (1 << 0)  // P0.0
#define LCD_EN     (1 << 1)  // P0.1
#define LCD_D4     (1 << 2)  // P0.2
#define LCD_D5     (1 << 3)  // P0.3
#define LCD_D6     (1 << 4)  // P0.4
#define LCD_D7     (1 << 5)  // P0.5

// Delay function for milliseconds
void delay_ms(uint32_t ms) {
    uint32_t count = ms * 30000; // Approximate delay (tuned for 12MHz clock)
    while (count--) {
        __NOP(); // No Operation - ensures the loop isn't optimized out
    }
}

// Send a command to the LCD
void LCD_Send_Command(uint8_t command) {
    LPC_GPIO0->FIOCLR = LCD_RS; // RS = 0 for command mode

    // Send higher nibble (4 bits)
    LPC_GPIO0->FIOCLR = (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Clear data pins
    LPC_GPIO0->FIOSET = ((command >> 4) & 0x0F) << 2;        // Set higher nibble
    LPC_GPIO0->FIOSET = LCD_EN;                              // Pulse EN
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;

    // Send lower nibble (4 bits)
    LPC_GPIO0->FIOCLR = (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Clear data pins
    LPC_GPIO0->FIOSET = (command & 0x0F) << 2;              // Set lower nibble
    LPC_GPIO0->FIOSET = LCD_EN;                              // Pulse EN
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

// Send data (character) to the LCD
void LCD_Send_Data(uint8_t data) {
    LPC_GPIO0->FIOSET = LCD_RS; // RS = 1 for data mode

    // Send higher nibble (4 bits)
    LPC_GPIO0->FIOCLR = (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Clear data pins
    LPC_GPIO0->FIOSET = ((data >> 4) & 0x0F) << 2;           // Set higher nibble
    LPC_GPIO0->FIOSET = LCD_EN;                              // Pulse EN
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;

    // Send lower nibble (4 bits)
    LPC_GPIO0->FIOCLR = (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Clear data pins
    LPC_GPIO0->FIOSET = (data & 0x0F) << 2;                  // Set lower nibble
    LPC_GPIO0->FIOSET = LCD_EN;                              // Pulse EN
    delay_ms(1);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

// Print a string on the LCD
void LCD_Print(char *message) {
    while (*message) {
        LCD_Send_Data(*message++);
    }
}

// Initialize the LCD
void LCD_Init() {
    // Configure GPIO pins P0.0 to P0.5 as GPIO
    LPC_PINCON->PINSEL0 &= ~0x00000FFF; // Set P0.0 to P0.5 as GPIO
    LPC_GPIO0->FIODIR |= (LCD_RS | LCD_EN | LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Set pins as output

    // Initialize the LCD in 4-bit mode
    delay_ms(20);        // Wait for LCD power-up
    LCD_Send_Command(0x02); // Initialize LCD to 4-bit mode
    delay_ms(1);
    LCD_Send_Command(0x28); // Function set: 4-bit, 2-line, 5x8 dots
    delay_ms(1);
    LCD_Send_Command(0x0C); // Display ON, Cursor OFF
    delay_ms(1);
    LCD_Send_Command(0x01); // Clear display
    delay_ms(2);
    LCD_Send_Command(0x06); // Entry mode set: Auto increment, No shift
    delay_ms(1);
}

int main(void) {
    // Initialize the LCD
    LCD_Init();

    // Set the cursor to the beginning of the first row
    LCD_Send_Command(0x80); // Move cursor to first row, first column

    // Display "UEDB BY SIMS" on the LCD
    LCD_Print("UEDB BY SIMS");

    // Set entire Port 0 as output
    LPC_GPIO0->FIODIR = 0xFFFFFFFF; // Configure all 32 pins of Port 0 as output

    while (1) {
        // Sequentially toggle the specific GPIO pins high and low
        LPC_GPIO0->FIOSET = 0x07F40000; // Set specific GPIO pins high
        delay_ms(200);
        LPC_GPIO0->FIOCLR = 0x07F40000; // Set specific GPIO pins low
        delay_ms(200);
    }

    return 0;
}