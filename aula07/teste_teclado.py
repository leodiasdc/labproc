import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

ROWS=[5,6,13,19]
COLS=[12,16,20,21]

KEYS=[
['1','2','3','A'],
['4','5','6','B'],
['7','8','9','C'],
['*','0','#','D']
]

for r in ROWS:
    GPIO.setup(r,GPIO.OUT)
    GPIO.output(r,1)

for c in COLS:
    GPIO.setup(c,GPIO.IN,pull_up_down=GPIO.PUD_UP)

while True:
    for i,r in enumerate(ROWS):
        GPIO.output(r,0)
        for j,c in enumerate(COLS):
            if GPIO.input(c)==0:
                print(KEYS[i][j])
                while GPIO.input(c)==0:
                    pass
        GPIO.output(r,1)
    time.sleep(0.02)