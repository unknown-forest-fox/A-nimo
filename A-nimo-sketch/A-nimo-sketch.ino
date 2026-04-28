#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FluxGarage_RoboEyes.h>
#include <Adafruit_SSD1306.h>

const char* ssid     = "RUC-IOT";
const char* password = "GiHa5934La";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> eyes(display1);

ESP8266WebServer server(80);

bool isAngry = false;
bool lastAngry = false;
unsigned long lastEyeUpdate = 0;

// ─── Handlers ─────────────────────────────────────────────────
void handleAngry() {
  isAngry = true;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "ACK: angry");
  Serial.println("Received: angry");
}

void handleHappy() {
  isAngry = false;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "ACK: happy");
  Serial.println("Received: happy");
}

void handleNotFound() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/plain", "Not found");
}

// ─── SETUP ────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
  }
  display1.clearDisplay();
  display1.display();

  eyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  eyes.setAutoblinker(ON, 3, 2);
  eyes.setIdleMode(ON, 3, 2);
  eyes.setMood(HAPPY);

  // ─── WiFi ───────────────────────────────────────────────────
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    eyes.update();
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  
  server.on("/angry", handleAngry);
  server.on("/happy", handleHappy);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Server: http://" + WiFi.localIP().toString());
}

// ─── LOOP ─────────────────────────────────────────────────────
void loop() {
  server.handleClient();
  yield();

  // only change mood when state actually changes
  if (isAngry != lastAngry) {
    lastAngry = isAngry;
    if (isAngry) {
      eyes.setMood(ANGRY);
      eyes.setAutoblinker(ON, 3, 2);
      eyes.setIdleMode(ON, 3, 2);
    } else {
      display1.clearDisplay();
      display1.display();
      eyes.setMood(HAPPY);
      eyes.setAutoblinker(ON, 3, 2);
      eyes.setIdleMode(ON, 3, 2);
    }
  }

  // update eyes every 16ms (~60fps)
  unsigned long now = millis();
  if (now - lastEyeUpdate >= 16) {
    lastEyeUpdate = now;
    server.handleClient();
    yield();
    eyes.update();
  }
}
