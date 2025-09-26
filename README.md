# Embedded Systems Lab – MT7687 Examples

This repository contains six C projects for the MediaTek MT7687 (ARM Cortex-M4) platform, demonstrating GPIO, timing, I2C peripherals, ADC, and common sensors (OLED SSD1306, light sensor, ultrasonic HC‑SR04, DHT, and MPU‑9250/MPU‑6050 IMU + AK8963 magnetometer).

## Contents

- HW1: Button‑controlled LED blinking using GPIO and millisecond timer
- HW2: 16‑bit shift LED bar driven via GPIO bit‑banging (data/clock)
- HW3: OLED light meter – SSD1306 OLED over I2C + ADC sampling
- HW4: OLED ultrasonic range finder – HC‑SR04 style trigger/echo timing
- HW5: OLED + DHT temperature/humidity sensor via one‑wire‑like timing
- HW6: IMU dashboard – MPU‑9250/6050 accel/gyro + AK8963 magnetometer over I2C, with on‑screen telemetry

## Build and Run

These examples target the MT7687 SDK/HAL.

Prerequisites:
- MT7687 SDK with headers and HAL: `hal.h`, `system_mt7687.h`, `top.h`, `hal_i2c*.h`
- GNU ARM toolchain
- Board wired to sensors per the pinout below

Typical build (adjust paths/toolchain as needed):
```bash
arm-none-eabi-gcc -Os -I<mt7687_sdk_include_paths> -c HW1.c -o HW1.o
arm-none-eabi-gcc HW1.o -T<linker_script.ld> -o HW1.elf
```
Flash with your usual vendor tool/openocd, then connect UART at 115200‑8‑N‑1.

## Pinout Summary

All projects initialize UART logging at 115200 baud on `HAL_UART_0`.

- HW1 (LED + Button)
  - LED: `GPIO36` (output)
  - Button: `GPIO37` (input)

- HW2 (LED bar shift output + Button)
  - DATA: `GPIO0` (output)
  - CLK: `GPIO39` (output)
  - Button: `GPIO37` (input)

- HW3 (OLED + ADC light sensor)
  - I2C (SSD1306 at 0x3C): `GPIO27` (SCL), `GPIO28` (SDA)
  - ADC channel: `HAL_ADC_CHANNEL_0`

- HW4 (OLED + Ultrasonic distance)
  - I2C (SSD1306 at 0x3C): `GPIO27` (SCL), `GPIO28` (SDA)
  - Ultrasonic TRIG/ECHO (single pin timing): `GPIO0`

- HW5 (OLED + DHT temperature/humidity)
  - I2C (SSD1306 at 0x3C): `GPIO27` (SCL), `GPIO28` (SDA)
  - DHT data: `GPIO0`

- HW6 (IMU + OLED over I2C)
  - I2C bus: `GPIO27` (SCL), `GPIO28` (SDA)
  - IMU address: `0x68` (MPU‑9250/6050), magnetometer `0x0C` (AK8963)

## Program Behavior

- HW1
  - Press and hold button to arm; on release (>15 ms), toggles LED flashing mode.
  - When active, LED toggles every 500 ms. Logs: `LED FLASH = 0/1`.

- HW2
  - On each qualified button release, lights an additional 8 LEDs on a chained bar (max pattern up to 10 steps). Bit‑banged DATA/CLK stream updates bar; logs current mask.

- HW3
  - Initializes SSD1306 OLED, shows title, then displays `Light Value=` from ADC0 once per second. Also prints the value on UART.

- HW4
  - Drives an ultrasonic sensor on `GPIO0` (trigger then measure echo pulse width with 1 MHz GPT). Displays `Distance = X.X` cm on OLED and logs over UART.

- HW5
  - Reads DHT sensor using timing‑based one‑wire protocol on `GPIO0`. Displays and logs temperature and humidity every 2 s.

- HW6
  - Configures IMU full‑scale ranges, reads accel/gyro/temp directly and magnetometer via pass‑through. Displays temperature plus G/A/M vectors on OLED; logs all values at ~10 Hz.

## Notes

- Delay/timing: Uses `hal_gpt_delay_ms/us` and GPT free‑run counters for precise measurements.
- OLED: Minimal SSD1306 helpers for page mode text; built‑in 5x8 font tables in each example.
- Safety: Debounce thresholds are simple counters; adapt to your hardware needs.
- SDK integration: Replace include paths and linker script per your MT7687 SDK setup.

## License

Provided for educational use in embedded systems labs.

