import csv
import matplotlib.pyplot as plt
import numpy as np

# Import data for each CSV file
csv_files = ['sigA.csv', 'sigB.csv', 'sigC.csv', 'sigD.csv']
t_A = []
t_B = []
t_C = []
t_D = []
data_A = []
data_B = []
data_C = []
data_D = []

for csv_file in csv_files:
    t = []
    data = []
    with open(csv_file) as f:
        reader = csv.reader(f)
        for row in reader:
            t.append(float(row[0]))
            data.append(float(row[1]))
    if csv_file == 'sigA.csv':
        t_A = t
        data_A = data
    elif csv_file == 'sigB.csv':
        t_B = t
        data_B = data
    elif csv_file == 'sigC.csv':
        t_C = t
        data_C = data
    elif csv_file == 'sigD.csv':
        t_D = t
        data_D = data

# find sample rates 
sample_rate_A = len(t_A) / t_A[-1]
sample_rate_B = len(t_B) / t_B[-1]
sample_rate_C = len(t_C) / t_C[-1]
sample_rate_D = len(t_D) / t_D[-1]

# select data
data = data_D
t = t_D

# filter data with IIR
A = 0.99
B = 1 - A
filtered = np.zeros(len(data))
filtered[0] = data[0]  # initialize first value of filtered signal
for i in range(1,len(data)):
    filtered[i] = A*filtered[i-1] + B*data[i]

# FFT for original dataset
Fs = 10000  # sample rate
n = len(data)  # length of the signal
k = np.arange(n)
T = n / Fs
frq = k / T  # two sides frequency range
frq = frq[range(int(n / 2))]  # one side frequency range
Y = np.fft.fft(data) / n  # fft computing and normalization
Y = Y[range(int(n / 2))]

# FFT for IIR filtered dataset
n_filtered = len(filtered)  # length of the signal
k_filtered = np.arange(n_filtered)
T_filtered = n_filtered / Fs
frq_filtered = k_filtered / T_filtered  # two sides frequency range
frq_filtered = frq_filtered[range(int(n_filtered / 2))]  # one side frequency range
Y_filtered = np.fft.fft(filtered) / n_filtered
Y_filtered = Y_filtered[range(int(n_filtered / 2))]

# Plotting
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t, data, 'k', label='unfiltered')
ax1.plot(t, filtered, 'r', label='filtered')
ax1.set_title(f'Time Domain sigD, A = {A}, B = {B:.3f}')
ax1.set_xlabel('Time (sec)')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq, abs(Y), 'k', label='unfiltered')
ax2.loglog(frq_filtered, abs(Y_filtered), 'r', label='filtered')
ax2.set_title(f'Frequency Domain sigD, A = {A}, B = {B:.3f}')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

plt.tight_layout()
plt.legend()
plt.show()

