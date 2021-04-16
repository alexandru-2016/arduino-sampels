#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>

const int maxScale = 16;
const int redZone = 5;

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//
// *** Led setup ***
//

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    17

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 8

#define AUDIO_IN_PIN 32

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//
// *** FFT setup ***
//

#define SAMPLES         1024          // Must be a power of 2
#define SAMPLING_FREQ   40000         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE       1          // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define NUM_BANDS       8            // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE           1200           // Used as a crude noise filter, values below this are ignored
#define TOP             255

unsigned int sampling_period_us;
byte peak[] = {0, 0, 0, 0, 0, 0, 0, 0}; // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0, 0, 0, 0, 0, 0, 0, 0};
int bandValues[] = {0, 0, 0, 0, 0, 0, 0, 0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setup()
{
//  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

uint32_t buffer[] = { 10 , 10 , 10 , 10 , 10 , 10 , 10 , 10};

void loop()
{
  // Reset bandValues[]
  for (int i = 0; i < NUM_BANDS; i++) {
    bandValues[i] = 0;
  }

  // Sample the audio pin
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us) {
      /* chill */
    }
  }

  // Compute FFT
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES / 2); i++) {    // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE) {                    // Add a crude noise filter

      //16 bands, 12kHz top band
      if (i <= 3  ) bandValues[0]  += (int)vReal[i];
      if (i > 3   && i <= 7  ) bandValues[1]  += (int)vReal[i];
      if (i > 7   && i <= 13  ) bandValues[2]  += (int)vReal[i];
      if (i > 13  && i <= 25 ) bandValues[3]  += (int)vReal[i];
      if (i > 25  && i <= 50 ) bandValues[4]  += (int)vReal[i];
      if (i > 50  && i <= 97 ) bandValues[5] += (int)vReal[i];
      if (i > 97  && i <= 189) bandValues[6] += (int)vReal[i];
      if (i > 189) bandValues[7] += (int)vReal[i];
    }
  }

  // Process the FFT data into bar heights
  for (byte band = 0; band < NUM_BANDS; band++) {

    // Scale the bars for the display
    int barHeight = bandValues[band] / AMPLITUDE;
//    if (barHeight > TOP) barHeight = TOP;

    // Small amount of averaging between frames
//    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

//    Serial.print(barHeight - oldBarHeights[band]);
//    Serial.print(" ");

    //strip.setPixelColor(band, strip.gamma32(strip.ColorHSV(bandValues[band])));
    setColor(band, barHeight - oldBarHeights[band]);      
    
    // Move peak up
    if (barHeight > peak[band]) {
      peak[band] = min(TOP, barHeight);
    }
    // Save oldBarHeights for averaging later
    oldBarHeights[band] = barHeight;
  }

//  Serial.println(" ");
  strip.show();
}

void setColor(int pixel, uint32_t value) {
  if (value <= 0) {
    strip.setPixelColor(pixel, strip.Color(0, 0, 0));    
  } else {
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(value)));
  }
}
