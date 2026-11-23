# Gebruiksaanwijzing — Line follower V4

Deze handleiding hoort bij de sketch `LineFollowerV4.ino` voor de Arduino Nano ESP32. De robot volgt een donkere lijn met een analoge sensorarray. De Nano ESP32 maakt een eigen Wi-Fi-netwerk waarop je de status kunt bekijken, de robot kunt starten en stoppen, sensoren kunt kalibreren en regelparameters kunt instellen.

## 1. Voor gebruik

- Zet de robot op een vlakke ondergrond met ruimte om te rijden.
- Controleer of de wielen vrij kunnen draaien en of er geen losse draden zijn.
- Controleer of de sensorarray naar beneden gericht is en dicht genoeg bij het parcours staat.
- Laad de XTAR 18650-cellen alleen met een geschikte Li-ion-lader. De XTAR MC1 laadt **één cel per keer**; laad geen in serie geschakeld accupakket met deze lader.
- Controleer vóór inschakelen dat de accu's en voedingsspanning volgens het elektrisch schema zijn aangesloten.

## 2. Inschakelen en verbinden

1. Zet de aan/uit-schakelaar van de robot aan.
2. Open de Wi-Fi-instellingen op je gsm of laptop.
3. Verbind met **LineFollower-Ward**.
4. Voer wachtwoord **12345678** in.
5. Kies indien gevraagd om verbonden te blijven zonder internet.
6. Open een browser en ga naar [http://192.168.4.1](http://192.168.4.1).
7. De webpagina toont de robotstatus als **rijdt** of **gestopt**. De status wordt ongeveer elke halve seconde bijgewerkt.

## 3. Sensoren kalibreren

Kalibreer de sensoren voor het parcours en de lichtomstandigheden waarin je gaat rijden. Kalibreren zet de robot stil.

1. Zet de robot op een stabiele plek en open de webpagina.
2. Plaats de sensorarray boven de zwarte lijn.
3. Druk op **Meet zwart**.
4. Plaats de sensorarray boven de lichte achtergrond.
5. Druk op **Meet wit**.
6. De waarden worden opgeslagen in het geheugen en blijven bewaard na uitschakelen.

De webpagina geeft geen aparte melding dat de kalibratie is geslaagd. Zorg dat iedere sensor tijdens de kalibratie daadwerkelijk boven de bedoelde zwarte of witte ondergrond staat. Kalibreer opnieuw als de verlichting of het parcours verandert.

## 4. Starten en stoppen

### Via de webpagina

- Druk op **Start** om lijnvolgen te starten. De status verandert naar **rijdt**.
- Druk op **Stop** om de motoren stil te zetten. De status verandert naar **gestopt**.

### Via de fysieke drukknop

De knop is aangesloten op Nano-pin D2. Druk eenmaal om tussen rijden en stoppen te wisselen. De webpagina blijft de actuele softwarestatus tonen zolang je gsm verbonden is.

De blauwe status-led op D12 brandt wanneer de software in de rijmodus staat en gaat uit wanneer de robot gestopt is.

**Let op:** bij verlies van de Wi-Fi-verbinding stopt de robot niet automatisch. Gebruik de fysieke knop of schakel de robot uit als je hem niet via de webpagina kunt stoppen.

## 5. Instellingen wijzigen

Vul de gewenste waarde in en druk op **Parameters opslaan**. De waarden worden opgeslagen. De robot wordt daarbij gestopt; start hem opnieuw wanneer je verder wilt testen.

| Instelling | Betekenis | Beschikbaar bereik / notitie |
|---|---|---|
| CycleTime | Tijd tussen regelberekeningen | 1.000–1.000.000 microseconden |
| Power | Basissnelheid van de motoren | 0–255 |
| Diff | Verschilcorrectie voor de motorsnelheid | Numerieke waarde |
| Kp | Proportionele regelactie | Numerieke waarde |
| Ki | Integrale regelactie | Numerieke waarde; waarde wordt voor CycleTime omgerekend |
| Kd | Differentiële regelactie | Numerieke waarde; waarde wordt voor CycleTime omgerekend |

Verander bij het afstellen telkens één parameter tegelijk. Test op lage snelheid en stop de robot voordat je instellingen opslaat. Er zijn geen universele Kp-, Ki- of Kd-waarden: die hangen af van het parcours, de motoren, de batterijspanning en de mechanische montage.

## 6. Uitschakelen en opladen

1. Druk op **Stop** of gebruik de fysieke drukknop.
2. Zet de aan/uit-schakelaar uit.
3. Maak de accu('s) los volgens de connectoren van de robot.
4. Laad de XTAR-cellen één voor één in de XTAR MC1 met een 5 V USB-voeding.
5. Volg de statusindicatie en aanwijzingen van de lader. Koppel de lader los wanneer het laden voltooid is.
6. Plaats de cellen terug volgens het elektrisch schema en zet de schakelaar weer aan wanneer je wilt rijden.

**Laad nooit twee cellen die in serie met elkaar verbonden zijn tegelijk in de MC1.** De MC1 is bedoeld voor één losse Li-ion-cel.

## 7. Problemen oplossen

| Probleem | Controleer dit |
|---|---|
| Wi-Fi-netwerk verschijnt niet | Staat de robot aan? Herstart de Nano ESP32 en zoek opnieuw naar `LineFollower-Ward`. |
| Webpagina opent niet | Controleer dat je verbonden bent met het robotnetwerk en gebruik `http://192.168.4.1`. Mobiele data of internet is niet nodig. |
| Status toont “geen verbinding” of ververst niet | Controleer de Wi-Fi-verbinding en laad de pagina opnieuw. |
| Robot rijdt niet op de lijn | Stop, controleer de sensorhoogte en kalibreer opnieuw op zwart en wit. Controleer ook of de sensoruitgangen in links-rechtsvolgorde zijn aangesloten. |
| Robot draait de verkeerde kant op | Stop de robot en controleer de motoruitgangen en bedrading. Verwissel de twee draden van de betreffende motor als de draairichting omgekeerd is. |
| Robot stopt na opslaan van parameters | Dit is normaal. Druk opnieuw op **Start**. |
| Robot stopt niet via de webpagina | Gebruik de fysieke knop of schakel de voeding uit. |

## 8. Belangrijke gegevens

- Wi-Fi-naam: `LineFollower-Ward`
- Wi-Fi-wachtwoord: `12345678`
- Webadres: `http://192.168.4.1`
- Statusverversing: ongeveer 500 ms
- Fysieke start/stopknop: D2
- Blauwe rijstatus-led: D12
