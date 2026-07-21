
import time
import warnings
from gpiozero import Buzzer, DistanceSensor
from LCD1602 import CharLCD1602
import Keypad

warnings.filterwarnings("ignore")   # o DistanceSensor pode emitir avisos
                                     # quando a leitura sai fora da faixa

# ---------------------------------------------------------------------
# Configuracao geral
# ---------------------------------------------------------------------
PASSWORD = "1234"
MAX_DIGITS = 8

ROWS = 4
COLS = 4
KEYS = [
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
]
ROW_PINS = [16, 20, 21, 26]   # pinos das linhas do teclado
COL_PINS = [19, 13, 6, 5]     # pinos das colunas do teclado

BUZZER_PIN = 12               # mesmo pino do Doorbell.py
US_TRIGGER_PIN = 14           # Trigger do HC-SR04 (igual ao UltrasonicRanging.py)
US_ECHO_PIN = 15              # Echo do HC-SR04 (com divisor de tensao!)

DOOR_CLOSED_THRESHOLD_M = 0.05   # 5 cm: abaixo disso, consideramos "fechada"
US_MAX_DISTANCE_M = 3            # mesmo valor usado no UltrasonicRanging.py

# ---------------------------------------------------------------------
# Objetos de hardware
# ---------------------------------------------------------------------
lcd1602 = CharLCD1602()
keypad = Keypad.Keypad(KEYS, ROW_PINS, COL_PINS, ROWS, COLS)
keypad.setDebounceTime(50)

buzzer = Buzzer(BUZZER_PIN)

door_sensor = DistanceSensor(
    echo=US_ECHO_PIN,
    trigger=US_TRIGGER_PIN,
    max_distance=US_MAX_DISTANCE_M,
    threshold_distance=DOOR_CLOSED_THRESHOLD_M,
)


def door_is_closed():
    return door_sensor.distance < DOOR_CLOSED_THRESHOLD_M


# ---------------------------------------------------------------------
# Feedback sonoro
# ---------------------------------------------------------------------
def beep(duration=0.05):
    buzzer.on()
    time.sleep(duration)
    buzzer.off()


def beep_key():
    beep(0.04)


def beep_success():
    beep(0.08)
    time.sleep(0.05)
    beep(0.08)


def beep_error():
    beep(0.5)


def beep_alarm_pulse():
    beep(0.15)
    time.sleep(0.1)


# ---------------------------------------------------------------------
# Estados da fechadura
# ---------------------------------------------------------------------
LOCKED = 'LOCKED'
UNLOCKED = 'UNLOCKED'
ALARM = 'ALARM'

state = LOCKED
entered = ""


def reset_entry():
    global entered
    entered = ""


# ---------------------------------------------------------------------
# Telas do LCD
# ---------------------------------------------------------------------
def show_locked_screen():
    lcd1602.clear()
    lcd1602.write(0, 0, '  TRANCADO')
    sensor_txt = 'Porta:Fechada' if door_is_closed() else 'Porta:ABERTA'
    lcd1602.write(0, 1, sensor_txt[:16])


def show_unlocked_screen():
    lcd1602.clear()
    lcd1602.write(0, 0, '  ABERTO !')
    lcd1602.write(0, 1, '# p/ trancar')


def show_error_screen():
    lcd1602.clear()
    lcd1602.write(0, 0, ' SENHA ERRADA')
    lcd1602.write(0, 1, ' Tente de novo')
    time.sleep(1.2)


def show_locked_denied_screen():
    # tentativa de trancar com a porta fisicamente aberta
    lcd1602.clear()
    lcd1602.write(0, 0, 'Feche a porta')
    lcd1602.write(0, 1, 'p/ trancar')
    time.sleep(1.2)


def show_alarm_screen():
    lcd1602.clear()
    lcd1602.write(0, 0, '!! ALARME !!')
    lcd1602.write(0, 1, 'Porta violada')


def show_entry_progress():
    lcd1602.write(0, 1, 'Senha:' + '*' * len(entered) + ' ' * (10 - len(entered)))


# ---------------------------------------------------------------------
# Logica de teclas por estado
# ---------------------------------------------------------------------
def handle_locked_key(key):
    global state

    if key == '#':
        beep_key()
        if entered == PASSWORD:
            beep_success()
            state = UNLOCKED
            reset_entry()
            show_unlocked_screen()
        else:
            beep_error()
            show_error_screen()
            reset_entry()
            show_locked_screen()
    elif key == '*':
        beep_key()
        reset_entry()
        show_locked_screen()
    elif key in '0123456789':
        beep_key()
        if len(entered) < MAX_DIGITS:
            entered += key
        show_entry_progress()


def handle_unlocked_key(key):
    global state
    if key == '#':
        beep_key()
        if door_is_closed():
            state = LOCKED
            reset_entry()
            show_locked_screen()
        else:
            show_locked_denied_screen()
            show_unlocked_screen()


def handle_alarm_key(key):
    global state

    if key == '#':
        beep_key()
        if entered == PASSWORD:
            beep_success()
            state = LOCKED
            reset_entry()
            show_locked_screen()
        else:
            beep_error()
            reset_entry()
            show_alarm_screen()
    elif key == '*':
        beep_key()
        reset_entry()
    elif key in '0123456789':
        beep_key()
        if len(entered) < MAX_DIGITS:
            entered += key


# ---------------------------------------------------------------------
# Loop principal
# ---------------------------------------------------------------------
def loop():
    global state

    lcd1602.init_lcd()
    show_locked_screen()

    while True:
        key = keypad.getKey()
        if key != keypad.NULL:
            print("Tecla pressionada: %c" % key)
            if state == LOCKED:
                handle_locked_key(key)
            elif state == UNLOCKED:
                handle_unlocked_key(key)
            elif state == ALARM:
                handle_alarm_key(key)

        if state == LOCKED and not door_is_closed():
            state = ALARM
            reset_entry()
            show_alarm_screen()

        if state == ALARM:
            beep_alarm_pulse()

        time.sleep(0.05)


def destroy():
    lcd1602.clear()
    buzzer.close()
    door_sensor.close()   


if __name__ == '__main__':
    print('Program is starting ... ')
    try:
        loop()
    except KeyboardInterrupt:
        print("Ending program")
    finally:
        destroy()