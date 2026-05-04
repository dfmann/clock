import board
import digitalio
import pwmio
import random
import time

# --- Pin configuration: adjust to match your wiring ---
BUTTON_PINS = [
    board.D2,   # Button 1: toggle red channel
    board.D3,   # Button 2: toggle green channel
    board.D4,   # Button 3: toggle blue channel
    board.D5,   # Button 4: increment red brightness
    board.D6,   # Button 5: increment green brightness
    board.D7,   # Button 6: increment blue brightness
    board.D8,   # Button 7: master toggle (on = random colour, off = all off)
]
R_PIN = board.D9    # → 100Ω → MOSFET gate (red channel)
G_PIN = board.D10   # → 100Ω → MOSFET gate (green channel)
B_PIN = board.D11   # → 100Ω → MOSFET gate (blue channel)

PWM_FREQ     = 1000   # Hz — increase if visible flicker on camera
INCREMENT_STEP = 8192  # 65535 / 8 steps across full brightness range
DEBOUNCE_MS  = 50

r_pwm = pwmio.PWMOut(R_PIN, frequency=PWM_FREQ, duty_cycle=0)
g_pwm = pwmio.PWMOut(G_PIN, frequency=PWM_FREQ, duty_cycle=0)
b_pwm = pwmio.PWMOut(B_PIN, frequency=PWM_FREQ, duty_cycle=0)

buttons = []
for pin in BUTTON_PINS:
    btn = digitalio.DigitalInOut(pin)
    btn.direction = digitalio.Direction.INPUT
    btn.pull = digitalio.Pull.UP
    buttons.append(btn)

power_on = False
r_active  = True
g_active  = True
b_active  = True
r_val = 32768   # start at 50% brightness
g_val = 32768
b_val = 32768

prev_states    = [True] * len(buttons)
debounce_times = [0]   * len(buttons)


def refresh():
    r_pwm.duty_cycle = r_val if (power_on and r_active) else 0
    g_pwm.duty_cycle = g_val if (power_on and g_active) else 0
    b_pwm.duty_cycle = b_val if (power_on and b_active) else 0


def handle_button(index):
    global power_on, r_active, g_active, b_active, r_val, g_val, b_val

    if index == 0:
        r_active = not r_active
    elif index == 1:
        g_active = not g_active
    elif index == 2:
        b_active = not b_active
    elif index == 3:
        r_val = (r_val + INCREMENT_STEP) % 65536
        r_active = True
    elif index == 4:
        g_val = (g_val + INCREMENT_STEP) % 65536
        g_active = True
    elif index == 5:
        b_val = (b_val + INCREMENT_STEP) % 65536
        b_active = True
    elif index == 6:
        power_on = not power_on
        if power_on:
            r_val = random.randint(0, 7) * INCREMENT_STEP
            g_val = random.randint(0, 7) * INCREMENT_STEP
            b_val = random.randint(0, 7) * INCREMENT_STEP
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
