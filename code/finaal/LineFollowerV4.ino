#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <math.h>

// De Nano maakt zelf een wifi-netwerk. Pas deze gegevens naar wens aan.
const char* WIFI_SSID = "LineFollower-Ward";
const char* WIFI_PASSWORD = "12345678"; // Minimaal 8 tekens

WebServer server(80);
Preferences preferences;

// Pinout van de line follower
#define MotorLeft1 5   // Fysiek linkermotor: DRV8833 IN3 -> D5 (vooruit)
#define MotorLeft2 3   // Fysiek linkermotor: DRV8833 IN4 -> D3 (achteruit)
#define MotorRight1 6  // Fysiek rechtermotor: DRV8833 IN2 -> D6 (vooruit)
#define MotorRight2 9  // Fysiek rechtermotor: DRV8833 IN1 -> D9 (achteruit)
const int SleepPin = 4; // DRV8833 SLEEP -> D4
const int Sensoren[] = {A7, A6, A5, A4, A3, A2, A1, A0};
const int StartKnop = 2;
const int LedBlauw = 12;
const int LedSensor = 13;

int normalised[8];
volatile bool run = false;
float Error = 0;
float LastError = 0;
float Position = 3.5f;
float Output = 0;
float iTerm = 0;
unsigned long Current = 0;
unsigned long Previous = 0;
unsigned long Difference = 0;
unsigned long CalculationTime = 0;
float PowerLeft = 0;
float PowerRight = 0;

struct Parameters_t {
  unsigned long CycleTime; // microseconden
  int Black[8];
  int White[8];
  int Power;
  float Diff;
  float Kp;
  float Ki; // interne waarde, zoals in het oorspronkelijke programma
  float Kd; // interne waarde, zoals in het oorspronkelijke programma
} Parameters;

void laadInstellingen() {
  Parameters.CycleTime = preferences.getULong("cycle", 10000);
  Parameters.Power = preferences.getInt("power", 120);
  Parameters.Diff = preferences.getFloat("diff", 0.0f);
  Parameters.Kp = preferences.getFloat("kp", 1.0f);
  Parameters.Ki = preferences.getFloat("ki", 0.0f);
  Parameters.Kd = preferences.getFloat("kd", 0.0f);
  preferences.getBytes("black", Parameters.Black, sizeof(Parameters.Black));
  preferences.getBytes("white", Parameters.White, sizeof(Parameters.White));

  // Eerste start: bruikbare standaardwaarden voor 12-bit analoge metingen.
  if (!preferences.isKey("black")) {
    for (int i = 0; i < 8; i++) Parameters.Black[i] = 0;
  }
  if (!preferences.isKey("white")) {
    for (int i = 0; i < 8; i++) Parameters.White[i] = 4095;
  }
  Parameters.CycleTime = constrain(Parameters.CycleTime, 1000UL, 1000000UL);
  Parameters.Power = constrain(Parameters.Power, 0, 255);
}

void bewaarInstellingen() {
  preferences.putULong("cycle", Parameters.CycleTime);
  preferences.putInt("power", Parameters.Power);
  preferences.putFloat("diff", Parameters.Diff);
  preferences.putFloat("kp", Parameters.Kp);
  preferences.putFloat("ki", Parameters.Ki);
  preferences.putFloat("kd", Parameters.Kd);
  preferences.putBytes("black", Parameters.Black, sizeof(Parameters.Black));
  preferences.putBytes("white", Parameters.White, sizeof(Parameters.White));
}

float getKigu() {
  return Parameters.Ki / (Parameters.CycleTime / 1000000.0f);
}

float getKdgu() {
  return Parameters.Kd * (Parameters.CycleTime / 1000000.0f);
}

