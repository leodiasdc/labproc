

from gpiozero import PWMLED, AngularServo, Buzzer, Button
from gpiozero.tones import Tone
import time 


led = PWMLED(17 ,initial_value=0 ,frequency=1000)
buzzer = Buzzer(12)
myGPIO=18

SERVO_DELAY_SEC = 0.001 
myCorrection=0.0
maxPW=(2.5+myCorrection)/1000
minPW=(0.5-myCorrection)/1000
servo =  AngularServo(myGPIO,initial_angle=0,min_angle=0, max_angle=180,min_pulse_width=minPW,max_pulse_width=maxPW)

def loop():
    while True: 
        initial_time = time.time()
        buzzer.on()
        led.on()
        time.sleep(0.1)
        buzzer.stop()
        for angle in range(0, 181, 1):   # make servo rotate from 0 to 180 deg
            servo.angle = angle
            time.sleep(SERVO_DELAY_SEC)
        drift_time = time.time() - initial_time
        time.sleep(max(0, 1 - drift_time))
        initial_time = time.time() 
        buzzer.off()
        led.off()
        time.sleep(0.1)
        buzzer.stop()
        for angle in range(180, -1, -1): # make servo rotate from 180 to 0 deg
            servo.angle = angle
            time.sleep(SERVO_DELAY_SEC)
        drift_time = time.time() - initial_time
        time.sleep(max(0, 1 - drift_time))

def destroy():
    led.close()
    servo.close()
    buzzer.close()


if __name__ == '__main__':     # Program entrance
    print ('Program is starting...')
    try:
        loop()
    except KeyboardInterrupt:  # Press ctrl-c to end the program.
        print("Ending program")
    finally:
        destroy()