#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);  /* Use this for the 16-bit version */

// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV (default)
// ads.setGain(GAIN_ONE);     // 1x gain   +/- 4.096V  1 bit = 2mV
// ads.setGain(GAIN_TWO);     // 2x gain   +/- 2.048V  1 bit = 1mV
// ads.setGain(GAIN_FOUR);    // 4x gain   +/- 1.024V  1 bit = 0.5mV
// ads.setGain(GAIN_EIGHT);   // 8x gain   +/- 0.512V  1 bit = 0.25mV
// ads.setGain(GAIN_SIXTEEN); // 16x gain  +/- 0.256V  1 bit = 0.125mV

unsigned int Sensitivity = 185;   // 185mV/A for 5A, 100 mV/A for 20A and 66mV/A for 30A Module
float Vpp = 0; // peak-peak voltage
float Vrms = 0; // rms voltage
float Irms = 0; // rms current
float Supply_Voltage = 247.0;           // reading from DMM
float Vcc = 5.0;         // ADC reference voltage // voltage at 5V pin
float power = 0;         // power in watt            
float Wh =0 ;             // Energy in kWh
unsigned long last_time =0;
unsigned long current_time =0;
unsigned long interval = 100;
unsigned int calibration = 12;  // V2 slider calibrates this
unsigned int pF = 85;           // Power Factor default 95


void getACS712() {  // for AC
  Vpp = getVPP();
  Vrms = (Vpp/2.0) *0.707;
  Vrms = Vrms - (calibration / 10000.0);     // calibtrate to zero with slider
  Irms = (Vrms * 1000)/Sensitivity ;
  if((Irms > -0.015) && (Irms < 0.008)){  // remove low end chatter
    Irms = 0.0;
  }
  power= (Supply_Voltage * Irms) * (pF / 100.0);
  last_time = current_time;
  current_time = millis();  
  Wh = Wh+  power *(( current_time -last_time) /3600000.0) ; // calculating energy in Watt-Hour

  Serial.print("Irms:  ");
  Serial.print(String(Irms, 3));
  Serial.println(" A");
  Serial.print("Power: "); 
  Serial.print(String(power, 3));
  Serial.print(" W");
  Serial.print("\t Energy Unit(s) (KWh): ");
  Serial.println(String((Wh/1000), 4));

}

float getVPP()
{
  int16_t adc1;
  float result;
  int readValue;              
  int maxValue = 0;           
  int minValue = 32768;        
  uint32_t start_time = millis();



  while((millis()-start_time) < 950) //read every 0.95 Sec
  {
     readValue = ads.readADC_SingleEnded(0);  
     if (readValue > maxValue)
     {       
         maxValue = readValue;
     }
     if (readValue < minValue)
     {        
         minValue = readValue;
     }
  }
   result = ((maxValue - minValue) * Vcc) / 32768.0;
   Serial.print("\nVp-p:  ");
   Serial.println(String(result, 3));
 
   return result;
}


void setup() {
  Serial.begin(9600);
  ads.begin();
}

void loop()
{

  getACS712();
  delay (2000);
}
