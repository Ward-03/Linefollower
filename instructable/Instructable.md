# Instructable — Line follower met Arduino Nano ESP32

> Dit stappenplan volgt de huidige sketch `LineFollowerV4.ino`: Arduino Nano ESP32, DRV8833, analoge 8-kanaals QTR-sensorarray en Wi-Fi-webbediening. De bedrading hieronder volgt de pinnen in de sketch. Volg voor de mechanische montage de technische tekeningen en voor de componentkeuze de actuele bill of materials.

## Benodigdheden

- Chassis/protoboard en bevestigingsmateriaal
- Arduino Nano ESP32
- 2 × ASLONG JGA12-N20-30 motor, 6 V, 500 rpm, met motorhouders en wielen
- DRV8833 dubbele H-brugmodule
- 8-kanaals analoge QTR-sensorarray
- 2 × XTAR 18650 Li-ion-cel, 3500 mAh, met beveiligingscircuit
- XTAR MC1-lader en geschikte 5 V USB-voeding
- Aan/uit-schakelaar, startknop, bedrading en connectoren volgens het elektrisch schema
- Soldeerbout, soldeertin, draadstripper, kniptang, schroevendraaiers en multimeter
- Computer met Arduino IDE en USB-C-datakabel; smartphone of laptop met Wi-Fi

## Montage en bedrading

### Stap 1 — Controleer de onderdelen en tekeningen

Leg de onderdelen klaar. Controleer de mechanische tekening, het elektrisch schema en de pinlabels op jouw DRV8833-module voordat je soldeert.

### Stap 2 — Monteer het chassis

Bevestig de motorhouders en motoren op het chassis volgens de mechanische tekening. Monteer de wielen en het zwenkwiel. Plaats de sensorarray vooraan, naar de vloer gericht, met voldoende ruimte om de lijn te detecteren.

### Stap 3 — Monteer elektronica en aansluitingen

Plaats de Nano ESP32, DRV8833, sensorarray, schakelaar en connectoren op de daarvoor bedoelde headers of montageplaatsen. Controleer dat de USB-aansluiting bereikbaar blijft.

### Stap 4 — Sluit de sensorarray aan

Verbind VCC en GND van de sensorarray met de voedingsrails zoals aangegeven op het elektrisch schema. Verbind de acht analoge uitgangen met de analoge Nano-pinnen. De sketch leest de sensoren in deze volgorde:

| Sensorindex in sketch | Nano-pin |
|---:|---|
| 0 | A7 |
| 1 | A6 |
| 2 | A5 |
| 3 | A4 |
| 4 | A3 |
| 5 | A2 |
| 6 | A1 |
| 7 | A0 |

Controleer op jouw sensor welke uitgang bij de linker- en rechterkant hoort. De volgorde bepaalt de berekende lijnpositie; verwissel de signaaldraden of pas de arrayvolgorde aan als links en rechts omgekeerd reageren.

### Stap 5 — Verbind de Nano met de DRV8833

Verbind de Nano digitale pinnen met de driver-ingangen volgens de sketch:

| Nano ESP32 | DRV8833 | Rol volgens sketch |
|---|---|---|
| D5 | IN3 | Linkermotor, vooruit-signaal |
| D3 | IN4 | Linkermotor, achteruit-signaal |
| D6 | IN2 | Rechtermotor, vooruit-signaal |
| D9 | IN1 | Rechtermotor, achteruit-signaal |
| D4 | SLEEP / nSLEEP | Driver inschakelen |
| GND | GND | Gemeenschappelijke massa |

De sketch zet SLEEP hoog om de driver te activeren. Sluit elke motor aan op één paar motoruitgangen van de DRV8833 (AOUT1/AOUT2 of BOUT1/BOUT2). Gebruik de kanaaltoewijzing van jouw module/schema. Als een motor bij de latere richtingscontrole verkeerdom draait, verwissel dan de twee draden van die motor.

### Stap 6 — Sluit de voeding aan

Sluit accu, schakelaar, motordriver en Nano-voeding aan volgens het elektrisch schema voor jouw uitvoering. Controleer vóór inschakelen met een multimeter op kortsluiting tussen plus en GND en controleer dat alle GND's gemeenschappelijk zijn.

