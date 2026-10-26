#include <Arduino.h>

// Losse drukknop- en interrupttest voor de Arduino Nano ESP32.
// Knop tussen D2 en GND; INPUT_PULLUP gebruikt de interne pull-upweerstand.
const int StartKnop = 2;

// De ISR telt alleen de interrupt. Debounce en start/stop gebeuren in loop().
volatile uint32_t knopInterrupts = 0;
uint32_t verwerkteInterrupts = 0;
unsigned long laatsteDrukTijd = 0;
bool run = false;

void ARDUINO_ISR_ATTR knopISR() {
  knopInterrupts++;
}

void setup() {
  Serial.begin(115200);
  pinMode(StartKnop, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(StartKnop), knopISR, FALLING);

  Serial.println("Drukknoptest gestart. Druk de knop om run te wisselen.");
}

void loop() {
  // Lees de teller veilig uit; de interrupt kan op elk moment plaatsvinden.
  noInterrupts();
  uint32_t huidigeInterrupts = knopInterrupts;
  interrupts();

  if (huidigeInterrupts != verwerkteInterrupts) {
    verwerkteInterrupts = huidigeInterrupts;
    unsigned long nu = millis();

    // Contactdender negeren: maximaal één druk per 50 ms verwerken.
    if (nu - laatsteDrukTijd >= 50) {
      laatsteDrukTijd = nu;
      run = !run;
      Serial.print("run = ");
      Serial.println(run ? "AAN" : "UIT");
    }
  }
}
