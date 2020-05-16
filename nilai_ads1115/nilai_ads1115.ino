#include <Wire.h>
#include <Adafruit_ADS1015.h>
 
Adafruit_ADS1115 ads(0x48);
 
void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
   
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
   
  ads.begin();
}
 
void loop(void)
{
  int16_t adc0, adc1, adc2, adc3;
  float voltage0, voltage1, voltage2, voltage3;
   
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  voltage0 = (adc0 * 0.1876)/1000;
  voltage1 = (adc1 * 0.1876)/1000;
  voltage2 = (adc2 * 0.1876)/1000;
  voltage3 = (adc3 * 0.1876)/1000;
  
  Serial.print("AIN0: ");
  Serial.print(adc0);
  Serial.print(" Voltage: ");
  Serial.println(voltage0, 2);
  
  Serial.print("AIN1: ");
  Serial.print(adc1);
  Serial.print(" Voltage: ");
  Serial.println(voltage1, 2);
  
  Serial.print("AIN2: ");
  Serial.print(adc2);
  Serial.print(" Voltage: ");
  Serial.println(voltage2, 2);
  
  Serial.print("AIN3: ");
  Serial.print(adc3);
  Serial.print(" Voltage: ");
  Serial.println(voltage3, 2);
  Serial.println();
   
  delay(1000);
}
