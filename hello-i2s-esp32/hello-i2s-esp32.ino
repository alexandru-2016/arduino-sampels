
#include <driver/i2s.h>

TaskHandle_t Task0;

// approx 30ms of audio @ 16KHz
#define WINDOW_SIZE 512

// i2s config for reading from both m5stack mic
i2s_config_t i2sConfig = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = 16000,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 2,
  .dma_buf_len = 1024,
};

// i2s pins
i2s_pin_config_t i2sPins = {
  .bck_io_num = GPIO_NUM_25,
  .ws_io_num = GPIO_NUM_26,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = GPIO_NUM_27
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  start_i2s();
}

void loop() {
  Serial.print("Task1 running on core ");

  for (;;) {
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}
