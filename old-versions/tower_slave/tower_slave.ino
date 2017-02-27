#include <FastLED.h>
#include <SPI.h>

// LED Setup
#define LED_DAT_PIN 6
#define LED_CLK_PIN 7
#define COLOR_ORDER GBR
#define CHIPSET     APA102
#define NUM_LEDS    60
#define BRIGHTNESS  64

CRGB leds[NUM_LEDS];
volatile uint8_t buf[NUM_LEDS * 3];
volatile uint8_t dataIdx = 0;
volatile bool showFrame = true;


void setup() {
  Serial.begin(115200); // debugging

  // Setup LEDs.
  FastLED.addLeds<CHIPSET, LED_DAT_PIN, LED_CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS); //.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.showColor(CRGB(0,0,0));

  // turn on SPI in slave mode
  SPCR |= bit(SPE);

  // SPI Communication pins
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SS, INPUT);

  // Handle SPI with interrupts
  SPI.attachInterrupt();
}

void loop() {
  if (showFrame) {
    showFrame = false;
    dataIdx = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].r = buf[i*3 + 0];
      leds[i].g = buf[i*3 + 1];
      leds[i].b = buf[i*3 + 2];
    }
  }
  FastLED.show();
}

// SPI interrupt handler.
ISR (SPI_STC_vect) {
  buf[dataIdx] = SPDR;
  dataIdx++;
  if (dataIdx >= NUM_LEDS * 3) {
    dataIdx = 0;
    showFrame = true;
  }
}
