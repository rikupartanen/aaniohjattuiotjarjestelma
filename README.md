English below

# Ääniohjattu IoT-järjestelmä

OAMKissa lukuvuoden 2023-2024 neljännen jakson aikana toteutettava ryhmäprojekti.
Aiheena toteuttaa IoT-järjestelmä Thingy53-mikrokontrollerille, jota voi ohjata äänellä sytyttämään tiettyjä ledejä.

Aluksi toteutetaan toimiva konvoluutioneuroverkko joka tunnistaa ääntä Pythonilla, jonka jälkeen tämä muokataan C-kielelle ja siirretään Thingy53:seen.

Projektin jäsenet:
Riku Partanen, Arsi Kyrö, Kimmo Korhonen, Jere Manninen, Antti Rytilahti, Anton Ivanov, Thomas Grönroos ja Petteri Karjalainen


Projektin vaiheet:
- Opetellaan äänikomentojen tunnistuksen teoria (deep neural network) Google CoLabissa
- Opetetaan konvoluutioneuroverkko tunnistamaan äänikomentoja (esim. punainen, sininen, vihreä)
- Opetellaan toteuttamaan opetettu konvoluutioneuroverkko koodaamalla se ensin Pythonilla vaihe vaiheelta
- Toteutetaan opetettu konvoluutioneuroverkko C-kielellä vaihe vaiheelta Thingy53-laitteeseen ja käytetään Thingy53:n mikrofonia äänikomentojen keräämiseen
- Lopuksi ajan salliessa toteutetaan ääniohjattu IoT-järjestelmä Nordic Matter protokollakirjaston avulla. Ääniohjatulla laitteella voisi sytyttää haluamansa värisen ledin verkkoon kytketyissä laitteissa.


# Voice-Controlled IoT System
Group project implemented during the fourth period of the 2023-2024 academic year at OAMK.
The topic is to implement an IoT system for the Thingy53 microcontroller, which can be controlled by voice to light up certain LEDs.

Initially, a functional convolutional neural network that recognizes speech will be implemented in Python, after which it will be converted to C and transferred to the Thingy53.

Project members:
Riku Partanen, Arsi Kyrö, Kimmo Korhonen, Jere Manninen, Antti Rytilahti, Anton Ivanov, Thomas Grönroos and Petteri Karjalainen

Project phases:

- Learn the theory of voice command recognition (deep neural network) in Google CoLab
- Train the convolutional neural network to recognize voice commands (e.g., red, blue, green)
- Learn to implement the trained convolutional neural network by coding it first in Python step by step
- Implement the trained convolutional neural network in C step by step on the Thingy53 device and use the Thingy53's microphone to collect voice commands
- Finally, if time permits, implement the voice-controlled IoT system using the Nordic Matter protocol library. With the voice-controlled device, one could light up the desired color LED on network-connected devices.
