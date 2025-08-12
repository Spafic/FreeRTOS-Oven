# FreeRTOS-Oven: Wiring, Configuration, Debug, and Test Guide

This guide documents the required connections for the TM4C123 (Tiva C), the configuration options for the I2C LCD, how the FreeRTOS tasks interact, and how to debug and verify expected outputs.

## Hardware and Pinout

Board/MCU: TM4C123GXL (Tiva C LaunchPad) / TM4C123GH6PM

Reserved pins and functions:

- I2C LCD (I2C0):
  - PB2 -> SCL
  - PB3 -> SDA (open-drain; external pull-ups typically on LCD backpack)
- Servo (Door):
  - PB6 -> M0PWM0 (servo signal, 50 Hz)
- Buzzer:
  - PF1 -> Buzzer (active high)
- LEDs:
  - PF2 -> Temperature status (Blue)
  - PF3 -> Door status (Green)
- Light/Temperature analog sensor:
  - PE1 -> AIN2 (ADC0) [Door/Light pot]
  - PE2 -> AIN1 (ADC0) [Battery pot]
  - PE3 -> AIN0 (ADC0) [Temperature pot]
- Power and ground accordingly.

Notes:

- PB2/PB3 are dedicated to I2C0 for the LCD. Do not reuse.
- Servo must share ground with MCU. Prefer a separate power supply for the servo. Connect grounds.
- LCD backpack (PCF8574) usually provides the pull-ups on SDA/SCL; if not, add ~4.7k to 10k pull-ups to 3.3V.

## Components, voltages, polarities, and resistor values (SG90)

This project assumes an SG90 micro servo and a typical I2C LCD with a PCF8574 backpack.

- TM4C123 logic and ADC reference: 3.3 V (do not exceed 3.3 V on GPIO/ADC pins).
- SG90 servo:
  - Polarity: Brown = GND, Red = +5 V, Orange = PWM signal (from PB6 M0PWM0).
  - Voltage: 4.8–6.0 V; recommended 5.0 V.
  - Current: ~100–250 mA moving; up to 650–800 mA peak/stall. Use a dedicated 5 V supply rated ≥1 A. Always tie GND to LaunchPad GND.
- I2C LCD (PCF8574 backpack):
  - VCC: Many boards work at 3.3 V; some expect 5 V. If powered at 5 V, ensure I2C pull-ups are to 3.3 V (not 5 V). If the backpack’s on-board pull-ups go to 5 V, remove them and add external pull-ups to 3.3 V or use a level shifter.
  - Pull-up resistors (if needed): SDA to 3.3 V and SCL to 3.3 V with 4.7 kΩ (typical) to 10 kΩ each.
- Potentiometers (x3):
  - Value: 10 kΩ linear taper recommended (ends to 3.3 V and GND, wiper to PE3/PE2/PE1 as documented).
  - Optional filtering: 0.01–0.1 µF from wiper to GND for noise reduction. With 10 kΩ, 0.1 µF gives ~1 ms RC time constant.
- Buzzer (active, external):
  - If driving a 5 V active buzzer via transistor: NPN (e.g., 2N2222/SS8050). Base resistor 1 kΩ–4.7 kΩ (1 kΩ shown below). Add a flyback diode only if using an inductive load (not usually needed for an active piezo buzzer). If using a magnetic buzzer, add 1N4148/1N4007 across terminals (cathode to +, anode to −).
  - If using a small 3.3 V active buzzer directly from PF1, ensure current ≤10 mA and note PF1 also drives the on-board red LED.
- External LEDs (if used):
  - Series resistor: 220–330 Ω from GPIO to LED to limit current. On-board LEDs already have resistors.

Quick BOM:

- 1× SG90 micro servo (5 V).
- 1× External 5 V supply ≥1 A (buck converter/module) for the servo.
- 1× I2C 16×2 LCD with PCF8574 backpack.
- 2× 4.7 kΩ resistors for I2C pull-ups (only if backpack lacks 3.3 V pull-ups).
- 3× 10 kΩ potentiometers (linear).
- 1× NPN transistor (2N2222/SS8050) for a 5 V active buzzer.
- 1× 1 kΩ resistor (transistor base) — 2.2 kΩ–4.7 kΩ also acceptable.
- 1× Diode (1N4148/1N4007) across magnetic buzzer or relay (if used).
- Optional: 2–3× 0.01–0.1 µF capacitors for ADC filtering.

