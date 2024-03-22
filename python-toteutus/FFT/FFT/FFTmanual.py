#manual FFT implementation 
import os

import cmath
import math

import numpy as np
import matplotlib.pyplot as plt


save = 0          #save to a file
show_pic = 1      #plot the spectrogram









#Windowing part of the spectrogram task

#dummy_signal = np.random.rand(16000)  #create dummy signal

#Load data from a file, preformatted
with open('preprocessed_track.txt', 'r') as file:
    dummy_signal = [float(line.strip()) for line in file]

window_size = 256  #sepcifying the size of the window, from the original code: frame_length=255
window_step = 128  #specifying the step of the window, from the original code: frame_step=128

number_of_windows = int((len(dummy_signal)-window_size) / window_step + 1) #calculate how much windows can fit, cast to int

windows = [] # made a list to store the values of every window
    
#windwing part
for i in range(number_of_windows):
    start = i * window_step
    end = start + window_size
    window = dummy_signal[start : end]
    if len(window) < window_size: 
        window = np.pad(window, (0, window_size - len(window)), 'constant') #Zero padding to fill the short signal, probably should be done even before the function
    windows.append(window)






#Apply hamming window to reduce spectral leakage

#Create a separate hamming window, 256 values long
def hamming(length):
 hamming_window = [0] * length
 for i in range(length):
    hamming_window[i] = 0.54 - 0.46 * math.cos(2 * math.pi *  (i / length))
 return hamming_window

#cast to numpy array
hamming_case = np.array(hamming(window_size))

#apply to windows
for i in range(len(windows)):
 windows[i] *= hamming_case
 






#The part where the Fourier transform is applied 


#Implementig the Fast Fourier Transform via the Cooley-Tukey algorithm:
#FFT goes through each "window", so only the 1 window should be passed at a time to the function
def FFT(widnow):
 Length = len(window)
 if len(window)<=1:
  return window
 even = [] #create an even list
 odd = [] #create an odd list
 for i in range(Length):  #go through each value of the 256 values long window 
  if (i % 2)  == 0: #check how the value is placed in the window
   even.append(window[i])
  else: 
   odd.append(window[i])
 merged = [0] * Length
 for i in range(Length // 2): 
  twiddle_factor = math.cos(2* math.pi * i / Length) - 1j * math.sin(2 * math.pi * i  / Length) #expressed using Euler's formula
  merged[i] = even[i] + (twiddle_factor * odd[i])                                               #butterfly
  merged[i + Length // 2] = even[i] - (twiddle_factor * odd[i])
 return merged 
  


spectrogram = [] #list to store the answer

for window in windows: 
    #fft_result  = np.fft.fft(window)   #the next step of the project is to develop a custom fft function instead of utilizing the already existing one 
    fft_result  = FFT(window)
    absolute = abs(fft_result)[:window_size//2]     #calculate the absolute number  
    spectrogram.append(absolute)                  #basically has the answer of FFT as an absolute number
    


#the plotting part is a "hard copypaste" from interenet


#The part where is the 
spectrogram_data = np.array(spectrogram).T

plt.figure(figsize=(10, 4))
plt.imshow(spectrogram_data, aspect='auto', origin='lower', extent=[0, number_of_windows, 0, window_size // 2])
plt.xlabel('Aika/Ikkunat')
plt.ylabel('Taajuus(Hz)')
plt.colorbar(label='Magnitudi')
plt.title('Spektrogrammi')
if show_pic == 1:
 plt.show() 



#save to a file if needed

if save == 1: 
 output_file0 = "preprocessed_track_of_my_example.txt"
 with open(output_file0, "w") as file:
  for value in windows:
   file.write(f"{value}\n")

 output_file1 = "processed_track_of_my_example.txt"
 with open(output_file1, "w") as file:
  for value in get_amplitude:
   file.write(f"{value}\n")