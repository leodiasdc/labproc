import threading, time
from gpiozero import Device, PWMLED, AngularServo, Buzzer, Button
from gpiozero.tones import Tone

# Importação e configuração do PiGPIOFactory removidas.
# O gpiozero usará o factory padrão do sistema automaticamente.

led = PWMLED(17, initial_value=0, frequency=1000)
buzzer = Buzzer(12)
myCorrection = 0.0
maxPW = (2.5 + myCorrection) / 1000
minPW = (0.5 - myCorrection) / 1000
servo = AngularServo(18, initial_angle=0, min_angle=0, max_angle=180, min_pulse_width=minPW, max_pulse_width=maxPW)

MIN_BPM, MAX_BPM, BPM_STEP = 30, 240, 5
current_bpm = 60
bpm_lock = threading.Lock()

def get_interval():
    with bpm_lock:
        return 60.0 / current_bpm

def increase_bpm():
    global current_bpm
    with bpm_lock:
        current_bpm = min(MAX_BPM, current_bpm + BPM_STEP)

def decrease_bpm():
    global current_bpm
    with bpm_lock:
        current_bpm = max(MIN_BPM, current_bpm - BPM_STEP)

btn_up = Button(22, bounce_time=0.05, pull_up=True)
btn_down = Button(23, bounce_time=0.05, pull_up=True)
btn_up.when_pressed = increase_bpm
btn_down.when_pressed = decrease_bpm

servo_event = threading.Event()
servo_direction = {"forward": True}
running = True

def servo_worker():
    while running:
        servo_event.wait(timeout=0.5)
        if not running:
            break
        if not servo_event.is_set():
            continue
        servo_event.clear()
        rng = range(0, 181, 2) if servo_direction["forward"] else range(180, -1, -2)
        for angle in rng:
            servo.angle = angle
            time.sleep(0.001)
        servo_direction["forward"] = not servo_direction["forward"]

def beat_worker():
    next_tick = time.monotonic()
    while running:
        now = time.monotonic()
        sleep_time = next_tick - now
        if sleep_time > 0:
            time.sleep(sleep_time)
        else:
            next_tick = now
        buzzer.on()
        led.on()
        time.sleep(0.05)
        buzzer.off()
        led.off()
        servo_event.set()
        next_tick += get_interval()

def destroy():
    led.close()
    servo.close()
    buzzer.close()
    btn_up.close()
    btn_down.close()

if __name__ == "__main__":
    threading.Thread(target=servo_worker, daemon=True).start()
    try:
        beat_worker()
    except KeyboardInterrupt:
        pass
    finally:
        running = False
        servo_event.set()
        destroy()