## Software Modules and Roles

- sensors.c/h: Centralized ADC0 setup and raw/converted reads
  - Sensors_Init(): configures ADC0 SS2 to sample AIN0(PE3), AIN1(PE2), AIN2(PE1).
  - Sensors_ReadTemperatureC(): PE3 -> 0..4095 mapped to 0..300 C (placeholder).
  - Sensors_ReadBatteryPercent(): PE2 -> 0..4095 mapped to 0..100%.
  - Sensors_ReadDoorLightRaw(): PE1 raw 12-bit value for door/light.
- door_task.c/h:
  - PWM0 init on PB6 (M0PWM0) and door LED PF3.
  - Opens servo (0°) when light below threshold; closes (90°) otherwise.
  - Publishes door state to display queue.
- temperature_task.c/h:
  - Reads temperature; controls PF2 LED; sends temperature updates and alerts.
- battery_task.c/h:
  - Reads battery %; drives buzzer PF1 with hysteresis; sends battery % and alerts.
- display_task.c/h:
  - Initializes I2C0 on PB2/PB3.
  - Drives a 16x2 LCD via PCF8574 backpack.
  - Consumes messages from queue and renders battery/temp/door and alerts.
- shared_data.c/h:
  - gDisplayQueue (size 8) for display messages.
  - gI2CMutex to serialize I2C access.
  - Task priorities and default stack sizes.
- display_config.h:
  - LCD address (0x27 default), I2C speed, and on-screen positions.

## Configuration

Edit `display_config.h` if needed:

- LCD address: `#define LCD_PCF8574_ADDR 0x27` (change to 0x3F if needed)
- I2C speed: `#define I2C0_SPEED_HZ 100000U`
- Screen layout positions for fields.

Move battery/door sensors to other ADC pins:

- If your pots are on different analog pins, update `ADC0_SSMUX2_R` ordering in `Sensors_Init()` (AIN numbers per TM4C123 datasheet) and adjust which channel each read uses.

## Detailed Connections (Wire-by-Wire)

Use the BoosterPack headers on the LaunchPad. Each pin is silk-screened with its port/pin (e.g., PE1). Connect by the GPIO label to avoid confusion with header numbering.

- Potentiometer 1 (Temperature control)

  - 10k pot recommended.
  - One end to 3.3V, other end to GND.
  - Wiper (middle) to PE3 (AIN0).
  - Optional: 0.1 µF capacitor from wiper to GND to reduce noise.

- Potentiometer 2 (Battery level simulation)

  - One end to 3.3V, other end to GND.
  - Wiper to PE2 (AIN1).
  - Optional: 0.1 µF to GND.

- Potentiometer 3 (Door/light level)

  - One end to 3.3V, other end to GND.
  - Wiper to PE1 (AIN2).
  - Optional: 0.1 µF to GND.

- I2C LCD (PCF8574 backpack)

  - PB2 (SCL) -> LCD SCL.
  - PB3 (SDA) -> LCD SDA. Configure as open-drain in code; ensure pull-ups to 3.3V.
  - 3.3V -> LCD VCC (if your backpack supports 3.3V; many do). If powering at 5V, make sure SDA/SCL pull-ups are to 3.3V, not 5V (TM4C pins are not 5V tolerant).
  - GND -> LCD GND.

- Servo (Door)

  - PB6 (M0PWM0) -> Servo signal (white/orange wire depending on model).
  - Servo V+ -> External 5 V supply (≥1 A recommended for SG90). Brown=GND, Red=+5 V, Orange=Signal.
  - Servo GND -> External supply GND and LaunchPad GND must be common.
  - Note: TM4C outputs 3.3V logic; most hobby servos accept it. If not, use a level shifter.

- LEDs (on-board)

  - PF2 (Blue) -> Temperature status (on-board LED; no external wiring needed).
  - PF3 (Green) -> Door status (on-board LED; no external wiring needed).

- Buzzer
  - PF1 -> Buzzer control (shares on-board red LED). For an active buzzer:
  - Use an NPN transistor: PF1 -> 1 kΩ -> base; emitter -> GND; collector -> buzzer (−); buzzer (+) -> 5 V. For magnetic/inductive loads, add a diode (1N4148/1N4007) across terminals (cathode to +, anode to −). Active piezo buzzers typically do not need a diode.
    - Alternatively, use a 3.3V active buzzer with low current and connect directly PF1 -> buzzer (+), buzzer (-) -> GND, but note PF1 already drives the on-board red LED.

