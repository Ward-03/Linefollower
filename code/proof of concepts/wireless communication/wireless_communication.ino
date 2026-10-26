#include <WiFi.h>
#include <WebServer.h>

// De Nano ESP32 maakt zelf een wifi-netwerk.
const char* WIFI_SSID = "LineFollower-Ward";
const char* WIFI_PASSWORD = "12345678"; // Minimaal 8 tekens

WebServer server(80);

void toonStartpagina() {
  String pagina = R"HTML(
<!doctype html>
<html lang="nl">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Nano ESP32 wifi-test</title>
  <style>
    body{font-family:system-ui,sans-serif;max-width:520px;margin:2rem auto;padding:0 1rem;background:#f4f6f8;color:#17202a}
    main{background:white;padding:1.25rem;border-radius:14px;box-shadow:0 3px 16px #0001}
  </style>
</head>
<body>
  <main>
    <h1>Wifi werkt</h1>
    <p>Je telefoon of computer is verbonden met de Arduino Nano ESP32.</p>
    <p>Dit is alleen een draadloze verbindingstest; er is geen motor- of sensorcode actief.</p>
  </main>
</body>
</html>
)HTML";

  server.send(200, "text/html; charset=utf-8", pagina);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Wifi-toegangspunt starten is mislukt.");
    while (true) {
      delay(1000);
    }
  }

  server.on("/", HTTP_GET, toonStartpagina);
  server.begin();

  Serial.println("Wifi-toegangspunt gestart.");
  Serial.print("Netwerknaam: ");
  Serial.println(WIFI_SSID);
  Serial.print("Open in je browser: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
