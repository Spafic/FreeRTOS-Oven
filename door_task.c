#include "FreeRTOS.h"
#include "task.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define SERVO_PIN   (1U << 5)  // PB5
#define SERVO_MIN_PULSE  1000  // 1.0 ms
#define SERVO_MAX_PULSE  2000  // 2.0 ms
#define SERVO_PERIOD     20000 // 20 ms (50 Hz)

// ===== Servo Init using Timer1B =====
void Servo_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable Port B clock
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);

    GPIO_PORTB_AFSEL_R |= SERVO_PIN;         // Alternate function for PB5
    GPIO_PORTB_PCTL_R &= ~0x00F00000;
    GPIO_PORTB_PCTL_R |=  0x000700000;       // PB5 -> T1CCP1
    GPIO_PORTB_DEN_R  |= SERVO_PIN;          // Digital enable PB5

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Enable Timer1 clock
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1) == 0);

    TIMER1_CTL_R &= ~TIMER_CTL_TBEN;  // Disable Timer1B
    TIMER1_CFG_R = 0x4;               // 16-bit mode
    TIMER1_TBMR_R = TIMER_TBMR_TBMR_PERIOD | TIMER_TBMR_TBPWMIE | TIMER_TBMR_TBMRSU | TIMER_TBMR_TBAMS; 
    TIMER1_TBMR_R &= ~TIMER_TBMR_TBCDIR; // Count down

    // 20 ms period: 16 MHz / 8 prescaler = 2 MHz timer tick (0.5 us each)
    TIMER1_TBPR_R = (SERVO_PERIOD * 2) >> 16;
    TIMER1_TBILR_R = (SERVO_PERIOD * 2) & 0xFFFF;

    TIMER1_CTL_R |= TIMER_CTL_TBEN; // Enable Timer1B
}

// ===== Set servo angle =====
void Servo_SetAngle(uint8_t angle) {
    if (angle > 180) angle = 180;
    uint32_t pulse = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;
    uint32_t match = (SERVO_PERIOD - pulse) * 2; // in ticks (0.5 us)

    TIMER1_TBMATCHR_R = match & 0xFFFF;
    TIMER1_TBPMR_R = match >> 16;
}

// ===== ADC Init for door potentiometer on PD0 (AIN7) =====
void ADC_Init_Door(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // Enable Port D
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3) == 0);

    GPIO_PORTD_AFSEL_R |= 0x01; // PD0 as AIN7
    GPIO_PORTD_DEN_R &= ~0x01;  // Disable digital
    GPIO_PORTD_AMSEL_R |= 0x01; // Enable analog

    SYSCTL_RCGCADC_R |= 0x01;   // Enable ADC0
    while ((SYSCTL_PRADC_R & 0x01) == 0);

    ADC0_ACTSS_R &= ~0x08;      // Disable SS3
    ADC0_EMUX_R &= ~0xF000;     // Software trigger
    ADC0_SSMUX3_R = 7;          // AIN7 (PD0)
    ADC0_SSCTL3_R = 0x06;       // IE0, END0
    ADC0_ACTSS_R |= 0x08;       // Enable SS3
}

// ===== Task =====
void DoorTask(void *pvParameters) {
    uint32_t potValue;
    uint8_t angle;

    Servo_Init();
    ADC_Init_Door();

    while (1) {
        ADC0_PSSI_R = 0x08; // Start conversion SS3
        while ((ADC0_RIS_R & 0x08) == 0);
        potValue = ADC0_SSFIFO3_R & 0xFFF;
        ADC0_ISC_R = 0x08;

        angle = (potValue * 180) / 4095;
        Servo_SetAngle(angle);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}