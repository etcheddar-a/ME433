import numpy as np
import matplotlib.pyplot as plt
import serial

# open serial port
ser = serial.Serial('/dev/tty.usbmodem2101')
print('Opening port: ' + ser.name)

# read in number of samples
n_samp = int(input('Enter number of samples: '))

# print in serial to Pico
ser.write((str(n_samp) + '\n').encode())

# read in data
data = np.zeros(n_samp)
raw_data = np.zeros(n_samp)
t = np.zeros(n_samp)

for i in range(n_samp):
    n_str = ser.read_until(b'\n')
    n_list = list(map(int, n_str.split()))
    t[i] = n_list[0]
    data[i] = n_list[1]
    raw_data[i] = n_list[2]

# start t at zero
t = t - t[0]

# take FFT of both datasets, nyquist frequency is 40 Hz
# FFT for original dataset
Fs = 80  # sample rate
n = len(raw_data)  # length of the signal
k = np.arange(n)
T = n / Fs
frq = k / T  # two sides frequency range
frq = frq[range(int(n / 2))]  # one side frequency range
Y = np.fft.fft(raw_data) / n  # fft computing and normalization
Y = Y[range(int(n / 2))]

# FFT for IIR filtered dataset
n_filtered = len(data)  # length of the signal
k_filtered = np.arange(n_filtered)
T_filtered = n_filtered / Fs
frq_filtered = k_filtered / T_filtered  # two sides frequency range
frq_filtered = frq_filtered[range(int(n_filtered / 2))]  # one side frequency range
Y_filtered = np.fft.fft(data) / n_filtered
Y_filtered = Y_filtered[range(int(n_filtered / 2))]

# plot raw vs filtered data vs time
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t, raw_data, 'k', label='Raw Data')
ax1.plot(t, data, 'r', label='IIR Filtered Data')
ax1.set_xlabel('Time (ms)')
ax1.set_ylabel('HX711 Reading')
ax1.set_title('HX711 Data')
ax1.legend()
plt.grid()

# plot FFT of raw vs filtered data
ax2.loglog(frq, abs(Y), 'k', label='Raw Data')
ax2.loglog(frq_filtered, abs(Y_filtered), 'r', label='IIR Filtered Data')
ax2.set_title('FFT of HX711 Data')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
ax2.legend()

plt.tight_layout()
plt.show()