#include "LPC17xx.h"
#include <math.h> // For sine wave generation

#define PI 3.141592653589793
#define NUM_SAMPLES 256 // Number of samples for smoother sine wave
#define DAC_PORT LPC_GPIO0
#define DAC_PINS_MASK 0xFF

uint8_t sine_wave[NUM_SAMPLES];
volatile uint32_t sample_index = 0;

// Function Prototypes
void generate_sine_wave_table(void);
void DAC_Init(void);
void Timer_Init(uint32_t frequency);
void DAC_Write(uint8_t value);
void TIMER0_IRQHandler(void);

void generate_sine_wave_table(void) {
    for (uint32_t i = 0; i < NUM_SAMPLES; i++) {
        sine_wave[i] = (uint8_t)(127.5 * (1 + sin(2 * PI * i / NUM_SAMPLES))); // 8-bit DAC (0-255)
    }
}

void DAC_Init(void) {
    LPC_GPIO0->FIODIR |= DAC_PINS_MASK; // Set P0.0-P0.7 as outputs
}

void Timer_Init(uint32_t frequency) {
    LPC_SC->PCONP |= (1 << 1); // Power up Timer 0
    LPC_TIM0->PR = 0; // No prescaler
    LPC_TIM0->MR0 = SystemCoreClock / (frequency * NUM_SAMPLES); // Match value for the desired frequency
    LPC_TIM0->MCR = (1 << 0) | (1 << 1); // Interrupt and reset on MR0
    LPC_TIM0->TCR = (1 << 0); // Start Timer 0
    NVIC_EnableIRQ(TIMER0_IRQn); // Enable Timer 0 interrupt
}

void TIMER0_IRQHandler(void) {
    DAC_Write(sine_wave[sample_index++]); // Write the current sample to DAC
    if (sample_index >= NUM_SAMPLES) {
        sample_index = 0; // Reset to start of sine wave table
    }
    LPC_TIM0->IR = (1 << 0); // Clear interrupt flag
}

void DAC_Write(uint8_t value) {
    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~DAC_PINS_MASK) | (value & DAC_PINS_MASK);
}

int main(void) {
    DAC_Init(); // Initialize DAC
    generate_sine_wave_table(); // Generate sine wave lookup table
    Timer_Init(1000); // Set Timer frequency for 1 kHz sine wave

    while (1) {
        // CPU can perform other tasks while Timer and ISR handle sine wave generation
    }
}
