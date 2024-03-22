"""
Python-ohjelma neuroverkon koulutusäänien nauhoittamista varten
Varmista että path on muokattu itselle sopivaksi
Painamalla eri numeroita nauhoitetaan eri sanoja
1 = sininen
2 = vihreä
3 = punainen
"""

import pyaudio, keyboard, wave, time, random, string, sys, os
import numpy as np

# Muokkaa omaksi, käytä /, elä \, python ei tykkää niistä. Loppuun ei kauttaviivaa.
# Koodi luo kansiot pathin päähän
oma_directory = "C:/Users/rikup/Desktop/School/4-vuosi-kevat/yritysprojekti/Python-toteutus"

FRAMES_PER_BUFFER = 3200
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
p = pyaudio.PyAudio()

def record_audio(directory):
    stream = p.open(
        format=FORMAT,
        channels=CHANNELS,
        rate=RATE,
        input=True,
        frames_per_buffer=FRAMES_PER_BUFFER
    )

    print("Nauhoitus aloitettu...")
    frames = []
    seconds = 1
    for i in range(0, int(RATE / FRAMES_PER_BUFFER * seconds)):
        data = stream.read(FRAMES_PER_BUFFER)
        frames.append(data)

    print("Nauhoitus lopetettu.")
    stream.stop_stream()
    stream.close()
    
    wavstring = np.frombuffer(b''.join(frames), dtype=np.int16)

    # Random nimi tiedostolle + reitti oikeean kansioon
    random_string = ''.join(random.choice(string.ascii_letters) for _ in range(10))
    output_file = f"{random_string}.wav"
    output_path = directory + output_file

    # Audiotiedoston tallennus
    with wave.open(output_path, 'w') as wav_file:
        # wave-tiedoston parametrien asetus (1 channel, 2 bytes per sample, sample rate)
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)  # 2 byteä = 16 bittiä
        wav_file.setframerate(16000)
        wav_file.writeframes(wavstring)


def aanittaminen():
    if keyboard.read_key() == "1":
        print("Painettu 1, nauhoitetaan sininen")
        directory = oma_directory + "/aanisamplet/sininen/sininen_"
        record_audio(directory)
        print("Tiedosto tallennettu kansioon sininen")

    elif keyboard.read_key() == "2":
        print("Painettu 2, nauhoitetaan vihreä")
        directory = oma_directory+"/aanisamplet/vihrea/vihrea_"
        record_audio(directory)
        print("Tiedosto tallennettu kansioon vihreä")

    elif keyboard.read_key() == "3":
        print("Painettu 3, nauhoitetaan punainen")
        directory = oma_directory+"/aanisamplet/punainen/punainen_"
        record_audio(directory)
        print("Tiedosto tallennettu kansioon punainen")
    
    elif keyboard.read_key() == "esc":
        print("Esciä painettu, suljetaan ohjelma")
        sys.exit()

    else:
        time.sleep(0.1)


if __name__ == "__main__":
    print("Ohjelma käynnistetään")
    # Kansio olemassaolon tarkistus, jos ei ole niin luodaan
    new_folder_check = oma_directory + "/aanisamplet"
    if not os.path.exists(new_folder_check):
        print("Luodaan kansiot")
        os.makedirs(new_folder_check+"/vihrea")
        os.makedirs(new_folder_check+"/sininen")
        os.makedirs(new_folder_check+"/punainen")
    while True:
        aanittaminen()
