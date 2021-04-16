// approx 30ms of audio @ 16KHz
// Must be a power of 2
#define SAMPLES 512

// Hz. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define SAMPLING_FREQ   16000         

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  start_i2s();
}

void loop() {
  for (;;) {
//    Serial.print("Task1 running on core ");
//    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}
