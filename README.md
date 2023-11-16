# NeoPixel_MSP430

## Overview
This project is a C-based application for the MSP430 microcontroller, designed to control WS2812B LED strips. The code allows for various animations and color settings, controlled via a single push button.

## Memory and Device Configuration
- **Flash/FRAM usage**: 2448 bytes of 4KB (59.76% used)
- **RAM usage**: 112 bytes of 256B (43.75% used)
- **Microcontroller**: MSP4302332

## Device Pinouts
- **P1.0**: Push Button
- **P1.2**: DIN (WS2812B D1)
- **Other Pins**: Various functions including JTAG and ground/power connections.

## Code Structure
- **Configuration**: Definitions for DIN pin, button pin, and number of LEDs.
- **Macros**: `delay(x)` macro to facilitate timing. Also is used to check when the button was pressed.
- **Global Variables**: `state`, `change`, and an array `leds` to hold LED colors.
- **Functions**: Initialization, port configuration, main runnable, LED control, and animation functions.
- **Interrupt Service Routine**: Handles button press events.

## Main Features
- **LED Animations**: Includes several animations like breathing, racing, rainbow, blinking, waving, center-to-out, ping-pong, fade in/out, and build-up animations.
- **Button Control**: Cycling through animations using a push button.

## Usage
1. Flash this code to an MSP4302332 microcontroller.
2. Connect a WS2812B LED strip to the DIN pin (P1.2). Change the number of leds used in main.c file.
3. Use the push button (P1.0) to cycle through different LED animations.

## License
This project is open source. Feel free to use and modify as needed, keeping in mind to credit the original source.

## Note
Ensure the MSP430 is properly set up and that the WS2812B LED strip is compatible with the microcontroller's voltage and pin configuration.