# Arduino RGB LED Strip Controller

Controls a 24V non-addressable common-cathode RGB LED strip via 7 buttons and three N-channel MOSFETs. Written as a standard Arduino sketch — works on any Arduino board (Uno, Nano, Mega, etc.).

## Hardware Requirements

- Any Arduino board with PWM on D9, D10, D11 (Uno, Nano, Mega, etc.)
- 24V non-addressable RGB LED strip (common-cathode / shared GND)
- 24V power supply
- Buck converter module (24V → 5V, e.g. LM2596) to power the Arduino
- 3× N-channel MOSFET — IRLZ44N recommended (logic-level, 55V 47A TO-220)
- 3× 100Ω resistor (gate series resistors)
- 3× 10kΩ resistor (gate pull-down resistors)
- 7× momentary push buttons

## Wiring

See `circuit_diagram.svg` for the full schematic.

### Buttons (D2–D8)

| Button | Pin | Function              |
|--------|-----|-----------------------|
| 1      | D2  | Toggle red channel    |
| 2      | D3  | Toggle green channel  |
| 3      | D4  | Toggle blue channel   |
| 4      | D5  | Increment red value   |
| 5      | D6  | Increment green value |
| 6      | D7  | Increment blue value  |
| 7      | D8  | Master toggle (on/off)|

All buttons wire between the pin and GND — internal pull-ups are enabled in software.

### MOSFET outputs (D9–D11)

| Pin | Channel | MOSFET |
|-----|---------|--------|
| D9  | Red     | IRLZ44N — drain to strip R−, source to GND |
| D10 | Green   | IRLZ44N — drain to strip G−, source to GND |
| D11 | Blue    | IRLZ44N — drain to strip B−, source to GND |

Each gate: Arduino pin → 100Ω → MOSFET gate, with 10kΩ from gate to GND.

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

| Constant        | Default | Description                                      |
|-----------------|---------|--------------------------------------------------|
| `BUTTON_PINS`   | D2–D8   | Digital pins for each button                     |
| `R_PIN`         | D9      | PWM output for red MOSFET gate                   |
| `G_PIN`         | D10     | PWM output for green MOSFET gate                 |
| `B_PIN`         | D11     | PWM output for blue MOSFET gate                  |
| `INCREMENT_STEP`| 32      | Brightness step per press (0–255, 8 steps = 32)  |
| `DEBOUNCE_MS`   | 50      | Button debounce window in milliseconds           |

## Installation

1. Open `rgb_led_controller/rgb_led_controller.ino` in the Arduino IDE
2. Select your board under **Tools → Board**
3. Select the correct port under **Tools → Port**
4. Click **Upload**

No external libraries required.
