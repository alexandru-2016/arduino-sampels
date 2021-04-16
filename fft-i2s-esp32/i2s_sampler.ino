#include <driver/i2s.h>

#define I2S_PORT I2S_NUM_0

// i2s config for reading from both m5stack mic
i2s_config_t i2sConfig = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLING_FREQ,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 2,
  .dma_buf_len = 1024,
};

// i2s pins
i2s_pin_config_t i2sPins = {
  .bck_io_num = GPIO_NUM_32,
  .ws_io_num = GPIO_NUM_25,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = GPIO_NUM_33
};

// i2s reader queue
QueueHandle_t i2sQueue;

// current position in the audio buffer
int32_t audioBufferPos = 0;

// current audio buffer
int16_t *currentAudioBuffer;

// buffer containing samples that have been captured already
int16_t *capturedAudioBuffer;

void addSample(int32_t sample)
{
  // add the sample to the current audio buffer
  currentAudioBuffer[audioBufferPos] = sample >> 16;
  audioBufferPos++;
  // have we filled the buffer with data?
  if (audioBufferPos == SAMPLES)
  {
    // swap to the other buffer
    std::swap(currentAudioBuffer, capturedAudioBuffer);
    // reset the buffer position
    audioBufferPos = 0;

//    Serial.print("Sampler running on core ");
//    Serial.println(xPortGetCoreID());

    doFFT(capturedAudioBuffer);
  }
}

void i2sReaderTask(void * parameter)
{
  while (true)
  {
    // wait for some data to arrive on the queue
    i2s_event_t evt;
    if (xQueueReceive(i2sQueue, &evt, portMAX_DELAY) == pdPASS)
    {
      if (evt.type == I2S_EVENT_RX_DONE)
      {
        size_t bytesRead = 0;
        do
        {
          // read data from the I2S peripheral
          int32_t i2sData[1024];
          // read from i2s
          i2s_read(I2S_PORT, i2sData, 4096, &bytesRead, 10);
          for (int i = 0; i < bytesRead / 4; i++)
          {
            addSample(i2sData[i] & 0xfffffff0);
          }
        } while (bytesRead > 0);
      }
    }
  }
}

void start_i2s() {
  currentAudioBuffer = (int16_t *)malloc(SAMPLES * sizeof(int16_t));
  capturedAudioBuffer = (int16_t *)malloc(SAMPLES * sizeof(int16_t));

  //install and start i2s driver
  i2s_driver_install(I2S_PORT, &i2sConfig, 4, &i2sQueue);
  i2s_set_pin(I2S_NUM_0, &i2sPins);
  i2s_set_clk(I2S_NUM_0, i2sConfig.sample_rate, i2sConfig.bits_per_sample, I2S_CHANNEL_MONO);

  TaskHandle_t readerTaskHandle;
  xTaskCreatePinnedToCore(i2sReaderTask, "i2s Reader Task", 8192, NULL, 1, &readerTaskHandle, 0);
}
