# ESP32 RGB LED Strip Controller

Controls a 24V non-addressable common-cathode RGB LED strip via 7 buttons and three N-channel MOSFETs. Written as an Arduino sketch targeting the ESP32.

## Hardware Requirements

- ESP32 development board (e.g. ESP32-WROOM-32 DevKit)
- 24V non-addressable RGB LED strip (common-cathode / shared GND)
- 24V power supply
- Buck converter module (24V → 5V, e.g. LM2596) to power the Arduino
- 3× N-channel MOSFET — IRLZ44N recommended (logic-level, 55V 47A TO-220)
- 3× 100Ω resistor (gate series resistors)
- 3× 10kΩ resistor (gate pull-down resistors)
- 7× momentary push buttons

## Wiring

See `circuit_diagram.svg` for the full schematic.

### Buttons (GPIO 4, 5, 16–19, 21)

| Button | GPIO | Function              |
|--------|------|-----------------------|
| 1      | 4    | Toggle red channel    |
| 2      | 5    | Toggle green channel  |
| 3      | 16   | Toggle blue channel   |
| 4      | 17   | Increment red value   |
| 5      | 18   | Increment green value |
| 6      | 19   | Increment blue value  |
| 7      | 21   | Master toggle (on/off)|

All buttons wire between the pin and GND — internal pull-ups are enabled in software.

### MOSFET outputs (GPIO 25, 26, 27)

| GPIO | Channel | MOSFET |
|------|---------|--------|
| 25   | Red     | IRLZ44N — drain to strip R−, source to GND |
| 26   | Green   | IRLZ44N — drain to strip G−, source to GND |
| 27   | Blue    | IRLZ44N — drain to strip B−, source to GND |

Each gate: ESP32 GPIO → 100Ω → MOSFET gate, with 10kΩ from gate to GND.

### Power

- 24V PSU → LED strip +24V rail
- 24V PSU → Buck converter IN+ → Buck converter OUT+ → Arduino 5V pin
- All GNDs (PSU −ve, buck GND, Arduino GND, MOSFET sources) share one bus

> **Important:** Set the buck converter output to exactly 5.0V before connecting it to the Arduino. Connect to the 5V pin, not VIN.

## Button Behaviour

| Button | Action |
|--------|--------|
| 1 | Toggles red channel on/off |
| 2 | Toggles green channel on/off |
| 3 | Toggles blue channel on/off |
| 4 | Increments red brightness by one step (8 steps, wraps around) and enables red |
| 5 | Increments green brightness by one step and enables green |
| 6 | Increments blue brightness by one step and enables blue |
| 7 | Master power toggle — turning ON picks a random colour; OFF blanks all LEDs |

## Configuration

Edit the constants at the top of `rgb_led_controller/rgb_led_controller.ino`:

| Constant        | Default        | Description                                      |
|-----------------|----------------|--------------------------------------------------|
| `BUTTON_PINS`   | 4,5,16–19,21   | GPIO pins for each button                        |
| `R_PIN`         | 25             | PWM output for red MOSFET gate                   |
| `G_PIN`         | 26             | PWM output for green MOSFET gate                 |
| `B_PIN`         | 27             | PWM output for blue MOSFET gate                  |
| `PWM_FREQ`      | 1000           | PWM frequency in Hz (raise to 5000+ for cameras) |
| `INCREMENT_STEP`| 32             | Brightness step per press (8 steps across 0–255) |
| `DEBOUNCE_MS`   | 50             | Button debounce window in milliseconds           |

## Installation

### 1. Add ESP32 board support to Arduino IDE

In Arduino IDE: **File → Preferences → Additional boards manager URLs**, add:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then **Tools → Board → Boards Manager**, search for `esp32` by Espressif, and install version **3.x**.

### 2. Linux USB permissions

```bash
sudo usermod -a -G dialout $USER
newgrp dialout
```

ESP32 boards typically appear as `/dev/ttyUSB0` (CH340 chip) or `/dev/ttyACM0` (CP210x chip).

### 3. Upload

1. Open `rgb_led_controller/rgb_led_controller.ino` in the Arduino IDE
2. **Tools → Board → ESP32 Arduino** → select your specific board (e.g. *ESP32 Dev Module*)
3. **Tools → Port** → select `/dev/ttyUSB0` or `/dev/ttyACM0`
4. Click **Upload**

No external libraries required.
