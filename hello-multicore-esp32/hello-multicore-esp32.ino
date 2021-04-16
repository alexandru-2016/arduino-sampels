
TaskHandle_t Task0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    Task0code, /* Function to implement the task */
    "Task0", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    0,  /* Priority of the task */
    &Task0,  /* Task handle. */
    0); /* Core where the task should run */
}

void Task0code( void * parameter) {
  Serial.print("Task0 running on core ");

  for (;;) {
    Serial.print("Task0 running on core ");
    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}

void loop() {
  Serial.print("Task1 running on core ");

  for (;;) {
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}
