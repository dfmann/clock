// ESP8266 RGB LED Strip Controller
// Requires ESP8266 Arduino core — Boards Manager URL:
// http://arduino.esp8266.com/stable/package_esp8266com_index.json
//
// GPIO caveats (NodeMCU / Wemos D1 Mini):
//   GPIO 0  (D3) — must be HIGH at boot; do NOT press BTN3 at power-on
//   GPIO 2  (D4) — must be HIGH at boot; do NOT press BTN4 at power-on
//   GPIO 1  (TX) — outputs serial noise at boot; fine once running
//   GPIO 16 (D0) — no internal pull-up; wire 10kΩ from GPIO16 to 3.3V
//
// DEBUG NOTE: Serial uses GPIO1 (TX) and GPIO3 (RX).
//   These are the same pins as BTN6 and BTN5. While DEBUG=1, BTN5 and
//   BTN6 will not function. Set DEBUG to 0 for production use.

#define DEBUG 1   // 1 = serial debug output on, 0 = off (restores BTN5/BTN6)

// --- Pin configuration (GPIO numbers) ---
//                              BTN:  1   2   3   4   5   6   7
const uint8_t BUTTON_PINS[7]       = {5,  4,  0,  2,  3,  1, 16};
const uint8_t R_PIN = 12;  // D6 — PWM → 100Ω → MOSFET gate (red)
const uint8_t G_PIN = 13;  // D7 — PWM → 100Ω → MOSFET gate (green)
const uint8_t B_PIN = 14;  // D5 — PWM → 100Ω → MOSFET gate (blue)

const uint16_t PWM_FREQ      = 1000;  // Hz
const uint8_t  INCREMENT_STEP = 32;   // 8 steps across 0–255
const uint32_t DEBOUNCE_MS   = 50;

bool    powerOn = false;
bool    rActive = true, gActive = true, bActive = true;
uint8_t rVal    = 128, gVal    = 128, bVal    = 128;

bool     prevStates[7];
uint32_t debounceTimes[7];

void refresh() {
    analogWrite(R_PIN, (powerOn && rActive) ? rVal : 0);
    analogWrite(G_PIN, (powerOn && gActive) ? gVal : 0);
    analogWrite(B_PIN, (powerOn && bActive) ? bVal : 0);
}

void printState() {
#if DEBUG
    Serial.printf("  state: power=%s  R=%s(%d)  G=%s(%d)  B=%s(%d)\n",
        powerOn ? "ON" : "OFF",
        rActive ? "on" : "off", rVal,
        gActive ? "on" : "off", gVal,
        bActive ? "on" : "off", bVal);
#endif
}

void handleButton(uint8_t index) {
    switch (index) {
        case 0:
            rActive = !rActive;
#if DEBUG
            Serial.printf("[BTN1] Toggle Red → %s\n", rActive ? "ON" : "OFF");
#endif
            break;
        case 1:
            gActive = !gActive;
#if DEBUG
            Serial.printf("[BTN2] Toggle Green → %s\n", gActive ? "ON" : "OFF");
#endif
            break;
        case 2:
            bActive = !bActive;
#if DEBUG
            Serial.printf("[BTN3] Toggle Blue → %s\n", bActive ? "ON" : "OFF");
#endif
            break;
        case 3:
            rVal = (rVal + INCREMENT_STEP) % 256;
            rActive = true;
#if DEBUG
            Serial.printf("[BTN4] Increment Red → %d\n", rVal);
#endif
            break;
        case 4:
            gVal = (gVal + INCREMENT_STEP) % 256;
            gActive = true;
#if DEBUG
            Serial.printf("[BTN5] Increment Green → %d\n", gVal);
#endif
            break;
        case 5:
            bVal = (bVal + INCREMENT_STEP) % 256;
            bActive = true;
#if DEBUG
            Serial.printf("[BTN6] Increment Blue → %d\n", bVal);
#endif
            break;
        case 6:
            powerOn = !powerOn;
            if (powerOn) {
                rVal = random(8) * INCREMENT_STEP;
                gVal = random(8) * INCREMENT_STEP;
                bVal = random(8) * INCREMENT_STEP;
                rActive = true; gActive = true; bActive = true;
#if DEBUG
                Serial.printf("[BTN7] Power ON → random colour R:%d G:%d B:%d\n",
                    rVal, gVal, bVal);
#endif
            } else {
#if DEBUG
                Serial.println("[BTN7] Power OFF");
#endif
            }
            break;
    }
    printState();
    refresh();
}

void setup() {
#if DEBUG
    Serial.begin(115200);
    delay(100);  // let the serial port settle after boot noise
    Serial.println("\n\n=== ESP8266 RGB LED Controller ===");
    Serial.printf("  Buttons:  GPIO %d(BTN1) %d(BTN2) %d(BTN3) %d(BTN4) %d(BTN5) %d(BTN6) %d(BTN7)\n",
        BUTTON_PINS[0], BUTTON_PINS[1], BUTTON_PINS[2], BUTTON_PINS[3],
        BUTTON_PINS[4], BUTTON_PINS[5], BUTTON_PINS[6]);
    Serial.printf("  PWM pins: R=GPIO%d  G=GPIO%d  B=GPIO%d\n", R_PIN, G_PIN, B_PIN);
    Serial.printf("  PWM freq: %d Hz  step: %d/255  debounce: %dms\n",
        PWM_FREQ, INCREMENT_STEP, DEBOUNCE_MS);
    Serial.println("  WARNING: BTN5(GPIO3) and BTN6(GPIO1) inactive while DEBUG=1");
    Serial.println("==================================\n");
#endif

    randomSeed(RANDOM_REG32);

    analogWriteRange(255);
    analogWriteFreq(PWM_FREQ);

    for (uint8_t i = 0; i < 7; i++) {
        // GPIO16 has no internal pull-up — requires external 10kΩ to 3.3V
        pinMode(BUTTON_PINS[i], BUTTON_PINS[i] == 16 ? INPUT : INPUT_PULLUP);
        prevStates[i]    = HIGH;
        debounceTimes[i] = 0;
    }

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    refresh();

#if DEBUG
    Serial.println("Ready. Waiting for button presses...");
    printState();
#endif
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