Power/gnd summary:

- 3.3V rail -> all three pots high ends; LCD VCC (if 3.3V capable); logic pull-ups.
- 5V rail (optional) -> servo V+ and/or buzzer if needed. Always common ground with MCU.

Important cautions:

- Never exceed 3.3V on any analog input (PE1/PE2/PE3).
- Ensure I2C pull-ups go to 3.3V.
- Common ground between all modules (servo/LCD/buzzer) and the LaunchPad.
- Do not power the SG90 from the LaunchPad’s 5 V/USB rail if it cannot supply the peak current; brown-outs and resets will occur. Use a separate 5 V supply and common ground.

## Task Priorities

Configured in `shared_data.h`:

- Door task: priority 3 (highest among app tasks)
- Temperature task: priority 2
- Display task: priority 1
- Battery task: priority 1
- Idle task: priority 0

Rationale:

- Door reacts fast to sensor changes.
- Temperature moderate.
- Display and battery are service tasks; same priority is fine.

## Expected Runtime Behavior

- Display at startup:
  - Row 0: `Bat:XX%  Door:Open` or `Door:Close`
  - Row 1: `Temp:XXC  `
- Alerts:
  - Battery low: shows `BAT!` on row 1 right side; buzzer PF1 ON until cleared.
  - Temperature high: shows `TEMP!` on row 1 right side.
  - Clearing alert writes blanks in the alert region.
- Door task:
  - If light raw < threshold (default 2000 in `door_task.c`), servo angle -> 0°; PF3 LED ON.
  - Otherwise servo -> 90°; PF3 LED OFF.
- Temperature task:
  - If temperature > TEMP_THRESHOLD (default in `temperature_task.c`), PF2 LED ON and `TEMP!` alert.
  - Else PF2 LED OFF and alert cleared.
- Battery task:
  - Buzzer PF1 toggles with hysteresis (20% on, 25% off). Sends `BAT!`/clear alerts.

## Debugging Tips

- Basic electrical checks:
  - Verify 3.3V to LCD backpack and pull-ups on SDA/SCL.
  - Confirm servo power and common ground.
- I2C:
  - If display is blank, try changing `LCD_PCF8574_ADDR` between 0x27 and 0x3F.
  - Lower `I2C0_SPEED_HZ` if needed (e.g., 50000U).
- ADC reads:
  - Use breakpoints in `Sensors_ReadTemperatureC()` and `Sensors_ReadDoorLightRaw()` to inspect values.
  - Confirm `Sensors_Init()` is called once in `main.c` before tasks start.
- PWM/Servo:
  - Adjust `Servo_SetAngle()` mapping if your servo expects a different pulse range.
- Buzzer/LEDs:
  - Quick smoke: force thresholds to extreme values and observe PF1/PF2/PF3 toggling.

## Priority/Preemption Testing

- Raise workload in Display task (temporary `vTaskDelay`) and ensure Door task still reacts promptly (priority 3 should preempt).
- Temporarily make `Display_Render()` add artificial delay and confirm the queue doesn’t overflow (queue size 8). Increase if needed.
- Use FreeRTOS APIs:
  - `uxTaskPriorityGet(NULL)` in each task to print/log current priorities if you add UART logging.

## Building and Flashing

- Open `oven.uvprojx` in Keil uVision (or your IDE) and build.
- Flash to TM4C123GXL. Ensure jumpers for debugging are in place.

## Troubleshooting Common Issues

- LCD garbled text:
  - Ensure 4-bit init sequence timing – our driver delays are conservative; if issues persist, add slight delays (1–5 ms) between init commands.
- No I2C ACK:
  - Wrong address or no pull-ups. Try 0x27 vs 0x3F.
- Servo jitter:
  - Use stable power; increase PWM clock resolution by changing divider (in PWM init) if needed.
- ADC noise:
  - Add simple averaging in sensors if readings flicker.

## Future Improvements

- Move battery/door to dedicated ADC channels and configure a multi-step sequencer with averaging.
- Add UART logging for sensor/alert events.
- Encapsulate PWM in a small driver rather than in the door task.
- Expand the display to show setpoint/time if you add oven controls.
