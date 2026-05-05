# ESP8266 RGB LED Strip Controller

Controls a 24V non-addressable common-cathode RGB LED strip via 7 buttons and three N-channel MOSFETs. Written as an Arduino sketch targeting the ESP8266 (NodeMCU / Wemos D1 Mini or compatible).

## Hardware Requirements

- ESP8266 development board (e.g. NodeMCU v2/v3, Wemos D1 Mini)
- 24V non-addressable RGB LED strip (common-cathode / shared GND)
- 24V power supply
- Buck converter module (24V → 3.3V, e.g. LM2596 set to 3.3V) to power the ESP8266
- 3× N-channel MOSFET — IRLZ44N recommended (logic-level, 55V 47A TO-220)
- 3× 100Ω resistor (gate series resistors)
- 3× 10kΩ resistor (gate pull-down resistors)
- 7× momentary push buttons
- 1× additional 10kΩ resistor (external pull-up for BTN7 on GPIO16)

## Wiring

See `circuit_diagram.svg` for the full schematic.

### Buttons

| Button | GPIO | NodeMCU label | Function               | Notes                          |
|--------|------|---------------|------------------------|--------------------------------|
| 1      | 5    | D1            | Toggle red channel     |                                |
| 2      | 4    | D2            | Toggle green channel   |                                |
| 3      | 0    | D3            | Toggle blue channel    | Do not press at power-on       |
| 4      | 2    | D4            | Increment red value    | Do not press at power-on       |
| 5      | 3    | RX            | Increment green value  | Do not use Serial in sketch    |
| 6      | 1    | TX            | Increment blue value   | Avoid pressing during upload   |
| 7      | 16   | D0            | Master toggle (on/off) | Needs external 10kΩ to 3.3V   |

Buttons 1–6 wire between pin and GND — internal pull-ups enabled in software.
**Button 7 (GPIO16)** has no internal pull-up: wire a 10kΩ resistor from GPIO16 to 3.3V, then connect the button between GPIO16 and GND.

### MOSFET outputs

| GPIO | NodeMCU label | Channel | MOSFET connection                          |
|------|---------------|---------|--------------------------------------------|
| 12   | D6            | Red     | IRLZ44N — drain to strip R−, source to GND |
| 13   | D7            | Green   | IRLZ44N — drain to strip G−, source to GND |
| 14   | D5            | Blue    | IRLZ44N — drain to strip B−, source to GND |

Each gate: ESP8266 GPIO → 100Ω → MOSFET gate, with 10kΩ from gate to GND.

### Power

- 24V PSU → LED strip +24V rail
- 24V PSU → Buck converter IN+ → Buck converter OUT+ → ESP8266 3.3V pin
- All GNDs (PSU −ve, buck GND, ESP8266 GND, MOSFET sources) share one bus

> **Important:** ESP8266 runs at **3.3V** — set the buck converter to 3.3V, not 5V. The IRLZ44N MOSFETs switch fully at 3.3V gate voltage.

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

| Constant        | Default          | Description                                      |
|-----------------|------------------|--------------------------------------------------|
| `BUTTON_PINS`   | 5,4,0,2,3,1,16   | GPIO numbers for each button                     |
| `R_PIN`         | 12               | PWM output for red MOSFET gate (D6)              |
| `G_PIN`         | 13               | PWM output for green MOSFET gate (D7)            |
| `B_PIN`         | 14               | PWM output for blue MOSFET gate (D5)             |
| `PWM_FREQ`      | 1000             | PWM frequency in Hz (raise to 5000+ for cameras) |
| `INCREMENT_STEP`| 32               | Brightness step per press (8 steps across 0–255) |
| `DEBOUNCE_MS`   | 50               | Button debounce window in milliseconds           |

## Installation

### 1. Add ESP8266 board support to Arduino IDE

In Arduino IDE: **File → Preferences → Additional boards manager URLs**, add:

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

Then **Tools → Board → Boards Manager**, search for `esp8266` by ESP8266 Community, and install.

### 2. Linux USB permissions

```bash
sudo usermod -a -G dialout $USER
newgrp dialout
```

ESP8266 boards typically appear as `/dev/ttyUSB0` (CH340 chip) or `/dev/ttyACM0` (CP210x chip).

### 3. Upload

1. Open `rgb_led_controller/rgb_led_controller.ino` in the Arduino IDE
2. **Tools → Board → ESP8266 Boards** → select your board (e.g. *NodeMCU 1.0* or *LOLIN(WEMOS) D1 Mini*)
3. **Tools → Port** → select `/dev/ttyUSB0` or `/dev/ttyACM0`
4. Click **Upload**

No external libraries required.
