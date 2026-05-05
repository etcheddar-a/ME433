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

# filter data with moving average filter
X = 500  # window size
moving_avg = np.convolve(data, np.ones(X)/X, mode='valid') # we learned this in BME 309! easier than making a loop
# trim time vector to match
t_moving_avg = t[X-1:]

# FFT for original dataset
Fs = 10000  # sample rate
n = len(data)  # length of the signal
k = np.arange(n)
T = n / Fs
frq = k / T  # two sides frequency range
frq = frq[range(int(n / 2))]  # one side frequency range
Y = np.fft.fft(data) / n  # fft computing and normalization
Y = Y[range(int(n / 2))]

# FFT for moving average filtered dataset
n_avg = len(moving_avg)  # length of the signal
k_avg = np.arange(n_avg)
T_avg = n_avg / Fs
frq_avg = k_avg / T_avg  # two sides frequency range
frq_avg = frq_avg[range(int(n_avg / 2))]  # one side frequency range
Y_avg = np.fft.fft(moving_avg) / n_avg
Y_avg = Y_avg[range(int(n_avg / 2))]

# Plotting
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t, data, 'k', label='unfiltered')
ax1.plot(t_moving_avg, moving_avg, 'r', label='filtered')
ax1.set_title(f'Time Domain sigD, X = {X}')
ax1.set_xlabel('Time (sec)')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq, abs(Y), 'k', label='unfiltered')
ax2.loglog(frq_avg, abs(Y_avg), 'r', label='filtered')
ax2.set_title(f'Frequency Domain sigD, X = {X}')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

plt.tight_layout()
plt.legend()
plt.show()

