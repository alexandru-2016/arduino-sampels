#include <Wire.h>
#include <Adafruit_NeoPixel.h>

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

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


uint32_t buffer[] = { 10 , 10 , 10 , 10 , 10 , 10 , 10 , 10};

void loop()
{
  sample = analogRead(32);
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(32);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;

  Serial.print("max: ");
  Serial.print(signalMax);
  Serial.print(" min: ");
  Serial.print(signalMin);
  Serial.println();

  // map 1v p-p level to the max scale of the display
  int displayValue = map(signalMax, 0, 1023, 0, maxScale);

  for (int i = 0; i < 7; i++) {
    setColor(i, buffer[i]);
    buffer[i] = buffer[i + 1];
  }

  setColor(7, displayValue);
  buffer[7] = displayValue;
  Serial.println(displayValue);

  strip.show();
}

void setColor(int pixel, uint32_t displayPeak) {
  strip.setPixelColor(pixel, strip.Color(0, displayPeak * 16, 0));
}
