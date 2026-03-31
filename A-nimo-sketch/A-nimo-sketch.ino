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

void handleRoot() {
  String html = "";
  html += "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<title>Focus Tracker</title>";
  html += "<script src='https://cdnjs.cloudflare.com/ajax/libs/p5.js/1.9.0/p5.min.js'></scr";
  html += "ipt>";
  html += "<script src='https://unpkg.com/ml5@0.12.2/dist/ml5.min.js'></scr";
  html += "ipt>";
  html += "<style>body{margin:0;background:black;display:flex;justify-content:center;}</style>";
  html += "</head><body><scr";
  html += "ipt>";
  html += "let esp8266IP=location.hostname;";
  html += "let classifier,video,label='Loading...',timer=10,sentAngry=false,lastSecond=0;";
  html += "let imageModelURL='https://teachablemachine.withgoogle.com/models/QVSDb3EHD/';";
  html += "function preload(){classifier=ml5.imageClassifier(imageModelURL+'model.json');}";
  html += "function setup(){createCanvas(320,260);video=createCapture(VIDEO);video.size(320,240);video.hide();classifyVideo();}";
  html += "function sendToESP(msg){fetch('http://'+esp8266IP+'/'+msg).then(r=>r.text()).then(d=>console.log('ESP:',d)).catch(e=>console.error(e));}";
  html += "function draw(){";
  html += "background(0);push();translate(width,0);scale(-1,1);image(video,0,0);pop();";
  html += "fill(255);textAlign(CENTER);textSize(16);text(label,width/2,height-4);";
  html += "let bw=map(timer,0,10,0,width);";
  html += "fill(timer>6?color(0,200,0):color(255,50,50));rect(0,height-20,bw,6);";
  html += "textSize(12);fill(255);text('focus: '+timer,width/2,height-22);";
  html += "if(millis()-lastSecond>1000){";
  html += "if(label==='Distracted'){if(timer>0)timer--;}else{if(timer<10)timer++;}";
  html += "lastSecond=millis();}";
  html += "if(timer===0&&!sentAngry){sendToESP('angry');sentAngry=true;}";
  html += "if(timer>6&&sentAngry){sendToESP('happy');sentAngry=false;}}";
  html += "function classifyVideo(){classifier.classify(video,gotResult);}";
  html += "function gotResult(e,r){if(e){console.error(e);return;}label=r[0].label;classifyVideo();}";
  html += "</scr";
  html += "ipt></body></html>";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", html);
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

  // ─── Web Server ─────────────────────────────────────────────
  server.on("/", handleRoot);        // must be before server.begin()
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
      yield();
      eyes.setMood(ANGRY);
      eyes.setAutoblinker(OFF, 0, 0);
      eyes.setIdleMode(OFF, 0, 0);
      yield();
    } else {
      yield();
      eyes.setMood(HAPPY);
      eyes.setAutoblinker(ON, 3, 2);
      eyes.setIdleMode(ON, 3, 2);
      yield();
    }
  }

  // update eyes every 16ms (~60fps)
  unsigned long now = millis();
  if (now - lastEyeUpdate >= 16) {
    lastEyeUpdate = now;
    eyes.update();
  }
}