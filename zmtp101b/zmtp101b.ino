#include <Filters.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27 ,16,2);
#define ZMPT101B A0

float testFrequency = 50;                     // test signal frequency (Hz)
float windowLength = 100/testFrequency;       // how long to average the signal, for statistist, changing this can have drastic effect
                                              // Test as you need
int RawValue = 0;     
float Volts_TRMS;     // estimated actual voltage in Volts

float intercept = 0;  // to be adjusted based on calibration testin
float slope = 1;      

unsigned long printPeriod = 1000; //Measuring frequency, every 1s, can be changed
unsigned long previousMillis = 0;

RunningStatistics inputStats; //This class collects the value so we can apply some functions

void setup() {
  lcd.begin();
  lcd.clear();
  inputStats.setWindowSecs( windowLength );
}

void loop() {        
     ReadVoltage();
}

float ReadVoltage(){
  RawValue = analogRead(ZMPT101B);  // read the analog in value:
  inputStats.input(RawValue);       // log to Stats function
  lcd.setCursor(0, 0);
  lcd.print("Voltase:");
        
  if((unsigned long)(millis() - previousMillis) >= printPeriod) { //We calculate and display every 1s
    previousMillis = millis();   // update time
    
    int hasil = inputStats.sigma() * 1.25;

    lcd.setCursor(0, 1);
    lcd.print(hasil);
  }
}
