#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal_I2C.h>

Adafruit_ADS1115 ads(0x48);
LiquidCrystal_I2C lcd(0x27 ,16,2);
int pin_acs = 0;
int zero = 0;

void setup() 
{
  Serial.begin(115200);
  ads.begin();
  lcd.begin ();
  calibrate();
}

void loop() 
{
  lcd.setCursor(0, 0);
  lcd.print("Zero  : " + String(zero));
  lcd.setCursor(0, 1);
  lcd.print("Ampere: " + String(getACS()));

  delay(2000);
}

float getACS(){
  uint32_t period = 1000000 / 50;
  uint32_t t_start = micros();

  int16_t dataMax = 0;
  int16_t Inow;
  uint32_t Isum = 0;
  uint16_t measurements_count = 0;

  while (micros() - t_start < period) {
//    for (int i = 0; i < 10; i++) {
//      int adc = ads.readADC_SingleEnded(pin_acs);
//      if (adc > dataMax) dataMax = adc;
//    }
    Inow = ads.readADC_SingleEnded(pin_acs) - zero;
    Isum += Inow * Inow;
    measurements_count++;
  }
  Serial.println(sqrt(Isum / measurements_count));
  return sqrt(Isum / measurements_count) / 32767 * 6.144 / 0.066;
}

void calibrate() {
  uint32_t acc = 0;
  int sampling = 100;
  for (int i = 0; i < sampling; i++) {
    acc += ads.readADC_SingleEnded(pin_acs);
  }
  zero = acc / sampling;
}
