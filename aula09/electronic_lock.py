import time
import warnings
from gpiozero import Buzzer, DistanceSensor
from LCD1602 import CharLCD1602
import Keypad

warnings.filterwarnings("ignore")

# ---------------------------------------------------------------------
# Configurações Gerais
# ---------------------------------------------------------------------
PASSWORD = "1234"
MAX_DIGITS = 8

KEYS = [
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
]
ROW_PINS, COL_PINS = [16, 20, 21, 26], [19, 13, 6, 5]

BUZZER_PIN = 12
US_TRIGGER_PIN, US_ECHO_PIN = 14, 15
DOOR_CLOSED_THRESHOLD_M = 0.05

# ---------------------------------------------------------------------
# Hardware & Variáveis de Estado
# ---------------------------------------------------------------------
lcd = CharLCD1602()
keypad = Keypad.Keypad(KEYS, ROW_PINS, COL_PINS, 4, 4)
keypad.setDebounceTime(50)

buzzer = Buzzer(BUZZER_PIN)
door_sensor = DistanceSensor(
    echo=US_ECHO_PIN, 
    trigger=US_TRIGGER_PIN, 
    max_distance=3, 
    threshold_distance=DOOR_CLOSED_THRESHOLD_M
)

state = 'LOCKED'
entered = ""

# ---------------------------------------------------------------------
# Funções Auxiliares
# ---------------------------------------------------------------------
def door_is_closed():
    return door_sensor.distance < DOOR_CLOSED_THRESHOLD_M

def beep(duration=0.05):
    buzzer.on()
    time.sleep(duration)
    buzzer.off()

def show_msg(line1, line2=""):
    """Função centralizada para atualizar a tela."""
    lcd.clear()
    lcd.write(0, 0, line1[:16])
    lcd.write(0, 1, line2[:16])

def update_screen():
    """Atualiza o LCD conforme o estado atual."""
    if state == 'LOCKED':
        show_msg('  TRANCADO', 'Senha:')
    elif state == 'UNLOCKED':
        show_msg('  ABERTO !', '# p/ trancar')
    elif state == 'ALARM':
        show_msg('!! ALARME !!', 'Porta violada')

# ---------------------------------------------------------------------
# Processamento de Teclas
# ---------------------------------------------------------------------
def handle_key(key):
    global state, entered
    beep(0.04)

    # Adiciona dígitos
    if key in '0123456789':
        if len(entered) < MAX_DIGITS:
            entered += key
        if state != 'UNLOCKED':
            lcd.write(0, 1, ('Senha:' + '*' * len(entered))[:16])

    # Limpa digitação
    elif key == '*':
        entered = ""
        update_screen()

    # Confirma / Ação
    elif key == '#':
        if state in ('LOCKED', 'ALARM'):
            if entered == PASSWORD:
                beep(0.08)
                state = 'UNLOCKED'  # Vai para UNLOCKED para não reativar o alarme se a porta estiver aberta
                entered = ""
                update_screen()
            else:
                beep(0.5)
                entered = ""
                show_msg(' SENHA ERRADA', ' Tente de novo')
                time.sleep(1.2)
                update_screen()

        elif state == 'UNLOCKED':
            if door_is_closed():
                state = 'LOCKED'
                entered = ""
                update_screen()
            else:
                show_msg('Feche a porta', 'p/ trancar')
                time.sleep(1.2)
                update_screen()

# ---------------------------------------------------------------------
# Loop Principal
# ---------------------------------------------------------------------
def loop():
    global state, entered
    lcd.init_lcd()
    update_screen()

    while True:
        key = keypad.getKey()
        if key != keypad.NULL:
            handle_key(key)

        # Disparo do alarme
        if state == 'LOCKED' and not door_is_closed():
            state = 'ALARM'
            entered = ""
            update_screen()

        # Bip contínuo durante alarme
        if state == 'ALARM':
            beep(0.15)
            time.sleep(0.1)

        time.sleep(0.05)

def destroy():
    lcd.clear()
    buzzer.close()
    door_sensor.close()

if __name__ == '__main__':
    try:
        loop()
    except KeyboardInterrupt:
        pass
    finally:
        destroy()