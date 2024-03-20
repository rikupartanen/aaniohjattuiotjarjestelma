#manual FFT implementation 
save = 0
import os

import numpy as np
import matplotlib.pyplot as plt


#Windowing part of the spectrogram task

#dummy_signal = np.random.rand(16000)  #create dummy signal

with open('preprocessed_track.txt', 'r') as file:
    dummy_signal = [float(line.strip()) for line in file]
#print("The dummy signal is:", dummy_signal)
#print("The length of the signal is:", len(dummy_signal))

window_size = 256  #sepcifying the size of the window, from the original code: frame_length=255
window_step = 128  #specifying the step of the window, from the original code: frame_step=128

number_of_windows = int((len(dummy_signal)-window_size) / window_step + 1) #calculate how much windows we will get
#print("The number of windows is:", number_of_windows)

windows = [] # made to store the values of every window

for i in range(number_of_windows):
    start = i * window_step
    #print("The start value is:", start)
    end = start + window_size
    #print("The end value is:", end)
    window = dummy_signal[start : end]
    if len(window) < window_size: 
        window = np.pad(window, (0, window_size - len(window)), 'constant') #Zero padding to fill the short signal.
        #
    windows.append(window)
#print("The length of the windows is:", len(windows))

#Apply hamming window to reduce spectral leakage, as a lot of examples used it
for i in range(len(windows)):
    windows[i] *= np.hamming(window_size)


#The part where the Fourier transform is applied 

get_amplitude = [] #The answer of the Fourier transfrom which consists of complex number, but we could get hte answer by , se we need to get the magnitude of each frequency to display it on the graph as as spectrogram, which will be sotred 

for window in windows: 
    fft_result  = np.fft.fft(window)   #the next step of the project is to develop a custom fft function instead of utilizing the already existing one 
    magnitude = np.abs(fft_result)[:window_size//2]     #calculate the absoolute number of the FFT to show 
    get_amplitude.append(magnitude)                     #basically has the answer of FFT as an absolute numbers 
    


#the plotting part is a "hard copypaste" from interenet


#The part where is the 
spectrogram_data = np.array(get_amplitude).T

plt.figure(figsize=(10, 4))
plt.imshow(spectrogram_data, aspect='auto', origin='lower', extent=[0, number_of_windows, 0, window_size // 2])
plt.xlabel('Aika/Ikkunat')
plt.ylabel('Taajuus(Hz)')
plt.colorbar(label='Magnitudi')
plt.title('Spektrogrammi')
plt.show() 

if save == 1: 
 output_file0 = "preprocessed_track_of_my_example.txt"
 with open(output_file0, "w") as file:
  for value in windows:
   file.write(f"{value}\n")

 output_file1 = "processed_track_of_my_example.txt"
 with open(output_file1, "w") as file:
  for value in get_amplitude:
   file.write(f"{value}\n")
