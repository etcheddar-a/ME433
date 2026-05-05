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

h = [
    0.000000000000000000,
    0.000006654468709502,
    0.000029128522936023,
    0.000071630294094485,
    0.000139045364715286,
    0.000237018146420107,
    0.000371990776314115,
    0.000551191427798928,
    0.000782566139698271,
    0.001074650878794299,
    0.001436383452247726,
    0.001876857948081446,
    0.002405027463171737,
    0.003029363833662816,
    0.003757485769150283,
    0.004595769074907365,
    0.005548954406576212,
    0.006619769140808002,
    0.007808580390891126,
    0.009113095905728782,
    0.010528128553079571,
    0.012045438326481696,
    0.013653663385294468,
    0.015338348625262888,
    0.017082076796941639,
    0.018864703378109852,
    0.020663692418049256,
    0.022454546570933227,
    0.024211320690754524,
    0.025907204835505290,
    0.027515159476891312,
    0.029008583268927898,
    0.030361992005490210,
    0.031551686475304340,
    0.032556386852028101,
    0.033357812050676970,
    0.033941184117168982,
    0.034295640137166114,
    0.034414537262454484,
    0.034295640137166114,
    0.033941184117168982,
    0.033357812050676984,
    0.032556386852028101,
    0.031551686475304340,
    0.030361992005490220,
    0.029008583268927898,
    0.027515159476891322,
    0.025907204835505290,
    0.024211320690754531,
    0.022454546570933234,
    0.020663692418049256,
    0.018864703378109866,
    0.017082076796941632,
    0.015338348625262895,
    0.013653663385294473,
    0.012045438326481693,
    0.010528128553079576,
    0.009113095905728785,
    0.007808580390891126,
    0.006619769140808005,
    0.005548954406576217,
    0.004595769074907367,
    0.003757485769150290,
    0.003029363833662813,
    0.002405027463171739,
    0.001876857948081447,
    0.001436383452247725,
    0.001074650878794302,
    0.000782566139698270,
    0.000551191427798929,
    0.000371990776314116,
    0.000237018146420106,
    0.000139045364715286,
    0.000071630294094485,
    0.000029128522936022,
    0.000006654468709502,
    0.000000000000000000,
]

# Apply FIR filter
filtered_data = np.convolve(data, h, mode='valid') # learned convoluion in BME 309! easier than making a loop
# Trim time vector to match filtered data
filtered_time = t[len(h)-1:]

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
n_avg = len(filtered_data)  # length of the signal
k_avg = np.arange(n_avg)
T_avg = n_avg / Fs
frq_avg = k_avg / T_avg  # two sides frequency range
frq_avg = frq_avg[range(int(n_avg / 2))]  # one side frequency range
Y_avg = np.fft.fft(filtered_data) / n_avg
Y_avg = Y_avg[range(int(n_avg / 2))]

# Plotting
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t, data, 'k', label='unfiltered')
ax1.plot(filtered_time, filtered_data, 'r', label='filtered')
ax1.set_title(f'Time Domain sigD, {len(h)}-point FIR Filter')
ax1.set_xlabel('Time (sec)')
ax1.set_ylabel('Amplitude')

ax2.loglog(frq, abs(Y), 'k', label='unfiltered')
ax2.loglog(frq_avg, abs(Y_avg), 'r', label='filtered')
ax2.set_title(f'Frequency Domain sigD, cutoff = 100 Hz, bandwidth = 600 Hz')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

plt.tight_layout()
plt.legend()
plt.show()

