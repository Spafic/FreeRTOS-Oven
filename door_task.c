#include "tm4c.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "door_task.h"
#include "shared_data.h"
#include "sensors.h"

// Threshold for light sensor
#define LIGHT_THRESHOLD 2000

void PWM0_Init(void);
void Servo_SetAngle(int angle);
void LED_Init(void);
void LED_Set(int on);

void Door_Task(void *pvParameters) {
		(void)pvParameters;
    PWM0_Init();
    LED_Init();
    uint16_t lightValue;
    while(1) {
    lightValue = Sensors_ReadDoorLightRaw();
        if(lightValue < LIGHT_THRESHOLD) {
            Servo_SetAngle(0);   // Door open
            LED_Set(1);          // LED ON (PF3)
            DisplayMsg_t m = { .field = DISP_FIELD_DOOR, .value = 1};
            Shared_TrySendDisplay(&m);
        } else {
            Servo_SetAngle(90);  // Door closed
            LED_Set(0);          // LED OFF (PF3)
            DisplayMsg_t m = { .field = DISP_FIELD_DOOR, .value = 0};
            Shared_TrySendDisplay(&m);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

extern uint32_t SystemCoreClock;

void PWM0_Init(void) {
    // Use PB6 M0PWM0 (original) which does not conflict with PB2/PB3 I2C
    SYSCTL_RCGCPWM_R |= 0x01;    // Enable PWM0
    SYSCTL_RCGCGPIO_R |= 0x02;   // Enable Port B
    SYSCTL_RCC_R |= (1<<20);     // Use PWM divider
    // Set PWM clock = SystemClock/8 (PWMDIV=0b011 -> 0x00030000)
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000E0000) | 0x00030000;

    GPIO_PORTB_AFSEL_R |= (1<<6);
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x0F000000) | 0x04000000;
    GPIO_PORTB_DEN_R |= (1<<6);

    PWM0_0_CTL_R = 0;
    PWM0_0_GENA_R = 0x0000008C;
    // Compute 50 Hz period from actual SystemCoreClock and PWMDIV=/8
    uint32_t sysclk = (SystemCoreClock ? SystemCoreClock : 16000000U);
    uint32_t pwmclk = sysclk / 8U;               // per divider configured above
    uint32_t load = (pwmclk / 50U) - 1U;         // 20 ms period
    if (load < 1000U) load = 1000U;              // sanity guard
    PWM0_0_LOAD_R = load;
    // Default to ~1.5 ms pulse (center)
    PWM0_0_CMPA_R = ((PWM0_0_LOAD_R + 1U) * 1500U) / 20000U;
    PWM0_0_CTL_R = 1;
    PWM0_ENABLE_R |= 0x01;      // Enable M0PWM0
}

void Servo_SetAngle(int angle) {
    // Map 0..180 deg -> 1000..2000 us pulse
    if (angle < 0) angle = 0; if (angle > 180) angle = 180;
    int pulse_us = 1000 + (angle * (2000 - 1000) / 180);
    // Convert pulse_us to counts based on the configured period (20 ms nominal)
    uint32_t load = PWM0_0_LOAD_R + 1;
    uint32_t cmp = (load * (uint32_t)pulse_us) / 20000U;
    if (cmp >= load) cmp = load - 1;
    PWM0_0_CMPA_R = (uint32_t)cmp;
}

void LED_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_DIR_R |= (1<<3); // PF3 for door LED
    GPIO_PORTF_DEN_R |= (1<<3);
}

void LED_Set(int on) {
    if(on) GPIO_PORTF_DATA_R |= (1<<3);
    else GPIO_PORTF_DATA_R &= ~(1<<3);
}

// light reading via sensors.c
