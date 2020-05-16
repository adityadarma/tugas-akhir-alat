#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal_I2C.h>

Adafruit_ADS1115 ads(0x48);
LiquidCrystal_I2C lcd(0x27 ,16,2);
int pin_acs = 0;
int zero;

void setup() 
{
//  Serial.begin(9600);  
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
//  Serial.println(getACS());
//  Serial.println();
  delay(1000);
}

float getACS(){
  uint32_t period = 1000000 / 50;
  uint32_t t_start = micros();

  int16_t dataMax = 0;
  uint32_t Isum = 0;
  uint32_t measurements_count = 0;
  int32_t Inow;

  while (micros() - t_start < period) {
    for (int i = 0; i < 50; i++) {
      int adc = ads.readADC_SingleEnded(pin_acs);
      if (adc > dataMax) dataMax = adc;
    }
    Inow = dataMax - zero;
    Isum += Inow*Inow;
    measurements_count++;
  }
  float Irms = sqrt(Isum / measurements_count) / 32735 * 6.138 / 0.066;
  return Irms;
}

void calibrate() {
  uint32_t acc = 0;
  int sampling = 100;
  for (int i = 0; i < sampling; i++) {
    acc += ads.readADC_SingleEnded(pin_acs);
    delay(10);
  }
  zero = acc / sampling;
}
