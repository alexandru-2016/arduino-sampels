#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    17

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 8

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(200);
}

void loop() {
  for (int i = 0; i < 256; i++) {
    setAllLeds(0, i, 0);
    strip.show();
//    Serial.println(i);
    delay(3);
  }

  for (int i = 254; i >= 0; i--) {
    setAllLeds(0, i, 0);
    strip.show();
//    Serial.println(i);
    delay(3);
  }
  
  delay(2000);
}

void setAllLeds(int r, int g, int b) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}
