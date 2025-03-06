#include "LPC17xx.h"

#define ROW_MASK (0x0F)    // P0.0 to P0.3
#define COL_MASK (0xF0)    // P0.4 to P0.7

// Keypad layout (rows × columns)
const char KEYPAD[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Delay function (basic software delay)
void delay_ms(uint32_t ms) {
    uint32_t count = ms * 30000; // Approximate delay for 12 MHz clock
    while (count--) {
        __NOP();
    }
}

// Initialize GPIO pins for the keypad
void Keypad_Init(void) {
    // Configure P0.0 to P0.3 (rows) as output
    LPC_GPIO0->FIODIR |= ROW_MASK;

    // Configure P0.4 to P0.7 (columns) as input
    LPC_GPIO0->FIODIR &= ~COL_MASK;

    // Set all rows to high
    LPC_GPIO0->FIOSET = ROW_MASK;
}

// Initialize P0.8 for toggling
void Pin_Init(void) {
    // Configure P0.8 as output
    LPC_GPIO0->FIODIR |= (1 << 8);
    // Ensure P0.8 is initially low
    LPC_GPIO0->FIOCLR = (1 << 8);
}

// Scan the keypad and return the pressed key
char Keypad_Scan(void) {
    for (int row = 0; row < 4; row++) {
        // Set the current row low
        LPC_GPIO0->FIOCLR = (1 << row);

        // Check all columns
        for (int col = 0; col < 4; col++) {
            if (!(LPC_GPIO0->FIOPIN & (1 << (col + 4)))) { // Check if column is low
                // Debounce delay
                delay_ms(20);
                if (!(LPC_GPIO0->FIOPIN & (1 << (col + 4)))) { // Confirm press
                    // Restore row to high
                    LPC_GPIO0->FIOSET = (1 << row);

                    // Return the key
                    return KEYPAD[row][col];
                }
            }
        }

        // Restore the row to high
        LPC_GPIO0->FIOSET = (1 << row);
    }

    // No key pressed
    return '\0';
}

int main(void) {
    char key;

    // Initialize the keypad and P0.8
    Keypad_Init();
    Pin_Init();

    while (1) {
        // Scan the keypad
        key = Keypad_Scan();

        // If a key is pressed
        if (key != '\0') {
            // Toggle P0.8
            LPC_GPIO0->FIOPIN ^= (1 << 8);

            // Add a delay to prevent rapid toggling
            delay_ms(500);
        }
    }
}