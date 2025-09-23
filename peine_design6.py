import numpy as np
from scipy.signal import freqz
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')

# Parameters
fs = 48000.0  # Sampling frequency (Hz)
f0 = 60.0  # Frequency to remove (Hz)
L = int(fs / f0)  # Delay length
alpha = 0.9  # Scaling factor
## WE choose the radius of the pole to be 0.8, then calculate K so that at about 30Hz, we have gain of 1, also shift the gain of the frequency response by pi by changing signe of odd coefficients.

## eq used to obtian K in octave for gain 1 at 30Hz in octave 
#octave:6> (1-0.8*cos(2*pi*freq/Fs*L)+i*sin(2*pi*freq/Fs*L))/(1-cos(2*pi*freq/Fs*L)+i*sin(2*pi*freq*L/Fs))
#ans =  9.0000e-01 + 2.2817e-16
# Design the filter
coeff_b = np.zeros(L+1)
coeff_b[0] = 0.9
coeff_b[-1] = -alpha

coeff_a = np.zeros(L+1)
coeff_a[0] = 1
#coeff_a[1] = -1
coeff_a[-1] = -0.8  # Adda pole at z=1

# Frequency shift
#for k in range(1, L+1, 2): 
    #coeff_b[k] *= -1

# Compute frequency response
w, h = freqz(coeff_b, coeff_a, worN=8000)

# Plot magnitude response
plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(w * fs / (2*np.pi), abs(h), 'b')
plt.title('Magnitude and Phase Response of the Frequency-Shifted IIR Comb Filter')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.grid()

# Plot phase response
plt.subplot(2, 1, 2)
angles = np.unwrap(np.angle(h))
plt.plot(w * fs / (2*np.pi), angles, 'g')
plt.ylabel('Phase (radians)')
plt.grid()
plt.tight_layout()
plt.show()
print("Filter coefficients:")
print("b =", coeff_b)
print("a =", coeff_a)
print("L =", L)

