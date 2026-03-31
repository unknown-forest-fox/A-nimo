
let socket;
let wsConnected = false;
let esp8266IP = "10.104.67.2"; // your ESP8266 IP

// ML5
let classifier;
let imageModelURL = "https://teachablemachine.withgoogle.com/models/QVSDb3EHD/";
let video;
let label = "Loading model...";

// timer
let timer = 10;
let sentAngry = false;

// timing control
let lastSecond = 0;
let youtubeOpened = false;

// ─── PRELOAD ─────────────────────────────
function preload() {
classifier = ml5.imageClassifier(imageModelURL + "model.json");
}

// ─── SETUP ───────────────────────────────
function setup() {
createCanvas(320, 260);

video = createCapture(VIDEO);
video.size(320, 240);
video.hide();



classifyVideo();
}


// ─── SEND TO ESP ─────────────────────────
function sendToESP(msg) {
  fetch(`http://${esp8266IP}/${msg}`)
    .then(res => res.text())
    .then(data => console.log("ESP replied:", data))
    .catch(err => console.error("Failed to reach ESP:", err));
}


// ─── DRAW ────────────────────────────────
function draw() {
background(0);

// camera
push();
translate(width, 0);
scale(-1, 1);
image(video, 0, 0);
pop();


// label
fill(255);
textAlign(CENTER);
textSize(16);
text(label, width/2, height - 4);

// timer bar
let barWidth = map(timer, 0, 10, 0, width);
fill(timer > 6 ? color(0,200,0) : color(255,50,50));
rect(0, height - 20, barWidth, 6);

textSize(12);
fill(255);
text("focus: " + timer, width/2, height - 22);

// run timer every second
if (millis() - lastSecond > 1000) {


if (label === "Distracted") {
  if (timer > 0) timer--;
} else {
  if (timer < 10) timer++;
}

lastSecond = millis();


}

// send messages
  if (timer === 0 && !sentAngry) {
    sendToESP("angry");
    sentAngry = true;
    if (!youtubeOpened) {
      youtubeOpened = true;
      window.location.href = "https://www.youtube.com/watch?v=6EEW-9NDM5k&list=RD6EEW-9NDM5k&start_radio=1";
    }
  }

  if (timer > 6 && sentAngry) {
    sendToESP("happy");
    sentAngry = false;
    youtubeOpened = false;
  }
}

// ─── CLASSIFICATION ──────────────────────
function classifyVideo() {
classifier.classify(video, gotResult);
}

function gotResult(error, results) {

if (error) {
console.error(error);
return;
}

label = results[0].label;

classifyVideo();
}
