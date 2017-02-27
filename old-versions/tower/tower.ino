#include "FastLED.h"
#include <SPI.h>

// SPI Settings
#define CPU_CLOCK 12000000
#define CPU_DIVIDER 8

// LED Setup
#define NUM_LEDS     60
#define BRIGHTNESS   32
#define MS_PER_FRAME 1000/60
CRGB leds[NUM_LEDS];
unsigned long lastFrameSentAt = 0;

// Rangefinder
#define RANGE_PIN A0
#define RANGE_MIN 15
#define RANGE_MAX 244

void setup() {
  // Debugging
  Serial.begin(115200);

  // Setup SPI
  SPI.begin();

  // Setup Rangefinder
  pinMode(RANGE_PIN, INPUT);

  // Sanity setup delay.
  delay(1000);
}

void loop() {
  unsigned int cm = analogRead(RANGE_PIN);
  if (cm < RANGE_MIN) cm = RANGE_MIN;
  if (cm > RANGE_MAX) cm = RANGE_MAX;
  
  uint8_t count = map(cm, RANGE_MIN, RANGE_MAX, NUM_LEDS, 1);
  uint8_t hue = map(cm, RANGE_MIN, RANGE_MAX, 0, 170);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < count) {
      leds[i] = CHSV(hue, 255, 255);
    } else {
      leds[i] = CRGB::Black;
    }
  }

  sendFrameAtFPS();
}




// PIXEL DATA

void sendFrameAtFPS() {
  unsigned long now = millis();
  if (lastFrameSentAt + MS_PER_FRAME < now) {
    lastFrameSentAt = now;
    sendPixels();
  }
}

void sendPixels() {
  unsigned long x = micros();
  
  SPI.beginTransaction(SPISettings(CPU_CLOCK / CPU_DIVIDER, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < NUM_LEDS; i++) {
    SPI.transfer(leds[i].r);
    SPI.transfer(leds[i].g);
    SPI.transfer(leds[i].b);
  }
  SPI.endTransaction();

  Serial.println(micros() - x);
}
