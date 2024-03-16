# Ääniohjattu IoT-järjestelmä

OAMKissa lukuvuoden 2023-2024 neljännen jakson aikana toteutettava ryhmäprojekti.
Aiheena toteuttaa IoT-järjestelmä Thingy53-mikrokontrollerille, jota voi ohjata äänellä sytyttämään tiettyjä ledejä.

Aluksi toteutetaan toimiva konvoluutioneuroverkko joka tunnistaa ääntä Pythonilla, jonka jälkeen tämä muokataan C-kielelle ja siirretään Thingy53:seen.

Projektin jäsenet:
Riku Partanen, Arsi Kyrö, Kimmo Korhonen, Jere Manninen, Antti Rytilahti, Anton Ivanov, Thomas Grönroos, henkilö 8


Projektin vaiheet:
- Opetellaan äänikomentojen tunnistuksen teoria (deep neural network) Google CoLabissa
- Opetetaan konvoluutioneuroverkko tunnistamaan äänikomentoja (esim. punainen, sininen, vihreä)
- Opetellaan toteuttamaan opetettu konvoluutioneuroverkko koodaamalla se ensin Pythonilla vaihe vaiheelta
- Toteutetaan opetettu konvoluutioneuroverkko C-kielellä vaihe vaiheelta Thingy53-laitteeseen ja käytetään Thingy53:n mikrofonia äänikomentojen keräämiseen
- Lopuksi ajan salliessa toteutetaan ääniohjattu IoT-järjestelmä Nordic Matter protokollakirjaston avulla. Ääniohjatulla laitteella voisi sytyttää haluamansa värisen ledin verkkoon kytketyissä laitteissa.
