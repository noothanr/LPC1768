#include <LPC17xx.h>

void delay_ms(uint32_t ms) {
    uint32_t count = ms * 30000; // Approximate delay (tuned for 12MHz clock)
    while (count--) {
        __NOP(); // No Operation - ensures the loop isn't optimized out
    }
}

int main(void) {
    // Configure P0.27, P0.28, P0.29, P0.30 as output
    LPC_GPIO0->FIODIR |= (1 << 27) | (1 << 28) | (1 << 29) | (1 << 30);

    // Configure P1.4, P1.1, P1.9, P1.10 as output
    LPC_GPIO1->FIODIR |= (1 << 4) | (1 << 1) | (1 << 9) | (1 << 10);

    while (1) {
        // Set pins high
        LPC_GPIO0->FIOSET = (1 << 27) | (1 << 28) | (1 << 29) | (1 << 30);
        LPC_GPIO1->FIOSET = (1 << 4) | (1 << 1) | (1 << 9) | (1 << 10);
        delay_ms(200);

        // Set pins low
        LPC_GPIO0->FIOCLR = (1 << 27) | (1 << 28) | (1 << 29) | (1 << 30);
        LPC_GPIO1->FIOCLR = (1 << 4) | (1 << 1) | (1 << 9) | (1 << 10);
        delay_ms(200);
    }
}
