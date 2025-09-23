import numpy as np
from scipy.io.wavfile import write

# Frequencies in Hz
frequencies = np.arange(60, 6001, 60)

# Duration for each frequency in seconds
t = 1.0

# Sample rate in Hz
sample_rate = 48000

# Initialize the audio signal
audio_signal = np.array([])

for freq in frequencies:
    # Generate the time values
    t_values = np.linspace(0, t, int(t * sample_rate), False)

    # Generate the sine wave audio signal for this frequency
    signal = np.sin(2 * np.pi * freq * t_values)

    # Append the signal for this frequency to the overall audio signal
    audio_signal = np.concatenate((audio_signal, signal))

# Ensure that highest value is in 16-bit range
audio_signal *= 32767 / np.max(np.abs(audio_signal))

# Convert to 16-bit data
audio_signal = audio_signal.astype(np.int16)

# Write the audio signal to a .wav file
write('sine_wave_60Hz_to_6000Hz.wav', sample_rate, audio_signal)

