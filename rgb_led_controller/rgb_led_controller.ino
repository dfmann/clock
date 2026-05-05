// ESP32 RGB LED Strip Controller
// Requires ESP32 Arduino core v3.x
// Boards Manager URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

// --- Pin configuration (GPIO numbers) ---
const uint8_t BUTTON_PINS[7] = {4, 5, 16, 17, 18, 19, 21};
const uint8_t R_PIN = 25;   // PWM → 100Ω → MOSFET gate (red channel)
const uint8_t G_PIN = 26;   // PWM → 100Ω → MOSFET gate (green channel)
const uint8_t B_PIN = 27;   // PWM → 100Ω → MOSFET gate (blue channel)

const uint32_t PWM_FREQ      = 1000;  // Hz — raise to 5000+ if camera flicker is an issue
const uint8_t  PWM_BITS      = 8;     // 8-bit resolution: duty cycle 0–255
const uint8_t  INCREMENT_STEP = 32;   // 8 steps across 0–255
const uint32_t DEBOUNCE_MS   = 50;

bool    powerOn = false;
bool    rActive = true, gActive = true, bActive = true;
uint8_t rVal    = 128,  gVal    = 128,  bVal    = 128;

bool     prevStates[7];
uint32_t debounceTimes[7];

void refresh() {
    ledcWrite(R_PIN, (powerOn && rActive) ? rVal : 0);
    ledcWrite(G_PIN, (powerOn && gActive) ? gVal : 0);
    ledcWrite(B_PIN, (powerOn && bActive) ? bVal : 0);
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
    randomSeed(esp_random());  // ESP32 hardware RNG — better entropy than analogRead

    for (uint8_t i = 0; i < 7; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
        prevStates[i]    = HIGH;
        debounceTimes[i] = 0;
    }

    // LEDC is the ESP32 PWM peripheral — ledcAttach replaces pinMode+analogWrite
    ledcAttach(R_PIN, PWM_FREQ, PWM_BITS);
    ledcAttach(G_PIN, PWM_FREQ, PWM_BITS);
    ledcAttach(B_PIN, PWM_FREQ, PWM_BITS);

    refresh();
}

void loop() {
    uint32_t now = millis();

    for (uint8_t i = 0; i < 7; i++) {
        bool state = digitalRead(BUTTON_PINS[i]);
        if (state != prevStates[i] && (now - debounceTimes[i]) >= DEBOUNCE_MS) {
            debounceTimes[i] = now;
            if (state == LOW) {   // falling edge — button pressed (pull-up active)
                handleButton(i);
            }
        }
        prevStates[i] = state;
    }
}
