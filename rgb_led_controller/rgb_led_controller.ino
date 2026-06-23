// Arduino Nano RGB LED Strip Controller
//
// PWM outputs use Timer1 (pins 9, 10) and Timer2 (pin 11) — default ~490 Hz.
// All button pins support INPUT_PULLUP; no external pull-up resistors needed.
// Serial TX/RX are pins 0/1 — not used for buttons, so DEBUG=1 is always safe.

#define DEBUG 1   // 1 = serial debug output on, 0 = off

// --- Pin configuration ---
//                              BTN:  1   2   3   4    5    6    7
const uint8_t BUTTON_PINS[7]       = {2,  4,  7,  8,  A1,  A2,  A3};
const uint8_t R_PIN =  9;  // OC1A — PWM → 100Ω → MOSFET gate (red)
const uint8_t G_PIN = 10;  // OC1B — PWM → 100Ω → MOSFET gate (green)
const uint8_t B_PIN = 11;  // OC2A — PWM → 100Ω → MOSFET gate (blue)

const uint8_t  INCREMENT_STEP = 32;   // 8 steps across 0–255
const uint32_t DEBOUNCE_MS   = 50;
const uint8_t DEFAULT_BRIGHTNESS = 128;

bool    powerOn = true;
bool    rActive = true, gActive = true, bActive = true;
uint8_t rVal    = DEFAULT_BRIGHTNESS, gVal    = DEFAULT_BRIGHTNESS, bVal    = DEFAULT_BRIGHTNESS;

bool     prevStates[7];
uint32_t debounceTimes[7];

void refresh() {
    analogWrite(R_PIN, (powerOn && rActive) ? rVal : 0);
    analogWrite(G_PIN, (powerOn && gActive) ? gVal : 0);
    analogWrite(B_PIN, (powerOn && bActive) ? bVal : 0);
}

void printState() {
#if DEBUG
    char buf[68];
    snprintf(buf, sizeof(buf), "  state: power=%s  R=%s(%d)  G=%s(%d)  B=%s(%d)",
        powerOn ? "ON" : "OFF",
        rActive ? "on" : "off", rVal,
        gActive ? "on" : "off", gVal,
        bActive ? "on" : "off", bVal);
    Serial.println(buf);
#endif
}

void handleButton(uint8_t index) {
    switch (index) {
        case 0:
            rActive = !rActive;
#if DEBUG
            Serial.print(F("[BTN1] Toggle Red -> "));
            Serial.println(rActive ? F("ON") : F("OFF"));
#endif
            if (rActive) {
                rVal = DEFAULT_BRIGHTNESS;
                if (!powerOn) {
                    gVal = 0;
                    bVal = 0;
                }
                powerOn = true;
            }
            break;
        case 1:
            gActive = !gActive;
#if DEBUG
            Serial.print(F("[BTN2] Toggle Green -> "));
            Serial.println(gActive ? F("ON") : F("OFF"));
#endif
            if (gActive) {
                gVal = DEFAULT_BRIGHTNESS;
                if (!powerOn) {
                    bVal = 0;
                    rVal = 0;
                }
                powerOn = true;
            }
            break;
        case 2:
            bActive = !bActive;
#if DEBUG
            Serial.print(F("[BTN3] Toggle Blue -> "));
            Serial.println(bActive ? F("ON") : F("OFF"));
#endif
            if (bActive) {
                bVal = DEFAULT_BRIGHTNESS;
                if (!powerOn) {
                    gVal = 0;
                    rVal = 0;
                }
                powerOn = true;
            }
            break;
        case 3:
            rVal = (rVal + INCREMENT_STEP) % 256;
            rActive = (rVal != 0);
            if (!powerOn) {
                gVal = 0;
                bVal = 0;
            }
            powerOn = true;
#if DEBUG
            Serial.print(F("[BTN4] Increment Red -> "));
            Serial.println(rVal);
#endif
            break;
        case 4:
            gVal = (gVal + INCREMENT_STEP) % 256;
            gActive = (gVal != 0);
            if (!powerOn) {
                rVal = 0;
                bVal = 0;
            }
            powerOn = true;
#if DEBUG
            Serial.print(F("[BTN5] Increment Green -> "));
            Serial.println(gVal);
#endif
            break;
        case 5:
            bVal = (bVal + INCREMENT_STEP) % 256;
            bActive = (bVal != 0);
            if (!powerOn) {
                gVal = 0;
                rVal = 0;
            }
            powerOn = true;
#if DEBUG
            Serial.print(F("[BTN6] Increment Blue -> "));
            Serial.println(bVal);
#endif
            break;
        case 6:
            powerOn = !powerOn;
            if (powerOn) {
                //rVal = random(8) * INCREMENT_STEP;
                //gVal = random(8) * INCREMENT_STEP;
                //bVal = random(8) * INCREMENT_STEP;
                rVal = DEFAULT_BRIGHTNESS;
                gVal = DEFAULT_BRIGHTNESS;
                bVal = DEFAULT_BRIGHTNESS;
                rActive = true; gActive = true; bActive = true;
#if DEBUG
                char buf[52];
                snprintf(buf, sizeof(buf), "[BTN7] Power ON -> random colour R:%d G:%d B:%d",
                    rVal, gVal, bVal);
                Serial.println(buf);
#endif
            } else {
#if DEBUG
                Serial.println(F("[BTN7] Power OFF"));
#endif
                rActive = false;
                gActive = false;
                bActive = false;
            }
            break;
    }
    printState();
    refresh();
}

void setup() {
#if DEBUG
    Serial.begin(115200);
    Serial.println(F("\n\n=== Arduino Nano RGB LED Controller ==="));
    Serial.print(F("  Buttons: "));
    for (uint8_t i = 0; i < 7; i++) {
        Serial.print(F("BTN")); Serial.print(i + 1);
        Serial.print('='); Serial.print(BUTTON_PINS[i]);
        if (i < 6) Serial.print(' ');
    }
    Serial.println();
    char buf[56];
    snprintf(buf, sizeof(buf), "  PWM: R=%d  G=%d  B=%d  step=%d/255  debounce=%lums",
        R_PIN, G_PIN, B_PIN, INCREMENT_STEP, DEBOUNCE_MS);
    Serial.println(buf);
    Serial.println(F("=======================================\n"));
#endif

    randomSeed(analogRead(A0));  // floating pin for entropy

    for (uint8_t i = 0; i < 7; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
        prevStates[i]    = HIGH;
        debounceTimes[i] = 0;
    }

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    refresh();

#if DEBUG
    Serial.println(F("Ready. Waiting for button presses..."));
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
