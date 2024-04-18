#Manual FFT-implementation
import os
import math
import numpy as np
import matplotlib.pyplot as plt


#Loads preformatted audio data from a text file, the input must have 16000 values 
with open('preprocessed_track.txt', 'r') as file:
    signal = [float(line.strip()) for line in file]

window_size = 256 #Defines how much values one window has 
window_step = 128 #Specifies overlap per window
number_of_windows = int((len(signal)-window_size) / window_step + 1) #Calculates the number of windows  

windows = []  
    
#Windowing part, splits audio data into smaller parts, should be 124 windows and 256 values in each window
for i in range(number_of_windows):
    start = i * window_step #Sets overlap positions for each window
    end = start + window_size
    window = signal[start : end]
    if len(window) < window_size: 
        window = np.pad(window, (0, window_size - len(window)), 'constant') #Zero padding, isn't present in the C version
    windows.append(window)



#Hunning window to reduce spectral leakage
def hunning(length):
 hunningWindow = [0] * length #Stores Hunning window
 for i in range(length):
    hunningWindow[i] = 0.5 - 0.5 * math.cos(2 * math.pi *  (i / length))
 return hunningWindow

hunning_case = np.array(hunning(window_size))

#Applies the Hunning Window function
for i in range(len(windows)):
 windows[i] *= hunning_case
 

#Cooley-Tukey radix-2 FFT, recursive version
#Only 1 window(256 values) should be passed at a time
def FFT(window):
 Length = len(window)
 if Length <= 1:
  return window
 
 even = [0] * (Length // 2)
 odd = [0] * (Length // 2)
 
 for i in range(Length):   #Saves even and odd position 
  if i % 2  == 0: 
   even[i // 2] = window[i]
  else: 
   odd[i // 2] = window[i]
 
 even = FFT(even)           #Recursive calls to split
 odd = FFT(odd)

 merged = [0] * Length
 
 for i in range(Length // 2): #Butterfly calculations
  twiddle_factor = math.cos(2* math.pi * i / Length) - 1j * math.sin(2 * math.pi * i  / Length) #Twiddle factor expressed with Euler's formula
  merged[i] = even[i] + twiddle_factor * odd[i]                                                 #Butterfly part
  merged[i + Length // 2] = even[i] - twiddle_factor * odd[i]
 return merged 
  
spectrogram = [] 

for window in windows: 
    fft_result  = FFT(window)           #Manual FFT function
    fft_result = np.array(fft_result)
    absolute = abs(fft_result)[:window_size // 2 + 1]   #Calculates the absolute number  
    spectrogram.append(absolute)                  #Appends spectrogram results
    


#Final result
spectrogram_data = np.array(spectrogram)
