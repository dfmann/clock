import board
import digitalio
import neopixel
import random
import time

# --- Pin configuration: adjust to match your wiring ---
BUTTON_PINS = [
    board.D2,   # Button 1: toggle red channel
    board.D3,   # Button 2: toggle green channel
    board.D4,   # Button 3: toggle blue channel
    board.D5,   # Button 4: increment red value
    board.D6,   # Button 5: increment green value
    board.D7,   # Button 6: increment blue value
    board.D8,   # Button 7: master toggle (on = random colour, off = all off)
]
LED_PIN = board.D9
NUM_PIXELS = 30
INCREMENT_STEP = 32
DEBOUNCE_MS = 50

pixels = neopixel.NeoPixel(LED_PIN, NUM_PIXELS, brightness=0.5, auto_write=False)

buttons = []
for pin in BUTTON_PINS:
    btn = digitalio.DigitalInOut(pin)
    btn.direction = digitalio.Direction.INPUT
    btn.pull = digitalio.Pull.UP
    buttons.append(btn)

power_on = False
r_active = True
g_active = True
b_active = True
r_val = 128
g_val = 128
b_val = 128

prev_states = [True] * len(buttons)
debounce_times = [0] * len(buttons)


def current_color():
    return (
        r_val if r_active else 0,
        g_val if g_active else 0,
        b_val if b_active else 0,
    )


def refresh():
    pixels.fill(current_color() if power_on else (0, 0, 0))
    pixels.show()


def handle_button(index):
    global power_on, r_active, g_active, b_active, r_val, g_val, b_val

    if index == 0:
        r_active = not r_active
    elif index == 1:
        g_active = not g_active
    elif index == 2:
        b_active = not b_active
    elif index == 3:
        r_val = (r_val + INCREMENT_STEP) % 256
        r_active = True
    elif index == 4:
        g_val = (g_val + INCREMENT_STEP) % 256
        g_active = True
    elif index == 5:
        b_val = (b_val + INCREMENT_STEP) % 256
        b_active = True
    elif index == 6:
        power_on = not power_on
        if power_on:
            r_val = random.randint(0, 255)
            g_val = random.randint(0, 255)
            b_val = random.randint(0, 255)
            r_active = True
            g_active = True
            b_active = True

    refresh()


refresh()

while True:
    now = time.monotonic_ns() // 1_000_000  # ms

    for i, btn in enumerate(buttons):
        state = btn.value  # True = not pressed (pull-up)
        if state != prev_states[i] and (now - debounce_times[i]) >= DEBOUNCE_MS:
            debounce_times[i] = now
            if not state:  # falling edge = button pressed
                handle_button(i)
        prev_states[i] = state
