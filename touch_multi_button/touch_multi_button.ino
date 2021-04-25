#include <MultiButton.h>

// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.

void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
}

MultiButton btn = MultiButton();

void loop()
{
  int value = touchRead(T0);
  btn.update(value < 40);

  if (btn.isClick()) {
    Serial.println();
    Serial.println();
    Serial.println("click");
    Serial.println();
    Serial.println();
  }
  delay(10);
}
