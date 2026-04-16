import time
import board
import digitalio
import pwmio

motor = pwmio.PWMOut(board.GP16, frequency = 50)
print("Starting motor control")

while True:
    angle = 0
    while angle < 180:
        duty_cycle = int(((angle / 180)*0.1 + 0.025) * 65535)
        motor.duty_cycle = duty_cycle
        time.sleep(0.01)
        angle += 1
    while angle > 0:
        duty_cycle = int(((angle / 180)*0.1 + 0.025) * 65535)
        motor.duty_cycle = duty_cycle
        time.sleep(0.01)
        angle -= 1