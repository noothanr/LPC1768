#include "LPC17xx.h"
#include <stdint.h>

#define STEP_PIN  (1 << 10)  // Example: P0.10 as STEP pin
#define DIR_PIN   (1 << 11)  // Example: P0.11 as DIRECTION pin

void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++);
}

void initGPIO() {
    LPC_GPIO0->FIODIR |= STEP_PIN | DIR_PIN;  // Configure pins as output
}

void rotateStepper(uint32_t steps, uint8_t direction) {
    if (direction) {
        LPC_GPIO0->FIOSET = DIR_PIN;  // Set direction
    } else {
        LPC_GPIO0->FIOCLR = DIR_PIN;  // Clear direction
    }
    for (uint32_t i = 0; i < steps; i++) {
        LPC_GPIO0->FIOSET = STEP_PIN;  // Set STEP pin high
        delay(5000);                  // Adjust delay for speed
        LPC_GPIO0->FIOCLR = STEP_PIN;  // Set STEP pin low
        delay(5000);
    }
}

int main() {
    initGPIO();
    while (1) {
        rotateStepper(200, 1);  // Rotate 200 steps clockwise
        delay(1000000);
        rotateStepper(200, 0);  // Rotate 200 steps counter-clockwise
        delay(1000000);
    }
}
