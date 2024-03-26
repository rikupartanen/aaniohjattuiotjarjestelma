#manual FFT implementation 
import os

#import cmath
import math

import numpy as np
import matplotlib.pyplot as plt

save = 0          #save to a file
show_pic = 0      #plot the spectrogram

#Windowing part of the FFT and spectrogram task

#dummy_signal = np.random.rand(16000)  #create dummy signal

#Load data from a file, preformatted
with open('preprocessed_track.txt', 'r') as file:
    dummy_signal = [float(line.strip()) for line in file]

window_size = 256  #sepcifying the size of the window, from the original code: frame_length=255
window_step = 128  #specifying the step of the window, from the original code: frame_step=128

number_of_windows = int((len(dummy_signal)-window_size) / window_step + 1) #calculate how much windows can fit, cast to int

windows = [] # made a list to store the values of every window
    
#windowing part
for i in range(number_of_windows):
    start = i * window_step #set the frist and last position
    end = start + window_size
    window = dummy_signal[start : end]  #save data in windows
    if len(window) < window_size: 
        window = np.pad(window, (0, window_size - len(window)), 'constant') #Zero padding to fill the length < 256
    windows.append(window)


#Apply hamming window to reduce spectral leakage

#Create a separate hamming window, 256 values long
def hamming(length):
 hamming_window = [0] * length #list for hamming window
 for i in range(length):
    hamming_window[i] = 0.54 - 0.46 * math.cos(2 * math.pi *  (i / length))
 return hamming_window

#cast to numpy array
hamming_case = np.array(hamming(window_size))

#apply to windows
for i in range(len(windows)):
 windows[i] *= hamming_case
 
#The part where the Fourier transform is applied 


#Implementig the Fast Fourier Transform via the Cooley-Tukey algorithm with radix-2 implementation
#FFT goes through each "window", so only the window should be passed at a time to the function, not the entire data
def FFT(window):
 Length = len(window)
 if Length <= 1:
  return window
 
 even = [0] * (Length // 2) #create lists for even and odd positions from window
 odd = [0] * (Length // 2)
 
 for i in range(Length):   #split into even and odd positions
  if i % 2  == 0: 
   even[i // 2] = window[i]
  else: 
   odd[i // 2] = window[i]
 
 even = FFT(even)           #calculate recursively even and odd FFTs
 odd = FFT(odd)

 merged = [0] * Length
 
 for i in range(Length // 2): #combine even and odd FFTs
  twiddle_factor = math.cos(2* math.pi * i / Length) - 1j * math.sin(2 * math.pi * i  / Length) #twiddle factor expressed with Euler's formula
  merged[i] = even[i] + twiddle_factor * odd[i]                                                 #butterfly calculations 
  merged[i + Length // 2] = even[i] - twiddle_factor * odd[i]
 return merged 
  


spectrogram = [] #list to store the answer

for window in windows: 
    #fft_result  = np.fft.fft(window)   #the next step of the project is to develop a custom fft function instead of utilizing the already existing one 
    fft_result  = FFT(window)           #manual FFT function
    fft_result = np.array(fft_result)   #cast to numpy.array
    absolute = abs(fft_result)[:window_size//2]     #calculate the absolute number  
    spectrogram.append(absolute)                  #basically has the answer of FFT as an absolute number
    


#Probably the value you will need in your function
spectrogram_data = np.array(spectrogram)

#old plotting function, copypasted
#plt.figure(figsize=(10, 4))
#plt.imshow(spectrogram_data, aspect='auto', origin='lower', extent=[0, number_of_windows, 0, window_size // 2])
#plt.xlabel('Aika/Ikkunat')
#plt.ylabel('Taajuus(Hz)')
#plt.colorbar(label='Magnitudi')
#plt.title('Spektrogrammi')
#if show_pic == 1:
#plt.show() 


#plotting function from the target example, copypasted and edited a bit to work with this FFT implementation

def plot_spectrogram(spectrogram, ax):
 if len(spectrogram.shape) > 2:
  assert len(spectrogram.shape) == 3
  spectrogram = np.squeeze(spectrogram, axis=-1)
  #Convert the frequencies to log scale and transpose, so that the time is
  #represented on the x-axis (columns).
  #Add an epsilon to avoid taking a log of zero.
 log_spec = np.log(spectrogram.T + np.finfo(float).eps)
 height = log_spec.shape[0]
 width = log_spec.shape[1]
 X = np.linspace(0, np.size(spectrogram), num=width, dtype=int)
 Y = range(height)
 ax.pcolormesh(X, Y, log_spec)

  
fig, axes = plt.subplots(2, figsize=(12, 8))
timescale = np.arange(spectrogram_data.shape[0])
axes[0].plot(timescale, spectrogram_data)
axes[0].set_title('Waveform')
axes[0].set_xlim([0, 16000])

plot_spectrogram(spectrogram_data, axes[1])
axes[1].set_title('Spectrogram')
plt.suptitle("Up(Cooley-Tukey algoritmi)")
plt.show()


#save to a file if needed

if save == 1: 
 output_file0 = "preprocessed_track_of_my_example.txt"
 with open(output_file0, "w") as file:
  for value in windows:
   file.write(f"{value}\n")

 output_file1 = "processed_track_of_my_example.txt"
 with open(output_file1, "w") as file:
  for value in spectrogram_data:
   file.write(f"{value}\n")
