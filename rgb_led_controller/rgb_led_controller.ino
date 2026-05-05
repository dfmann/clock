// Pin assignments — all match the circuit diagram
const uint8_t BUTTON_PINS[7] = {2, 3, 4, 5, 6, 7, 8};
const uint8_t R_PIN = 9;   // PWM → 100Ω → MOSFET gate (red channel)
const uint8_t G_PIN = 10;  // PWM → 100Ω → MOSFET gate (green channel)
const uint8_t B_PIN = 11;  // PWM → 100Ω → MOSFET gate (blue channel)

const uint8_t  INCREMENT_STEP = 32;   // 8 steps across 0–255
const uint32_t DEBOUNCE_MS    = 50;

bool    powerOn = false;
bool    rActive = true, gActive = true, bActive = true;
uint8_t rVal    = 128,  gVal    = 128,  bVal    = 128;

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
    // Seed RNG from a floating analog pin before any reads
    randomSeed(analogRead(A0));

    for (uint8_t i = 0; i < 7; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
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
            if (state == LOW) {   // falling edge — button pressed (pull-up active)
                handleButton(i);
            }
        }
        prevStates[i] = state;
    }
}
