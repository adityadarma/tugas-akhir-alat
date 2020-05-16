#include "ACS712.h"

ACS712 currentSensor(ACS712_05B, A0);

void setup()
{
  Serial.begin(9600);
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  delay(100);
  currentSensor.calibrate();
  Serial.println("Done!");
}

void loop()
{
  float U = 220;
  float I = currentSensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  float P = U * I;

  Serial.println(String("U = ") + U + " V");
  Serial.println(String("I = ") + I + " A");
  Serial.println(String("P = ") + P + " Watts");

  delay(1000);
}
