import time
import RPi.GPIO as GPIO
from RPLCD.i2c import CharLCD

# ==========================
# LCD
# ==========================

lcd = CharLCD(
    i2c_expander='PCF8574',
    address=0x27,
    port=1,
    cols=16,
    rows=2,
    dotsize=8,
    charmap='A00',
    auto_linebreaks=False
)

# ==========================
# GPIO
# ==========================

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

ROWS = [5, 6, 13, 19]
COLS = [12, 16, 20, 21]

for r in ROWS:
    GPIO.setup(r, GPIO.OUT)
    GPIO.output(r, GPIO.HIGH)

for c in COLS:
    GPIO.setup(c, GPIO.IN, pull_up_down=GPIO.PUD_UP)

KEYS = [
    ['1','2','3','A'],
    ['4','5','6','B'],
    ['7','8','9','C'],
    ['*','0','#','D']
]

# ==========================
# Leitura teclado
# ==========================

def read_key():

    while True:

        for i, row in enumerate(ROWS):

            GPIO.output(row, GPIO.LOW)

            for j, col in enumerate(COLS):

                if GPIO.input(col) == GPIO.LOW:

                    time.sleep(0.03)

                    while GPIO.input(col) == GPIO.LOW:
                        pass

                    GPIO.output(row, GPIO.HIGH)

                    return KEYS[i][j]

            GPIO.output(row, GPIO.HIGH)

# ==========================
# LCD
# ==========================

def display(expr="", result=""):

    lcd.clear()

    lcd.cursor_pos = (0,0)
    lcd.write_string(expr[:16])

    lcd.cursor_pos = (1,0)
    lcd.write_string(result[:16])

# ==========================
# Operações
# ==========================

def overflow(v):
    return v < 0 or v > 15


def soma(a,b):
    r = a+b
    if overflow(r):
        return None
    return r


def sub(a,b):
    r = a-b
    if overflow(r):
        return None
    return r


def mult(a,b):
    r = a*b
    if overflow(r):
        return None
    return r


def fat(n):

    r = 1

    for i in range(2,n+1):
        r *= i

    if overflow(r):
        return None

    return r

# ==========================
# Estado
# ==========================

num1 = ""
num2 = ""

op = ""

state = "NUM1"

display("Calculadora", "4 bits")
time.sleep(2)

display("", "")

while True:

    key = read_key()

    # -----------------------
    # LIMPAR
    # -----------------------

    if key == "*":

        num1 = ""
        num2 = ""
        op = ""
        state = "NUM1"

        display("", "")

        continue

    # -----------------------
    # PRIMEIRO NUMERO
    # -----------------------

    if state == "NUM1":

        if key.isdigit():

            if len(num1) < 2:

                temp = num1 + key

                if int(temp) <= 15:

                    num1 = temp

                    display(num1, "")
                continue

        elif key == "A" and num1 != "":

            op = "+"
            state = "NUM2"
            display(num1 + op, "")

            continue

        elif key == "B" and num1 != "":

            op = "-"
            state = "NUM2"
            display(num1 + op, "")

            continue

        elif key == "C" and num1 != "":

            op = "*"
            state = "NUM2"
            display(num1 + op, "")

            continue

        elif key == "D" and num1 != "":

            op = "!"
            state = "EXEC"

            display(num1 + "!", "")

            continue

    # -----------------------
    # SEGUNDO NUMERO
    # -----------------------

    elif state == "NUM2":

        if key.isdigit():

            if len(num2) < 2:

                temp = num2 + key

                if int(temp) <= 15:

                    num2 = temp

                    display(num1 + op + num2, "")

            continue

        elif key == "#":

            if num2 == "":
                continue

            a = int(num1)
            b = int(num2)

            if op == "+":
                r = soma(a, b)

            elif op == "-":
                r = sub(a, b)

            elif op == "*":
                r = mult(a, b)

            else:
                r = None

            if r is None:

                display(num1 + op + num2, "OVERFLOW")

            else:

                display(num1 + op + num2, "=" + str(r))

            time.sleep(3)

            num1 = ""
            num2 = ""
            op = ""
            state = "NUM1"

            display("", "")

    # -----------------------
    # FATORIAL
    # -----------------------

    elif state == "EXEC":

        if key == "#":

            a = int(num1)

            r = fat(a)

            if r is None:

                display(num1 + "!", "OVERFLOW")

            else:

                display(num1 + "!", "=" + str(r))

            time.sleep(3)

            num1 = ""
            num2 = ""
            op = ""
            state = "NUM1"

            display("", "")

GPIO.cleanup()