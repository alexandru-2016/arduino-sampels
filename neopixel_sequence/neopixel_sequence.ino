#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    17

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 64

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(5);
}

int prevLed = LED_COUNT - 1;

void loop() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(prevLed, strip.Color(0, 0, 0));
    strip.setPixelColor(i, strip.Color(50, 50, 50));
    prevLed = i;
    strip.show();
    delay(500);
  }
}
