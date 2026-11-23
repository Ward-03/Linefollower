#include <WiFi.h>
#include <WebServer.h>

// Proof of concept: alleen wifi-communicatie en statusweergave.
// Geen motoren, sensoren of fysieke startknop aangestuurd.
const char* WIFI_SSID = "LineFollower-Ward";
const char* WIFI_PASSWORD = "12345678";

WebServer server(80);
bool robotRijdt = false;

const char pagina[] PROGMEM = R"HTML(
<!doctype html>
<html lang="nl">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Line follower communicatie PoC</title>
  <style>
    body { font-family: system-ui, sans-serif; max-width: 520px; margin: 2rem auto; padding: 0 1rem; background: #f4f6f8; color: #17202a; }
    main { background: white; padding: 1.25rem; border-radius: 14px; box-shadow: 0 3px 16px #0001; }
    button { padding: .8rem 1rem; margin: .4rem .3rem .4rem 0; border: 0; border-radius: 8px; color: white; background: #1264a3; font-size: 1rem; }
    button.stop { background: #a32929; }
    .status { padding: .8rem; background: #eef6ff; border-radius: 8px; }
    .hint { color: #46515c; font-size: .92rem; }
  </style>
</head>
<body>
  <main>
    <h1>Line follower PoC</h1>
    <p class="status">Status van ESP32: <strong id="status">verbinden...</strong></p>
    <button onclick="stuurCommando('/api/run')">Start</button>
    <button class="stop" onclick="stuurCommando('/api/stop')">Stop</button>
    <p class="hint">De gsm stuurt start/stop naar de ESP32. De pagina leest de status elke 500 ms terug.</p>
  </main>
  <script>
    async function leesStatus() {
      try {
        const antwoord = await fetch('/api/status', { cache: 'no-store' });
        const data = await antwoord.json();
        document.getElementById('status').textContent = data.rijdt ? 'RIJDT' : 'GESTOPT';
      } catch (fout) {
        document.getElementById('status').textContent = 'geen verbinding';
      }
    }

    async function stuurCommando(url) {
      try {
        await fetch(url, { method: 'POST' });
        await leesStatus();
      } catch (fout) {
        document.getElementById('status').textContent = 'geen verbinding';
      }
    }

    setInterval(leesStatus, 500);
    leesStatus();
  </script>
</body>
</html>
)HTML";

void toonPagina() {
  server.send(200, "text/html; charset=utf-8", pagina);
}

void toonStatus() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", robotRijdt ? "{\"rijdt\":true}" : "{\"rijdt\":false}");
}

void startRobot() {
  robotRijdt = true;
  toonStatus();
}

void stopRobot() {
  robotRijdt = false;
  toonStatus();
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Wifi-toegangspunt starten is mislukt.");
    while (true) delay(1000);
  }

  server.on("/", HTTP_GET, toonPagina);
  server.on("/api/status", HTTP_GET, toonStatus);
  server.on("/api/run", HTTP_POST, startRobot);
  server.on("/api/stop", HTTP_POST, stopRobot);
  server.begin();

  Serial.println("Communicatie-PoC gestart.");
  Serial.print("Wifi-netwerk: ");
  Serial.println(WIFI_SSID);
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
