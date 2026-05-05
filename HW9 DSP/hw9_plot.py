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

# FFT for each dataset
Fs = 10000  # sample rate

for t_data, data, label in [(t_A, data_A, 'sigA'), (t_B, data_B, 'sigB'), (t_C, data_C, 'sigC'), (t_D, data_D, 'sigD')]:
    n = len(data)  # length of the signal
    k = np.arange(n)
    T = n / Fs
    frq = k / T  # two sides frequency range
    frq = frq[range(int(n / 2))]  # one side frequency range
    Y = np.fft.fft(data) / n  # fft computing and normalization
    Y = Y[range(int(n / 2))]

    # Plotting
    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t_data, data, 'b')
    ax1.set_title(f'Time Domain - {label}')
    ax1.set_xlabel('Time (sec)')
    ax1.set_ylabel('Amplitude')

    ax2.loglog(frq, abs(Y), 'b')  # plotting the fft
    ax2.set_title(f'Frequency Domain - {label}')
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')

    plt.tight_layout()

plt.show()

