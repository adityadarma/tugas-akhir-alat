#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);
#define ADC_SCALE 65535.0
#define VREF 5.0
unsigned int sensitivity = 66;   // 185mV/A for 5A, 100 mV/A for 20A and 66mV/A for 30A Module
uint16_t zero = 16384;
uint16_t frequency = 50;
int pin = 0;

//void setup(void) 
//{
//  Serial.begin(9600);  
//  ads.begin();
//  calibrate();
//}
//
//void loop(void) 
//{
//  Serial.println(ads.readADC_SingleEnded(pin));
//  delay(1000);
//}

//void calibrate(){
//  uint32_t acc = 0;
//  for (int i = 0; i < 10; i++) {
//    acc += ads.readADC_SingleEnded(pin);
//  }
//  zero = acc / 10;
//}
//
//float getCurrentAC() {
//  uint32_t period = 100000 / frequency;
//  uint32_t t_start = micros();
//
//  uint64_t Isum = 0, measurements_count = 0;
//  int32_t Inow;
//
//  while (micros() - t_start < period) {
//    Inow = ads.readADC_SingleEnded(pin) - zero;
//    Isum += Inow*Inow;
//    measurements_count++;
//  }
//  Serial.println(sqrt(Isum));
//
//  float Irms = sqrt(Isum / measurements_count) / ADC_SCALE * VREF / sensitivity;
//  return Irms;
//}


/* https://www.nyebarilmu.com */
 
const int input_sensor = A0; // nama lain dari A0 yaitu input_sensor
const int output_pwm = 2; // nama lain dari 2 yaitu output_pwm
 
//nilai default variabel data
int nilai_adc= 00;    
int nilai_pwm = 00;    
float voltageOut= 00;
float voltage_ac= 00;
 
void setup() {
  // inisialisasi komunikasi serial pada 960bps
  Serial.begin(9600); 
  ads.begin();
}
 
void loop() {
  nilai_adc = ads.readADC_SingleEnded(pin); // Prosedur pembacaan analog pin           
  // mapping pembacaan resolusi sensor
  nilai_pwm = map(nilai_adc, 00, 1023, 00, 255);  
  // mengubah nilai analog out
  analogWrite(output_pwm, nilai_pwm);    
 voltageOut = nilai_adc * (5.0 / 1023.0);  
 voltage_ac = map(nilai_adc, 00,695,00,220); //didapat dari pembacaan program adc diatas
 
  // Mencetak hasil ke monitor serial:
  Serial.print("sensor = " );                       
  Serial.println(nilai_adc);      
  Serial.print("output pwm= ");      
  Serial.println(nilai_pwm);   
  Serial.print("Voltage ADC = ");      
  Serial.println(voltageOut);
  Serial.print("Voltage AC = ");      
  Serial.println(voltage_ac);
  delay(1500);  //jeda 2ms   
  Serial.println();                
}