**Laad de XTAR-cellen met de XTAR MC1 één cel tegelijk, buiten de robot.** De MC1 is een lader voor één Li-ion-cel; gebruik hem niet om twee in serie geschakelde cellen of een accupakket te laden. Volg voor de manier waarop de cellen de robot voeden uitsluitend het juiste schema voor jouw accuhouder en spanningsregelaar.

### Stap 7 — Controleer de bedrading

Controleer alle polariteiten, motoruitgangen, signaalpinnen en soldeerverbindingen. Schakel de robot nog niet in als er losse draden zijn of als de voedingsspanning niet overeenkomt met het schema en de onderdelen.

## Arduino IDE instellen en code uploaden

### Stap 8 — Installeer Arduino IDE en boardondersteuning

Installeer Arduino IDE. Open **Tools → Board → Boards Manager**, zoek naar **Arduino ESP32 Boards** en installeer het boardpakket indien dit nog niet aanwezig is.

### Stap 9 — Open de sketch

Open `LineFollowerV4.ino` in de Arduino IDE. Kies **Tools → Board → Arduino ESP32 Boards → Arduino Nano ESP32**. Kies zo nodig **Tools → Pin Numbering → By Arduino pin** en selecteer onder **Tools → Port** de Nano ESP32.

### Stap 10 — Compileer en upload

Klik op **Verify** en wacht tot de compilatie klaar is. Verbind de Nano via USB-C en klik op **Upload**. Wacht op de melding dat de upload is voltooid. Als de upload niet start, zet de Nano in bootloaderstand volgens de boardinstructies en probeer opnieuw.

### Stap 11 — Start de robot op

Koppel de USB-kabel los wanneer je de robot standalone wilt gebruiken. Schakel de voeding in. De Nano start een eigen Wi-Fi-netwerk. Bij een reset begint de softwarestatus op **gestopt**.

## Verbinden en bedienen

### Stap 12 — Verbind de gsm of laptop

Verbind met het Wi-Fi-netwerk:

- **Netwerknaam:** `LineFollower-Ward`
- **Wachtwoord:** `12345678`
- **Webpagina:** [http://192.168.4.1](http://192.168.4.1)

### Stap 13 — Controleer de status en bediening

De webpagina toont **rijdt** of **gestopt**. De status wordt tijdens gebruik automatisch bijgewerkt. Gebruik **Start** om de lijnvolgmodus te starten en **Stop** om de motoren stil te zetten. De fysieke startknop op D2 wisselt eveneens tussen starten en stoppen.

### Stap 14 — Kalibreer de sensoren

Zet de robot stil. Plaats de sensorarray boven de zwarte lijn en druk op **Meet zwart**. Plaats daarna de array boven de witte achtergrond en druk op **Meet wit**. De waarden worden opgeslagen in het geheugen van de Nano.

### Stap 15 — Test eerst met de wielen vrij

Zet de robot op een steun zodat de wielen vrij kunnen draaien. Start kort en controleer of de linker- en rechtermotor overeenkomen met de gewenste rijrichting. Druk op **Stop** en corrigeer een verkeerde motorrichting door de twee draden van die motor om te wisselen. Controleer ook op de webpagina dat de status verandert.

### Stap 16 — Test op het parcours

Zet de robot op het parcours met de sensoren boven de lijn. Start via de webpagina of D2. Gebruik **Stop** om de test te beëindigen.

## Instellingen aanpassen

De webpagina laat de volgende parameters instellen:

| Parameter | Betekenis |
|---|---|
| CycleTime | Regelperiode in microseconden |
| Power | Basissnelheid, 0–255 |
| Diff | Correctieverschil tussen de motoren |
| Kp | Proportionele regelparameter |
| Ki | Integrale regelparameter |
| Kd | Differentiële regelparameter |

De robot stopt wanneer instellingen worden opgeslagen. Pas telkens één waarde aan en test opnieuw. De instellingen en sensorcalibratie worden opgeslagen en blijven na een herstart bewaard.

## Bekende uitgangspunten

- Deze handleiding is gebaseerd op de huidige `LineFollowerV4.ino` en de productinformatie die voor de motor, accu en lader is vastgesteld.
- De concrete accuschakeling, spanningsregelaarinstelling, fysieke montageposities en pinvolgorde op de specifieke DRV8833-module moeten overeenkomen met de technische tekening van de gebouwde robot.
- De twee XTAR MC1-laadcontacten zijn niet bedoeld als laadoplossing voor een in serie geschakeld accupakket.
