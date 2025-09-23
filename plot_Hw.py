import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')
# Load the data from the file
data = np.fromfile('result.bin', dtype=np.complex64)

# Calculate magnitude and phase
magnitude = np.abs(data)
phase = np.angle(data)

# Create the plots
plt.figure()
plt.subplot(2, 1, 1)
plt.plot(magnitude)
plt.title('Magnitude')

plt.subplot(2, 1, 2)
plt.plot(phase)
plt.title('Phase')

# Display the plots
plt.show()

