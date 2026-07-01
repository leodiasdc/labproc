#!/usr/bin/env python3
from LCD1602 import CharLCD1602
import Keypad

ROWS=4
COLS=4
keys=['1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D']
rowsPins=[16,20,21,26]
colsPins=[19,13,6,5]

lcd=CharLCD1602()
lcd.init_lcd()
keypad=Keypad.Keypad(keys,rowsPins,colsPins,ROWS,COLS)
keypad.setDebounceTime(50)

num1=""
num2=""
op=""
state="NUM1"

def show():
    lcd.clear()
    if state=="NUM1":
        lcd.write(0,0,num1)
    elif state=="NUM2":
        lcd.write(0,0,num1+op+num2)
    else:
        lcd.write(0,0,num1+"!")

def calc():
    global num1,num2,op,state
    try:
        a=int(num1)
    except:
        return
    if op=="!":
        r=1
        for i in range(2,a+1): r*=i
    else:
        b=int(num2)
        if op=="+": r=a+b
        elif op=="-": r=a-b
        elif op=="*": r=a*b
        else: return
    lcd.clear()
    expr=num1+op+("" if op=="!" else num2)
    lcd.write(0,0,expr)
    if r<0 or r>15:
        lcd.write(0,1,"OVERFLOW")
    else:
        lcd.write(0,1,"="+str(r))
    import time
    time.sleep(3)
    num1=num2=op=""
    state="NUM1"
    lcd.clear()

while True:
    k=keypad.getKey()
    if k==keypad.NULL:
        continue
    if k=="*":
        num1=num2=op=""
        state="NUM1"
        lcd.clear()
        continue
    if state=="NUM1":
        if k.isdigit():
            t=num1+k
            if int(t)<=15:
                num1=t
                show()
        elif k in "ABC" and num1:
            op={ "A":"+","B":"-","C":"*" }[k]
            state="NUM2"
            show()
        elif k=="D" and num1:
            op="!"
            state="FACT"
            show()
    elif state=="NUM2":
        if k.isdigit():
            t=num2+k
            if int(t)<=15:
                num2=t
                show()
        elif k=="#" and num2:
            calc()
    elif state=="FACT":
        if k=="#":
            calc()
