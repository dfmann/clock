# CircuitPython RGB LED Strip Controller

Controls an addressable RGB LED strip (NeoPixel/WS2812B) via 7 buttons on a CircuitPython-compatible Arduino board.

## Hardware Requirements

- CircuitPython-compatible Arduino board (e.g. Arduino Nano RP2040 Connect, Nano 33 BLE, Nano ESP32)
- WS2812B / NeoPixel compatible RGB LED strip
- 7 momentary push buttons
- 470Ω resistor in series with the LED data line (recommended)
- Decoupling capacitor (1000µF) across LED strip power rails (recommended)

## Wiring

| Button | Pin  | Function              |
|--------|------|-----------------------|
| 1      | D2   | Toggle red channel    |
| 2      | D3   | Toggle green channel  |
| 3      | D4   | Toggle blue channel   |
| 4      | D5   | Increment red value   |
| 5      | D6   | Increment green value |
| 6      | D7   | Increment blue value  |
| 7      | D8   | Master toggle (on/off)|

- LED strip data line → D9 (via 470Ω resistor)
- Buttons wired between pin and GND (internal pull-up enabled)

## Button Behaviour

| Button | Action |
|--------|--------|
| 1 | Toggles red channel on/off |
| 2 | Toggles green channel on/off |
| 3 | Toggles blue channel on/off |
| 4 | Increments red brightness by one step (wraps 0→255) and enables red channel |
| 5 | Increments green brightness by one step (wraps 0→255) and enables green channel |
| 6 | Increments blue brightness by one step (wraps 0→255) and enables blue channel |
| 7 | Master power toggle — turning ON picks a random colour; turning OFF blanks all LEDs |

## Configuration

Edit the constants at the top of `code.py`:

| Constant | Default | Description |
|----------|---------|-------------|
| `BUTTON_PINS` | D2–D8 | GPIO pins for each button |
| `LED_PIN` | D9 | GPIO pin for LED strip data |
| `NUM_PIXELS` | 30 | Number of LEDs in the strip |
| `INCREMENT_STEP` | 32 | Brightness increment per button press (1–255) |
| `DEBOUNCE_MS` | 50 | Button debounce time in milliseconds |

## Dependencies

Install via [CircuitPython Library Bundle](https://circuitpython.org/libraries):

- `neopixel` (`adafruit_neopixel` / `neopixel.mpy`)

Copy `neopixel.mpy` to the `lib/` folder on your board's `CIRCUITPY` drive.

## Installation

1. Install CircuitPython on your Arduino board (see [circuitpython.org](https://circuitpython.org/downloads))
2. Copy `neopixel.mpy` to `CIRCUITPY/lib/`
3. Copy `code.py` to the root of the `CIRCUITPY` drive
4. The board will restart and run automatically
