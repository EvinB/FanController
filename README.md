## PWM Fan Controller with LCD Status and Stall Detection

An embedded system for controlling and monitoring a DC fan using PWM, with dynamic LCD updates, stall detection, and real-time RPM measurement. Built using Microchip Studio and AVR C on the ATmega328P.

![App Screenshot](https://github.com/EvinB/FanController/blob/main/FanController.png)

## Project Overview 
- Controls fan speed via 20kHz PWM output on PD5
- Displays real-time RPM and fan status on a 2x16 LCD
- Detects stalls using a tachometer signal (INT1)
- Switches display modes using a pushbutton (PD2)
- Adjusts PWM duty cycle using a rotary pulse generator (RPG)
- Triggers a buzzer alert on fan stall (PB2 output)

## Features 
- PWM Fan Speed Control
  - Uses Timer0 in Phase Correct PWM (Mode 5) to generate 20kHz PWM on PD5.
- Duty Cycle Adjustment
  - COMPARE value (OCR0B) is controlled via RPG on PB0/PB1.
- Tachometer RPM Monitoring
  - Timer1 counts time between tach pulses on PD3 (INT1). RPM is calculated from the pulse period.
- LCD Modes (switchable with pushbutton on PD2):
  - Mode 0: Displays RPM (line 1) and duty cycle (line 2)
  - Mode 1: Displays RPM (line 1) and fan status ("Fan OK" or "Fan Low", line 2)
  - Stall: Automatically displays “FAN STALLING” and activates buzzer when no pulses are detected.
- LCD Driver
  - Adapted from Joerg Wunsch’s HD44780 library to support 2-line displays
 
## Hardware Used 
- ATmega328P microcontroller (Arduino Uno)
- 4-wire PWM fan with tachometer
- HD44780-compatible 2x16 LCD
- Rotary Pulse Generator (RPG) on PB0/PB1
- Pushbutton on PD2
- Buzzer on PB2
- Pull-up resistors on PD2 and PD3 as needed

## How It Works 
- Timer0 generates the PWM signal with customizable duty cycle
- Timer0 Overflow Interrupt updates the OCR0B register (COMPARE value)
- Timer1 counts clock ticks between tachometer edges (via INT1)
- Interrupt safely captures Timer1 value using atomic access to prevent race conditions
- LCD updates are driven by printf() using an adapted library stream
- Fan stall is inferred by Timer1 overflow (no tach pulses received)
