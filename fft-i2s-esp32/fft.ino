#include <arduinoFFT.h>

#define NOISE           500           // Used as a crude noise filter, values below this are ignored

double vReal[SAMPLES];
double vImag[SAMPLES];

int bandValues[] = {0, 0, 0, 0, 0, 0, 0, 0};

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void doFFT(int16_t *samples) {
  for (int i = 0; i < SAMPLES; i++)
  {
    vReal[i] = samples[i] / 30.0d;
    vImag[i] = 0;
    //    Serial.print(samples[i]);
    //    Serial.print(" ");
  }
  //  Serial.println();
  //  Serial.println();

  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  for (int i = 0; i < 8; i++) {
    bandValues[i] = 0;
  }

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES / 2); i++) {    // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE) {                    // Add a crude noise filter

      //8 bands, 12kHz top band
      if (i <= 3) bandValues[0] += (int)vReal[i];
      else if (i > 3 && i <= 6) bandValues[1] += (int)vReal[i];
      else if (i > 6 && i <= 13) bandValues[2] += (int)vReal[i];
      else if (i > 13 && i <= 27) bandValues[3] += (int)vReal[i];
      else if (i > 27 && i <= 55) bandValues[4] += (int)vReal[i];
      else if (i > 55 && i <= 112) bandValues[5] += (int)vReal[i];
      else if (i > 112 && i <= 229) bandValues[6] += (int)vReal[i];
      else if (i > 229) bandValues[7] += (int)vReal[i];
    }
  }

  for (int i = 0; i < 8; i++) {
    Serial.print(bandValues[i]);
    Serial.print(" ");
  }

  Serial.println();
}
