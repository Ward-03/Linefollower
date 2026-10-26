// Standalone test voor de acht analoge lijnsensoren.
// Bevat alleen sensoruitlezing; geen wifi-, motor- of PID-code.

// Volgorde uit de oorspronkelijke line-followercode:
// index 0 t/m 7 komt overeen met A7 t/m A0.
const int Sensoren[] = {A7, A6, A5, A4, A3, A2, A1, A0};
const int AantalSensoren = sizeof(Sensoren) / sizeof(Sensoren[0]);

int sensorWaarden[AantalSensoren];

void leesSensoren() {
  for (int i = 0; i < AantalSensoren; i++) {
    sensorWaarden[i] = analogRead(Sensoren[i]);
  }
}

void printSensoren() {
  for (int i = 0; i < AantalSensoren; i++) {
    Serial.print("S");
    Serial.print(i);
    Serial.print(" (A");
    Serial.print(7 - i);
    Serial.print(")=");
    Serial.print(sensorWaarden[i]);
    if (i < AantalSensoren - 1) Serial.print("  ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Nano ESP32: waarden van 0 tot 4095

  for (int i = 0; i < AantalSensoren; i++) {
    pinMode(Sensoren[i], INPUT);
  }

  Serial.println("Sensorentest gestart. Open de seriele monitor op 115200 baud.");
}

void loop() {
  leesSensoren();
  printSensoren();
  delay(200);
}
