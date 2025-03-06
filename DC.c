#include "LPC17xx.h"

#define MOTOR_DIR1_PIN 0 // P0.0
#define MOTOR_DIR2_PIN 1 // P0.1
#define MOTOR_PWM_PIN  3 // P0.3

volatile uint32_t pwm_duty_cycle = 50; // Duty cycle (0-100)
volatile uint32_t pwm_period = 1000;  // Total period in microseconds

void TIMER1_Init(void) {
    LPC_SC->PCLKSEL0 |= (1 << 4);        // Set PCLK for Timer1
    LPC_TIM1->PR = 25 - 1;               // Prescaler for 1us tick (assuming 25 MHz clock)
    LPC_TIM1->MR0 = pwm_period;          // Set total period
    LPC_TIM1->MR1 = (pwm_period * pwm_duty_cycle) / 100; // Set on-time
    LPC_TIM1->MCR = (1 << 0) | (1 << 3); // Interrupt & Reset on MR0, Interrupt on MR1
    LPC_TIM1->TCR = 1;                   // Enable Timer1

    NVIC_EnableIRQ(TIMER1_IRQn);         // Enable Timer1 interrupt
}

void GPIO_Init(void) {
    LPC_GPIO0->FIODIR |= (1 << MOTOR_DIR1_PIN) | (1 << MOTOR_DIR2_PIN) | (1 << MOTOR_PWM_PIN);
    LPC_GPIO0->FIOCLR |= (1 << MOTOR_DIR1_PIN) | (1 << MOTOR_DIR2_PIN) | (1 << MOTOR_PWM_PIN);
}

void Motor_SetSpeed(uint32_t speed) {
    if (speed > 100) speed = 100;             // Clamp speed to 100%
    pwm_duty_cycle = speed;
    LPC_TIM1->MR1 = (pwm_period * pwm_duty_cycle) / 100; // Update on-time
}

void Motor_SetDirection(uint8_t direction) {
    switch (direction) {
        case 0: // Stop
            LPC_GPIO0->FIOCLR |= (1 << MOTOR_DIR1_PIN) | (1 << MOTOR_DIR2_PIN);
            break;
        case 1: // Forward
            LPC_GPIO0->FIOSET |= (1 << MOTOR_DIR1_PIN);
            LPC_GPIO0->FIOCLR |= (1 << MOTOR_DIR2_PIN);
            break;
        case 2: // Reverse
            LPC_GPIO0->FIOSET |= (1 << MOTOR_DIR2_PIN);
            LPC_GPIO0->FIOCLR |= (1 << MOTOR_DIR1_PIN);
            break;
    }
}

void TIMER1_IRQHandler(void) {
    if (LPC_TIM1->IR & (1 << 0)) { // Match on MR0 (end of period)
        LPC_GPIO0->FIOCLR |= (1 << MOTOR_PWM_PIN); // Turn off PWM pin
        LPC_TIM1->IR = (1 << 0);                  // Clear interrupt flag
    }
    if (LPC_TIM1->IR & (1 << 1)) { // Match on MR1 (start of on-time)
        LPC_GPIO0->FIOSET |= (1 << MOTOR_PWM_PIN); // Turn on PWM pin
        LPC_TIM1->IR = (1 << 1);                  // Clear interrupt flag
    }
}

int main(void) {
    GPIO_Init();
    TIMER1_Init();

    while (1) {
        Motor_SetDirection(1); // Forward
        Motor_SetSpeed(100);    // 50% speed
        for (volatile int i = 0; i < 1000000; i++); // Delay

        Motor_SetDirection(2); // Reverse
        Motor_SetSpeed(75);    // 75% speed
        for (volatile int i = 0; i < 1000000; i++); // Delay

        Motor_SetDirection(0); // Stop
        for (volatile int i = 0; i < 1000000; i++); // Delay
    }
}
