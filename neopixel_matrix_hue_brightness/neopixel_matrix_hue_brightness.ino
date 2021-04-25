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

int brightness[8][8] = {
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1}
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(150);
  
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      brightness[i][j] = 255;
    }
  }

  adjust(0);
}

void loop() {
//  for (int s = 0; s < 32; s++) {
//    adjust(-8);
//  }
//
//  for (int s = 0; s < 32; s++) {
//    adjust(8);
//  }
  delay(1000);
}

void adjust(int offset) {
//  Serial.println(brightness[0][0]);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int ledIndex = i * 8 + j;
      int pixelHue = j * 65536L / 8;

      int boundedBrightness = min(brightness[i][j], 256);
      boundedBrightness = max(brightness[i][j], 1);
      
      strip.setPixelColor(ledIndex, strip.gamma32(strip.ColorHSV(pixelHue, 255, boundedBrightness - 1)));
      brightness[i][j] += offset;
    }
  }
  strip.show();
  delay(20);
}
