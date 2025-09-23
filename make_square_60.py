import numpy as np
from scipy.io.wavfile import write

# Frequency in Hz
freq = 62

# Duration in seconds
t = 10.0

# Sample rate in Hz
sample_rate = 48000

# Generate the time values
t_values = np.linspace(0, t, int(t * sample_rate), False)

# Generate the square wave audio signal
audio_signal = np.sign(np.sin(2 * np.pi * freq * t_values))

# Ensure that highest value is in 16-bit range
audio_signal *= 32767 / np.max(np.abs(audio_signal))

# Convert to 16-bit data
audio_signal = audio_signal.astype(np.int16)

# Write the audio signal to a .wav file
write(f'square_wave_{freq}Hz.wav', sample_rate, audio_signal)

