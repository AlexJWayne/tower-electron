

#include "dotstar.h"

#define NUM_LEDS 10
#define LED_BRIGHTNESS 255
#define LED_DAT C0
#define LED_CLK1 C1
#define LED_CLK2 C2
#define LED_PWR D7

Adafruit_DotStar strip1 = Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK1);
Adafruit_DotStar strip2 = Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK2);

bool ledPower = false;

void setup() {
  Serial.begin(9600);

  strip1.begin();
  strip1.setBrightness(LED_BRIGHTNESS);
  strip1.show();

  strip2.begin();
  strip2.setBrightness(LED_BRIGHTNESS);
  strip2.show();

  pinMode(LED_PWR, OUTPUT);
}

void loop() {
  turnOnLEDs();
  showColors(2000);
  showColors(2000);
  showColors(2000);
  showColors(2000);
  showColors(2000);

  turnOffLEDs();
  delay(10000);
}

void turnOffLEDs() {
  // Send black to make sure the LEDS are cleanly off.
  fillColor1(0x000000);
  fillColor2(0x000000);

  ledPower = false;
  digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.

  // Set all LED pins to HIGH so these pins can't be used as GND for the LEDs.
  digitalWrite(LED_DAT, HIGH);
  digitalWrite(LED_CLK1, HIGH);
  digitalWrite(LED_CLK2, HIGH);
}

void turnOnLEDs() {
  ledPower = true;
  digitalWrite(LED_PWR, HIGH); // turn on the N-Channel transistor switch.
  delay(50); // Give the LEDs a moment to power up.
}



void showColors(uint time) {
  fillColor1(0xff0000);
  fillColor2(0x00ff00);
  delay(time / 3);

  fillColor1(0x00ff00);
  fillColor2(0x0000ff);
  delay(time / 3);

  fillColor1(0x0000ff);
  fillColor2(0xff0000);
  delay(time / 3);
}

void fillColor1(uint32_t color) {
  if (!ledPower) return;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    strip1.setPixelColor(i, color);
  }
  strip1.show();
}

void fillColor2(uint32_t color) {
  if (!ledPower) return;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    strip2.setPixelColor(i, color);
  }
  strip2.show();
}
