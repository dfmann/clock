// ESP8266 RGB LED Strip Controller
// Requires ESP8266 Arduino core — Boards Manager URL:
// http://arduino.esp8266.com/stable/package_esp8266com_index.json
//
// GPIO caveats (NodeMCU / Wemos D1 Mini):
//   GPIO 0  (D3) — must be HIGH at boot; do NOT press BTN3 at power-on
//   GPIO 2  (D4) — must be HIGH at boot; do NOT press BTN4 at power-on
//   GPIO 1  (TX) — outputs serial noise at boot; fine once running
//   GPIO 16 (D0) — no internal pull-up; wire 10kΩ from GPIO16 to 3.3V

// --- Pin configuration (GPIO numbers) ---
//                              BTN:  1   2   3   4   5   6   7
const uint8_t BUTTON_PINS[7]       = {5,  4,  0,  2,  3,  1, 16};
const uint8_t R_PIN = 12;  // D6 — PWM → 100Ω → MOSFET gate (red)
const uint8_t G_PIN = 13;  // D7 — PWM → 100Ω → MOSFET gate (green)
const uint8_t B_PIN = 14;  // D5 — PWM → 100Ω → MOSFET gate (blue)

const uint16_t PWM_FREQ      = 1000;  // Hz — raise if camera flicker seen
const uint8_t  INCREMENT_STEP = 32;   // 8 steps across 0–255
const uint32_t DEBOUNCE_MS   = 50;

bool    powerOn = true;
bool    rActive = true, gActive = true, bActive = true;
uint8_t rVal    = 128, gVal    = 128, bVal    = 128;

bool     prevStates[7];
uint32_t debounceTimes[7];

void refresh() {
    analogWrite(R_PIN, (powerOn && rActive) ? rVal : 0);
    analogWrite(G_PIN, (powerOn && gActive) ? gVal : 0);
    analogWrite(B_PIN, (powerOn && bActive) ? bVal : 0);
}

void handleButton(uint8_t index) {
    switch (index) {
        case 0: rActive = !rActive; break;
        case 1: gActive = !gActive; break;
        case 2: bActive = !bActive; break;
        case 3: rVal = (rVal + INCREMENT_STEP) % 256; rActive = true; break;
        case 4: gVal = (gVal + INCREMENT_STEP) % 256; gActive = true; break;
        case 5: bVal = (bVal + INCREMENT_STEP) % 256; bActive = true; break;
        case 6:
            powerOn = !powerOn;
            if (powerOn) {
                rVal = random(8) * INCREMENT_STEP;
                gVal = random(8) * INCREMENT_STEP;
                bVal = random(8) * INCREMENT_STEP;
                rActive = true; gActive = true; bActive = true;
            }
            break;
    }
    refresh();
}

void setup() {
    randomSeed(RANDOM_REG32);  // ESP8266 hardware RNG register

    analogWriteRange(255);     // use 8-bit PWM range (0–255)
    analogWriteFreq(PWM_FREQ);

    for (uint8_t i = 0; i < 7; i++) {
        // GPIO16 has no internal pull-up — all others use INPUT_PULLUP
        // GPIO16 button needs external 10kΩ resistor from pin to 3.3V
        pinMode(BUTTON_PINS[i], BUTTON_PINS[i] == 16 ? INPUT : INPUT_PULLUP);
        prevStates[i]    = HIGH;
        debounceTimes[i] = 0;
    }

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    refresh();
}

void loop() {
    uint32_t now = millis();

    for (uint8_t i = 0; i < 7; i++) {
        bool state = digitalRead(BUTTON_PINS[i]);
        if (state != prevStates[i] && (now - debounceTimes[i]) >= DEBOUNCE_MS) {
            debounceTimes[i] = now;
            if (state == LOW) {   // falling edge — button pressed
                handleButton(i);
            }
        }
        prevStates[i] = state;
    }
}
