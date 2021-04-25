#include <MultiButton.h>
#include <EEPROM.h>

// approx 30ms of audio @ 16KHz
// Must be a power of 2
#define SAMPLES 512

// Hz. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define SAMPLING_FREQ   30000

#define BAND_COUNT 8
#define BAND_HEIGHT 8

int bandValues[BAND_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};

TaskHandle_t fft_task_handle;
//TaskHandle_t ui_task_handle;

// buffer containing samples that have been captured already
int16_t *capturedAudioBuffer;

#define TOUCH_PIN_MODE T0
#define TOUCH_PIN_BRIGHTNESS T2

MultiButton btnMode = MultiButton();
MultiButton btnBrightness = MultiButton();

#define EEPROM_SIZE 2

void processFFTTask(void * parameter) {
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  while (true)
  {
    // use delay as workaround for error:
    // Task watchdog got triggered. The following tasks did not reset the watchdog in time
    delay(1);
    // wait for some samples to process
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      doFFT(capturedAudioBuffer);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  //  Serial.begin(115200);

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);

  xTaskCreatePinnedToCore(processFFTTask, "Processing Task", 4096, NULL, 2, &fft_task_handle, 0);

  start_i2s();
  setupDisplay();
}

int touchVal;

void loop() {
  delay(10);
  updateDisplay(bandValues);

  touchVal = touchRead(TOUCH_PIN_MODE);
  btnMode.update(touchVal < 50);

  if (btnMode.isClick()) {
    //    Serial.println("btnMode click");
    changeMode();
  }

  touchVal = touchRead(TOUCH_PIN_BRIGHTNESS);
  btnBrightness.update(touchVal < 50);

  if (btnBrightness.isClick()) {
    //    Serial.println("btnBrightness click");
    changeBrightness();
  }
}
