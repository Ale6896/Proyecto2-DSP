import numpy as np
from scipy.io.wavfile import write

# List of frequencies in Hz
frequencies = [50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 1000, 2000, 6000, 20000]

# Duration in seconds
t = 10.0

# Sample rate in Hz
sample_rate = 48000

for freq in frequencies:
    # Generate the time values
    t_values = np.linspace(0, t, int(t * sample_rate), False)

    # Generate the audio signal
    audio_signal = np.sin(freq * t_values * 2 * np.pi)

    # Ensure that highest value is in 16-bit range
    audio_signal *= 32767 / np.max(np.abs(audio_signal))

    # Convert to 16-bit data
    audio_signal = audio_signal.astype(np.int16)

    # Write the audio signal to a .wav file
    write(f'sine_wave_{freq}Hz.wav', sample_rate, audio_signal)

