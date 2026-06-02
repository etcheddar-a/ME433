import matplotlib.pyplot as plt
import serial
import numpy as np

ser = serial.Serial('/dev/tty.usbmodem1102', 115200, timeout=1)
print('Opening port:', ser.name)

print('starting itest')

index= np.zeros(400)
desired_current = np.zeros(400)
actual_current = np.zeros(400)

ser.write(b'a')  # trigger STM

i = 0

while i < 400:
    line = ser.readline().decode(errors='ignore').strip()

    parts = line.split()

    # must be exactly: index desired actual
    if len(parts) != 3:
        continue

    try:
        index[i] = int(parts[0])
        desired_current[i] = float(parts[1]) /1000
        actual_current[i] = float(parts[2]) /1000
        i += 1
    except:
        continue

plt.plot(index, desired_current, label='Desired Current')
plt.plot(index, actual_current, label='Actual Current')
plt.xlabel('Index')
plt.ylabel('Current (mA)')
plt.title('Current Control Test')
plt.legend()
plt.grid()
plt.show()