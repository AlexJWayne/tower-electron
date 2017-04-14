#include "hsv.h"
#include "dotstar.h"

#define NUM_LEDS 10
#define NUM_STRIPS 10
#define LED_BRIGHTNESS 127

#define LED_PWR D7
#define LED_DAT C0
#define LED_CLK1 C1
#define LED_CLK2 C2
#define LED_CLK3 C3
#define LED_CLK4 C4
#define LED_CLK5 C5
#define LED_CLK6 D0
#define LED_CLK7 D1
#define LED_CLK8 D2
#define LED_CLK9 D3
#define LED_CLK10 D4

Adafruit_DotStar strips[] = {
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK1),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK2),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK3),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK4),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK5),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK6),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK7),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK8),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK9),
  Adafruit_DotStar(NUM_LEDS, LED_DAT, LED_CLK10)
};

uint8_t mode = 0;
bool ledPower = false;

void setup() {
  Serial.begin(9600);

  // Expose cloud functions.
  Particle.function("TurnOff", turnOff);

  // Every pattern should register a start funtion here.
  Particle.function("Rainbow", rainbow_start);
  Particle.function("Twinkles", twinkles_start);

  // Initialize LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    strips[i].begin();
    strips[i].setBrightness(LED_BRIGHTNESS);
    strips[i].show();
  }

  // Decalre LED pwer control as output
  pinMode(LED_PWR, OUTPUT);

  // Give everying a moment.
  delay(100);

  // Ensure all LEDs are off to start.
  turnOffLEDs();
}

// Simply animate the current mode.
void loop() {
  switch (mode) {
    case 0:
      break;

    case 1:
      rainbow();
      break;

    case 2:
      twinkles();
      break;
  }
}

// Cuts power to LEDs via the N-Channel MOSFET.
void turnOffLEDs() {
  mode = 0;
  // Send black to make sure the LEDS are cleanly off.
  for (uint8_t stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      strips[stripIdx].setPixelColor(i, 0x000000);
    }
    strips[stripIdx].show();
  }

  ledPower = false;
  digitalWrite(LED_PWR, LOW); // turn off the N-Channel transistor switch.

  // Set all LED data and clock pins to HIGH so these pins can't be used as GND by the LEDs.
  digitalWrite(LED_DAT, HIGH);
  digitalWrite(LED_CLK1, HIGH);
  digitalWrite(LED_CLK2, HIGH);
  digitalWrite(LED_CLK3, HIGH);
  digitalWrite(LED_CLK4, HIGH);
  digitalWrite(LED_CLK5, HIGH);
  digitalWrite(LED_CLK6, HIGH);
  digitalWrite(LED_CLK7, HIGH);
  digitalWrite(LED_CLK8, HIGH);
  digitalWrite(LED_CLK9, HIGH);
  digitalWrite(LED_CLK10, HIGH);
}

// Cloud exposable version of turnOffLEDs()
int turnOff(String arg) {
  turnOffLEDs();
  return 1;
}

// Enables all LED by turning on the N-Channel MOSFET and connecting the LEDs to GND.
void turnOnLEDs() {
  ledPower = true;
  digitalWrite(LED_PWR, HIGH); // turn on the N-Channel transistor switch.
  delay(50); // Give the LEDs a moment to power up.
}



// =============
// = PATTERNS! =
// =============

// All pattern code and variables should be prefixed with their name and an underscore.
// There should be a `int pattern_name_start(String arg)` function for every pattern that
// is exposed in `setup()` via `Particle.function()`. The start function should simply
// set the `mode`, call `turnOnLEDs()` and `return 1`, as well as what other
// initialization the pattern may require.



// -----------
// - RAINBOW -

uint8_t rainbow_speed = 100;
uint16_t rainbow_hue = 0;

int rainbow_start(String arg) {
  mode = 1;
  turnOnLEDs();
  return 1;
}

void rainbow() {
  if (!ledPower) return;

  rainbow_hue += rainbow_speed;
  uint8_t hue = rainbow_hue >> 8;

  for (uint8_t stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      uint8_t hueOffset = (stripIdx * NUM_LEDS + i) * 255 / (NUM_STRIPS * NUM_LEDS);
      RgbColor rgb = HsvToRgb(HsvColor(hue + hueOffset, 255, 255));
      strips[stripIdx].setPixelColor(i, rgb.r, rgb.g, rgb.b);
    }
  }

  // Show all strips.
  for (uint8_t stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
    strips[stripIdx].show();
  }
}



// ------------
// - Twinkles -

uint8_t twinkles_brightness[NUM_STRIPS * NUM_LEDS];
bool twinkles_turningOn[NUM_STRIPS * NUM_LEDS];

int twinkles_start(String arg) {
  mode = 2;
  turnOnLEDs();
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    twinkles_brightness[i] = 0;
    twinkles_turningOn[i] = false;
  }
  return 1;
}

void twinkles() {
  for (uint8_t stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      uint ledIdx = stripIdx * NUM_LEDS + i;

      // animate the twinkles!
      if (twinkles_brightness[ledIdx] > 0) {
        // The pixel is turning on, count up.
        if (twinkles_turningOn[ledIdx]) {
          twinkles_brightness[ledIdx]++;
          // If max brightness, then now start dimming it.
          if (twinkles_brightness[ledIdx] == 255) {
            twinkles_turningOn[ledIdx] = false;
          }

        // The pixel is turning off, count down.
        } else if (twinkles_brightness[ledIdx] > 0) {
          twinkles_brightness[ledIdx]--;
        }
      }

      // See if we can start new twinkles now.

      // If the pixel is off.
      if (twinkles_brightness[ledIdx] == 0) {

        // if a rare random event occurred.
        if (random(100000) <= 100) {

          // Then start to turn on the pixel.
          twinkles_brightness[ledIdx] = 1;
          twinkles_turningOn[ledIdx] = true;
        }
      }
    }
  }

  for (uint8_t stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      uint8_t brightness = twinkles_brightness[stripIdx * NUM_STRIPS + i];
      strips[stripIdx].setPixelColor(i, brightness, brightness, brightness);
    }
    strips[stripIdx].show();
  }
}