String maakPagina() {
  String html = R"HTML(<!doctype html>
<html lang="nl"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Line follower instellingen</title>
<style>
body{font-family:system-ui,sans-serif;max-width:620px;margin:2rem auto;padding:0 1rem;background:#f4f6f8;color:#17202a}
main{background:white;padding:1.25rem;border-radius:14px;box-shadow:0 3px 16px #0001}
label{display:block;margin:1rem 0 .35rem;font-weight:600}
input[type=number]{box-sizing:border-box;width:100%;padding:.7rem;font-size:1rem}
button{margin:.6rem .35rem .2rem 0;padding:.75rem 1rem;font-size:1rem;background:#1264a3;color:white;border:0;border-radius:8px}
.stop{background:#a32929}.cal{background:#52616b}.status{padding:.75rem;background:#eef6ff;border-radius:8px}
.hint{font-size:.92rem;color:#46515c}
</style></head><body><main>
<h1>Line follower</h1>
<p class="status">Robotstatus: <strong>)HTML";
  html += run ? "rijdt" : "gestopt";
  html += R"HTML(</strong> &nbsp;|&nbsp; IP-adres: 192.168.4.1</p>
<form action="/instellen" method="get">
<label for="cycle">CycleTime (microseconden)</label>
<input id="cycle" name="cycle" type="number" min="1000" max="1000000" step="100" value=")HTML";
  html += String(Parameters.CycleTime);
  html += R"HTML(">
<label for="power">Power (0–255)</label>
<input id="power" name="power" type="number" min="0" max="255" value=")HTML";
  html += String(Parameters.Power);
  html += R"HTML(">
<label for="diff">Diff</label>
<input id="diff" name="diff" type="number" step="0.01" value=")HTML";
  html += String(Parameters.Diff, 3);
  html += R"HTML(">
<label for="kp">Kp</label>
<input id="kp" name="kp" type="number" step="0.01" value=")HTML";
  html += String(Parameters.Kp, 3);
  html += R"HTML(">
<label for="ki">Ki</label>
<input id="ki" name="ki" type="number" step="0.01" value=")HTML";
  html += String(getKigu(), 4);
  html += R"HTML(">
<label for="kd">Kd</label>
<input id="kd" name="kd" type="number" step="0.01" value=")HTML";
  html += String(getKdgu(), 4);
  html += R"HTML(">
<button type="submit">Parameters opslaan</button>
</form>
<form action="/run" method="get"><button type="submit">Start</button></form>
<form action="/stop" method="get"><button class="stop" type="submit">Stop</button></form>
<h2>Sensorcalibratie</h2>
<p class="hint">Plaats de sensoren op de juiste ondergrond voordat je calibreert.</p>
<form action="/calibrate" method="get">
<button class="cal" name="kleur" value="black" type="submit">Meet zwart</button>
<button class="cal" name="kleur" value="white" type="submit">Meet wit</button>
</form>
<p class="hint">Parameters en calibratie worden opgeslagen en blijven na een herstart bewaard.</p>
</main></body></html>)HTML";
  return html;
}

void toonStartpagina() {
  server.send(200, "text/html; charset=utf-8", maakPagina());
}

void stelIn() {
  // Ook de robot laten stoppen zodra instellingen worden aangepast.
  run = false;

  if (server.hasArg("cycle")) {
    unsigned long cycleNieuw = strtoul(server.arg("cycle").c_str(), nullptr, 10);
    Parameters.CycleTime = constrain(cycleNieuw, 1000UL, 1000000UL);
  }
  if (server.hasArg("power")) {
    Parameters.Power = constrain(server.arg("power").toInt(), 0, 255);
  }
  if (server.hasArg("diff")) Parameters.Diff = server.arg("diff").toFloat();
  if (server.hasArg("kp")) Parameters.Kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) {
    Parameters.Ki = server.arg("ki").toFloat() * (Parameters.CycleTime / 1000000.0f);
  }
  if (server.hasArg("kd")) {
    Parameters.Kd = server.arg("kd").toFloat() / (Parameters.CycleTime / 1000000.0f);
  }
  bewaarInstellingen();
  toonStartpagina();
}

void startRobot() {
  run = true;
  toonStartpagina();
}

void stopMotoren() {
  PowerLeft = 0;
  PowerRight = 0;
  analogWrite(MotorLeft1, 0);
  analogWrite(MotorLeft2, 0);
  analogWrite(MotorRight1, 0);
  analogWrite(MotorRight2, 0);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, LOW);
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, LOW);
}

void stopRobot() {
  run = false;
  stopMotoren();
  toonStartpagina();
}

void calibreerSensoren() {
  run = false;
  if (server.arg("kleur") == "black") {
    for (int i = 0; i < 8; i++) Parameters.Black[i] = analogRead(Sensoren[i]);
  } else if (server.arg("kleur") == "white") {
    for (int i = 0; i < 8; i++) Parameters.White[i] = analogRead(Sensoren[i]);
  }
  bewaarInstellingen();
  toonStartpagina();
}

