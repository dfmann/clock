# Arduino Nano RGB LED Strip Controller

Controls a 24V non-addressable common-cathode RGB LED strip via 7 buttons and three N-channel MOSFETs. Written as an Arduino sketch targeting the Arduino Nano (ATmega328P).

## Hardware Requirements

- Arduino Nano (ATmega328P)
- 24V non-addressable RGB LED strip (common-cathode / shared GND)
- 24V power supply
- Buck converter module (24V → 5V, e.g. LM2596 set to 5V) to power the Arduino Nano
- 3× N-channel MOSFET — IRLZ44N recommended (logic-level, 55V 47A TO-220)
- 3× 100Ω resistor (gate series resistors)
- 3× 10kΩ resistor (gate pull-down resistors)
- 7× momentary push buttons

## Wiring

See `circuit_diagram.svg` for the full schematic.

### Buttons

| Button | Pin | Function               | Notes                     |
|--------|-----|------------------------|---------------------------|
| 1      | D2  | Toggle red channel     |                           |
| 2      | D4  | Toggle green channel   |                           |
| 3      | D7  | Toggle blue channel    |                           |
| 4      | D8  | Increment red value    |                           |
| 5      | A1  | Increment green value  |                           |
| 6      | A2  | Increment blue value   |                           |
| 7      | A3  | Master toggle (on/off) |                           |

All buttons wire between pin and GND — internal pull-ups are enabled in software. No external pull-up resistors are required.

### MOSFET outputs

| Pin | Timer | Channel | MOSFET connection                          |
|-----|-------|---------|--------------------------------------------|
| D9  | OC1A  | Red     | IRLZ44N — drain to strip R−, source to GND |
| D10 | OC1B  | Green   | IRLZ44N — drain to strip G−, source to GND |
| D11 | OC2A  | Blue    | IRLZ44N — drain to strip B−, source to GND |

Each gate: Arduino pin → 100Ω → MOSFET gate, with 10kΩ from gate to GND.

### Power

- 24V PSU → LED strip +24V rail
- 24V PSU → Buck converter IN+ → Buck converter OUT+ (5V) → Arduino Nano 5V pin
- All GNDs (PSU −ve, buck GND, Arduino GND, MOSFET sources) share one bus

> **Important:** Power the Nano via its **5V pin** from the buck converter — do not feed 24V into VIN, as the onboard regulator cannot handle that voltage drop. The IRLZ44N MOSFETs switch fully at 5V gate voltage.

## Button Behaviour

| Button | Action |
|--------|--------|
| 1 | Toggles red channel on/off; turning ON sets red to mid-brightness and powers the strip |
| 2 | Toggles green channel on/off; turning ON sets green to mid-brightness and powers the strip |
| 3 | Toggles blue channel on/off; turning ON sets blue to mid-brightness and powers the strip |
| 4 | Increments red brightness one step (8 steps, wraps to 0), powers the strip on |
| 5 | Increments green brightness one step, powers the strip on |
| 6 | Increments blue brightness one step, powers the strip on |
| 7 | Master power toggle — turning ON picks a random colour; OFF blanks all LEDs |

When a channel button turns a channel on while the strip is powered off, the other two channels are zeroed so only that colour is active.

## Configuration

Edit the constants at the top of `rgb_led_controller/rgb_led_controller.ino`:

| Constant        | Default         | Description                                      |
|-----------------|-----------------|--------------------------------------------------|
| `BUTTON_PINS`   | 2,4,7,8,A1,A2,A3| Pin numbers for each button                      |
| `R_PIN`         | 9               | PWM output for red MOSFET gate (D9, OC1A)        |
| `G_PIN`         | 10              | PWM output for green MOSFET gate (D10, OC1B)     |
| `B_PIN`         | 11              | PWM output for blue MOSFET gate (D11, OC2A)      |
| `INCREMENT_STEP`| 32              | Brightness step per press (8 steps across 0–255) |
| `DEBOUNCE_MS`   | 50              | Button debounce window in milliseconds           |

PWM frequency is fixed at the hardware default (~490 Hz on D9/D10, ~980 Hz on D11).

## Installation

### 1. Linux USB permissions

```bash
sudo usermod -a -G dialout $USER
newgrp dialout
```

The Nano typically appears as `/dev/ttyUSB0` (CH340 chip) or `/dev/ttyACM0` (CP210x/FTDI chip).

### 2. Upload

1. Open `rgb_led_controller/rgb_led_controller.ino` in the Arduino IDE
2. **Tools → Board → Arduino AVR Boards** → select *Arduino Nano*
3. **Tools → Processor** → select *ATmega328P* (or *ATmega328P (Old Bootloader)* for clone boards)
4. **Tools → Port** → select `/dev/ttyUSB0` or `/dev/ttyACM0`
5. Click **Upload**

No external libraries required. No additional Boards Manager URL needed — Arduino AVR Boards is included with the Arduino IDE.
