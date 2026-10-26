// Standalone test/sjabloon voor de DRV8833 H-brug.
// Bevat alleen de motor-driverbediening; geen wifi-, sensor- of PID-code.

// DRV8833 aansluitingen uit de line-followercode
const int MotorLeft1 = 3;
const int MotorLeft2 = 5;
const int MotorRight1 = 6;
const int MotorRight2 = 9;
const int SleepPin = 4; // nSLEEP: HIGH = driver ingeschakeld

// Snelheid: -255 = achteruit, 0 = stop, +255 = vooruit.
void stuurMotor(int pinVooruit, int pinAchteruit, int snelheid) {
  snelheid = constrain(snelheid, -255, 255);

  if (snelheid > 0) {
    analogWrite(pinAchteruit, 0);
    analogWrite(pinVooruit, snelheid);
  } else if (snelheid < 0) {
    analogWrite(pinVooruit, 0);
    analogWrite(pinAchteruit, -snelheid);
  } else {
    analogWrite(pinVooruit, 0);
    analogWrite(pinAchteruit, 0);
  }
}

void stuurMotoren(int snelheidLinks, int snelheidRechts) {
  stuurMotor(MotorLeft1, MotorLeft2, snelheidLinks);
  stuurMotor(MotorRight1, MotorRight2, snelheidRechts);
}

void stopMotoren() {
  stuurMotoren(0, 0);
}

void setup() {
  pinMode(MotorLeft1, OUTPUT);
  pinMode(MotorLeft2, OUTPUT);
  pinMode(MotorRight1, OUTPUT);
  pinMode(MotorRight2, OUTPUT);
  pinMode(SleepPin, OUTPUT);

  // Houd de driver eerst uit en zet alle motoruitgangen laag.
  digitalWrite(SleepPin, LOW);
  stopMotoren();
  digitalWrite(SleepPin, HIGH);
}

void loop() {
  // De motoren blijven uit totdat je hieronder een functie aanroept.
  // Voorbeeld: stuurMotoren(150, 150);  // beide motoren vooruit
  // Voorbeeld: stuurMotoren(-100, -100); // beide motoren achteruit
  // Voorbeeld: stuurMotoren(100, -100);  // ter plaatse draaien
  // Voorbeeld: stopMotoren();
}
