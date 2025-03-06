#include "LPC17xx.h"

#define BUZZER_PIN (1 << 2) // Assuming DAC_IN2 is connected to P0.2

void delay_ms(uint32_t ms) {
    uint32_t count = ms * 30000; // Approximate delay for 12 MHz clock
    while (count--) {
        __NOP();
    }
}

void Buzzer_Init(void) {
    LPC_GPIO0->FIODIR |= BUZZER_PIN; // Set P0.2 as output
    LPC_GPIO0->FIOCLR = BUZZER_PIN; // Ensure buzzer is initially OFF
}

void Buzzer_On(void) {
    LPC_GPIO0->FIOSET = BUZZER_PIN; // Set P0.2 HIGH to turn on buzzer
}

void Buzzer_Off(void) {
    LPC_GPIO0->FIOCLR = BUZZER_PIN; // Set P0.2 LOW to turn off buzzer
}

int main(void) {
    Buzzer_Init(); // Initialize the buzzer pin

    while (1) {
        Buzzer_On();  // Turn on the buzzer
        delay_ms(1000); // Wait for 1 second
        Buzzer_Off(); // Turn off the buzzer
        delay_ms(1000); // Wait for 1 second
    }
}