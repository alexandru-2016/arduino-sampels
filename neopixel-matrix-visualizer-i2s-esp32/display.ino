#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#include <FastLED.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    17

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 64

#define AMPLITUDE_SCALE 300

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int peak[BAND_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};
int oldDrawHeights[BAND_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};

// takes values from 1 to 256
int brightness[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

#define MODE_FALLING_PEAK_DOTS  1
#define MODE_FALLING_PEAK_BAR   2
#define MODE_PEAK_SNAPSHOT      3

int currentMode = MODE_PEAK_SNAPSHOT;

struct BrightnessInfo {
  int displayBrightness;
  int fadeRate;
};

BrightnessInfo maxBrightness = { 150, 8 };
BrightnessInfo mediumBrightness = { 80, 6 };
BrightnessInfo minBrightness = { 10, 4 };

BrightnessInfo currentBrightness = maxBrightness;

void setupDisplay() {
  // read persisted mode
  int oldMode = EEPROM.read(0);
  if (oldMode > MODE_PEAK_SNAPSHOT || oldMode < MODE_FALLING_PEAK_DOTS) {
    oldMode = MODE_PEAK_SNAPSHOT;
  }
  currentMode = oldMode;

  // read persisted brightness
  int oldBrightness = EEPROM.read(1);
  if (oldBrightness > 2) {
    oldBrightness = 2;
  }
  if (oldBrightness == 0) {
    currentBrightness = minBrightness;
  } else if (oldBrightness == 1) {
    currentBrightness = mediumBrightness;
  } else if (oldBrightness == 2) {
    currentBrightness = maxBrightness;
  }

  Serial.println("oldMode");
  Serial.println(oldMode);
  Serial.println("oldBrightness");
  Serial.println(oldBrightness);


  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(currentBrightness.displayBrightness);
}

void changeMode() {
  if (currentMode == MODE_FALLING_PEAK_DOTS) {
    currentMode = MODE_FALLING_PEAK_BAR;
  } else if (currentMode == MODE_FALLING_PEAK_BAR) {
    currentMode = MODE_PEAK_SNAPSHOT;
  } else if (currentMode == MODE_PEAK_SNAPSHOT) {
    currentMode = MODE_FALLING_PEAK_DOTS;
  }

  EEPROM.write(0, currentMode);
  EEPROM.commit();
}

void changeBrightness() {
  if (currentBrightness.displayBrightness == minBrightness.displayBrightness) {
    currentBrightness = mediumBrightness;
    EEPROM.write(1, 1);
  } else if (currentBrightness.displayBrightness == mediumBrightness.displayBrightness) {
    currentBrightness = maxBrightness;
    EEPROM.write(1, 2);
  } else if (currentBrightness.displayBrightness == maxBrightness.displayBrightness) {
    currentBrightness = minBrightness;
    EEPROM.write(1, 0);
  }

  EEPROM.commit();
  strip.setBrightness(currentBrightness.displayBrightness);
}

void updateDisplay(int bandValues[BAND_COUNT]) {
  for (int band = 0; band < BAND_COUNT; band++) {
    int drawHeight = bandValues[band] / AMPLITUDE_SCALE;
    drawHeight = min(drawHeight, BAND_HEIGHT);

    peak[band] = max(peak[band], drawHeight);

    for (int bandPixel = 0; bandPixel < BAND_HEIGHT; bandPixel++) {
      int pixelHue = bandPixel * 65536L / BAND_HEIGHT;
      int ledIndex = band * BAND_HEIGHT + bandPixel;

      if (currentMode == MODE_FALLING_PEAK_DOTS) {
        if (bandPixel <= drawHeight - 1 || bandPixel == peak[band] - 1) {
          brightness[band][bandPixel] = 255;
        } else {
          brightness[band][bandPixel] = 0;
        }
      }

      if (currentMode == MODE_FALLING_PEAK_BAR) {
        if (bandPixel <= drawHeight - 1) {
          brightness[band][bandPixel] = 255;
        } else if (bandPixel < peak[band] && brightness[band][bandPixel] > 0) {
          brightness[band][bandPixel] = max(brightness[band][bandPixel] - currentBrightness.fadeRate, 0);
        } else {
          brightness[band][bandPixel] = 0;
        }
      }

      if (currentMode == MODE_PEAK_SNAPSHOT) {
        if (bandPixel <= drawHeight - 1) {
          brightness[band][bandPixel] = 255;
        } else if (brightness[band][bandPixel] > 0) {
          brightness[band][bandPixel] = max(brightness[band][bandPixel] - currentBrightness.fadeRate, 0);
        } else {
          brightness[band][bandPixel] = 0;
        }
      }

      strip.setPixelColor(ledIndex, strip.gamma32(strip.ColorHSV(pixelHue, 255, brightness[band][bandPixel])));
    }
  }

  EVERY_N_MILLISECONDS(80) {
    for (int band = 0; band < BAND_COUNT; band++) {
      if (peak[band] > 0) {
        peak[band]--;
      }
    }
  }

  strip.show();
}