void Interrupt() {
  static unsigned long laatsteInterrupt = 0;
  unsigned long interruptTijd = millis();
  if (interruptTijd - laatsteInterrupt > 500) {
    run = !run;
    laatsteInterrupt = interruptTijd;
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  preferences.begin("linefollower", false);
  laadInstellingen();

  for (int i = 0; i < 8; i++) pinMode(Sensoren[i], INPUT);
  pinMode(LedSensor, OUTPUT);
  digitalWrite(LedSensor, HIGH);
  pinMode(StartKnop, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(StartKnop), Interrupt, FALLING);
  pinMode(LedBlauw, OUTPUT);
  pinMode(MotorLeft1, OUTPUT);
  pinMode(MotorLeft2, OUTPUT);
  pinMode(MotorRight1, OUTPUT);
  pinMode(MotorRight2, OUTPUT);
  pinMode(SleepPin, OUTPUT);
  digitalWrite(SleepPin, HIGH);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Wifi-toegangspunt starten is mislukt.");
    while (true) delay(1000);
  }

  server.on("/", HTTP_GET, toonStartpagina);
  server.on("/instellen", HTTP_GET, stelIn);
  server.on("/run", HTTP_GET, startRobot);
  server.on("/stop", HTTP_GET, stopRobot);
  server.on("/calibrate", HTTP_GET, calibreerSensoren);
  server.begin();

  Serial.println("Wifi-netwerk gestart.");
  Serial.print("Verbind met: ");
  Serial.println(WIFI_SSID);
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();

  if (!run) {
    stopMotoren();
    iTerm = 0;
    LastError = 0;
    digitalWrite(LedBlauw, LOW);
    return; // Bij stop geen verdere berekeningen of motorcommando's uitvoeren.
  }

  Current = micros();
  digitalWrite(LedBlauw, HIGH);

  if (Current - Previous >= Parameters.CycleTime) {
    Previous = Current;
    for (int i = 0; i < 8; i++) {
      // Deze sensoren geven bij zwart een hogere ADC-waarde dan bij wit.
      // Zet zwart daarom om naar 1000 (lijn) en wit naar 0 (achtergrond).
      normalised[i] = constrain(
        map(analogRead(Sensoren[i]), Parameters.White[i], Parameters.Black[i], 0, 1000),
        0, 1000
      );
    }

    float weightedSum = 0;
    float sumVal = 0;
    int zwarteSensoren = 0;
    for (int i = 0; i < 8; i++) {
      if (normalised[i] > 100) {
        zwarteSensoren++;
        weightedSum += (float)normalised[i] * i;
        sumVal += normalised[i];
      }
    }
    if (sumVal > 0) Position = weightedSum / sumVal;

    Error = Position - 3.5f;
    Output = Error * Parameters.Kp;
    iTerm = 0;
    iTerm += Parameters.Ki * Error;
    iTerm = constrain(iTerm, -510.0f, 510.0f);
    Output += iTerm;
    Output += Parameters.Kd * (Error - LastError);
    LastError = Error;
    Output = constrain(Output, -510.0f, 510.0f);

    int sensorTotaal = 0;
    for (int i = 0; i < 8; i++) sensorTotaal += normalised[i];
    if (sensorTotaal < 200) Output = 0;

    // Op een breed kruispunt kan het gewogen midden naar een zijtak trekken.
    // Houd dan beide motoren gelijk aan om rechtdoor over het kruispunt te rijden.
    if (zwarteSensoren >= 6) Output = 0;

    if (run) {
      if (Output >= 0) {
        PowerLeft = constrain(Parameters.Power + (Parameters.Diff * Output), -255.0f, 255.0f);
        PowerRight = constrain(PowerLeft - Output, -255.0f, 255.0f);
        PowerLeft = PowerRight + Output;
      } else {
        PowerRight = constrain(Parameters.Power - (Parameters.Diff * Output), -255.0f, 255.0f);
        PowerLeft = constrain(PowerRight + Output, -255.0f, 255.0f);
        PowerRight = PowerLeft - Output;
      }

      if (PowerLeft > 0) {
        digitalWrite(MotorLeft1, LOW);
        analogWrite(MotorLeft2, (int)PowerLeft);
      } else {
        analogWrite(MotorLeft1, (int)-PowerLeft);
        digitalWrite(MotorLeft2, LOW);
      }
      if (PowerRight > 0) {
        digitalWrite(MotorRight1, LOW);
        analogWrite(MotorRight2, (int)PowerRight);
      } else {
        analogWrite(MotorRight1, (int)-PowerRight);
        digitalWrite(MotorRight2, LOW);
      }
    }
  }

  Difference = micros() - Current;
  if (Difference > CalculationTime) CalculationTime = Difference;
}

