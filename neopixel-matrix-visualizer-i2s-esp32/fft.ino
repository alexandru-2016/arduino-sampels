#include <arduinoFFT.h>

#define NOISE           200           // Used as a crude noise filter, values below this are ignored

double vReal[SAMPLES];
double vImag[SAMPLES];

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void doFFT(int16_t *samples) {
  for (int i = 0; i < SAMPLES; i++)
  {
    vReal[i] = samples[i] / 30.0d;
    vImag[i] = 0;
  }

  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  for (int i = 0; i < BAND_COUNT; i++) {
    bandValues[i] = 0;
  }

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES / 2); i++) {    // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE) {                    // Add a crude noise filter

      // 30KHz freq, 512 samples, 12kHz top band, 8 bands
      if (i <= 2) bandValues[0] += (int)vReal[i];
      else if (i > 2 && i <= 4) bandValues[1] += (int)vReal[i];
      else if (i > 4 && i <= 9) bandValues[2] += (int)vReal[i];
      else if (i > 9 && i <= 18) bandValues[3] += (int)vReal[i];
      else if (i > 18 && i <= 36) bandValues[4] += (int)vReal[i];
      else if (i > 36 && i <= 75) bandValues[5] += (int)vReal[i];
      else if (i > 75 && i <= 152) bandValues[6] += (int)vReal[i];
      else if (i > 152) bandValues[7] += (int)vReal[i];
    }
  }
}
