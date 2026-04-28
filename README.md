# A-nimo
> A smart desk companion that keeps you focused

A-nimo is a physical desk companion designed to help you stay focused while working or studying. It watches your attention through your laptop camera and reacts in real time — giving you a gentle (or not so gentle) nudge when your focus drifts.

## How it works
1. **Watch** — your webcam feeds into a machine learning model that detects whether you're focused or distracted
2. **Track** — a focus timer counts down when you're distracted and recovers when you're back on task
3. **React** — when focus drops too low, A-nimo's OLED eyes turn angry. Recover your focus and it goes back to happy

## System overview
| Component | Details |
|---|---|
| Frontend | GitHub Pages — p5.js + ml5.js, Teachable Machine model |
| Hardware | ESP8266 + 128×64 SSD1306 OLED, RoboEyes animations |
| Communication | HTTP over WiFi — browser sends `/angry` and `/happy` to the ESP |
