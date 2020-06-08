#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Filters.h>
#include <Adafruit_ADS1015.h>

// http server
HTTPClient http;
const char* ssid = "WIFI_EXT";
const char* password = "terusmeganti";
const char* api_key = "Bearer 3HhTHxVEj3LmFWUNIdewb7oEgNNBskInmOGWsCmQeuv9UmemvuoIZCepgtzy"; // API_KEY

// general
int res_time = 0;
int daya_actual = 0;

// waktu,lcd, add1115
SimpleTimer timer;
LiquidCrystal_I2C lcd(0x27 ,16,2);
Adafruit_ADS1115 ads(0x48);
RunningStatistics inputStats;

// array dan object json
StaticJsonDocument<4096> doc;
JsonArray d_tegangan = doc.createNestedArray("tegangan");
JsonArray d_arus = doc.createNestedArray("arus");
JsonArray d_daya = doc.createNestedArray("daya");
char jsonData[4096];

// sensor zmpt101b
#define ZMPT101B A0
float windowLength = 100/50;    
int tegangan_actual = 0;
int RawValue = 0;
unsigned long previousMillis = 0;

// sensor acs712
int pin_acs = 0;
int zero;
float arus_actual = 0;
int16_t Inow;
uint32_t Isum = 0;
int measurements_count = 0;

//int number = 0;
void calibrate_acs712() {
  uint32_t acc = 0;
  int sampling = 100;
  for (int i = 0; i < sampling; i++) {
    acc += ads.readADC_SingleEnded(pin_acs);
  }
  zero = acc / sampling;
}

void setup() {
  Serial.begin(115200);
  lcd.begin ();
  WiFi.begin(ssid, password);
  
  lcd.setCursor(0, 0);
  lcd.print("Status wifi:");
  lcd.setCursor(0, 1);
  lcd.print("Memeriksa");
  delay(1000);
  while(WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 1);
    lcd.print("Menghubungkan");
    delay(500);
    
    Serial.print(".");
  }
  lcd.setCursor(0, 1);
  lcd.print("Terhubung       ");
  delay(1000);
  lcd.clear();
  
  ads.begin();
  inputStats.setWindowSecs(windowLength);
  calibrate_acs712();

  timer.setInterval(1000, tempData);
  timer.setInterval(20000, upload);
}

void loop() {
  timer.run();

  // tegangan
  RawValue = analogRead(ZMPT101B);
  inputStats.input(RawValue);

  // arus
  if(millis() % 8 == 0){
    Inow = ads.readADC_SingleEnded(pin_acs) - zero;
    Isum += Inow * Inow;
    measurements_count++;
  }
    
  if((unsigned long)(millis() - previousMillis) >= 2000) {
    previousMillis = millis();

    // tegangan
    tegangan_actual = inputStats.sigma() * 2.5;    
    lcd.setCursor(0, 0);
    lcd.print("V:" + String(tegangan_actual) +"   ");

    // arus
    arus_actual = sqrt(Isum / measurements_count) / 32767 * 6.144 / 0.066;
    lcd.setCursor(8, 0);
    lcd.print("I:" + String(arus_actual) +"   ");
//    lcd.print("I:" + String(measurements_count) +"   ");

    // daya
    daya_actual = tegangan_actual * arus_actual;
    d_daya.add(daya_actual);
    lcd.setCursor(0, 1);
    lcd.print("P:" + String(daya_actual) +"   ");

    // set ke awal
    Isum = 0;
    measurements_count = 0;
    
//    Serial.println(number);
//    number = 0;
  }
//  number++;
}

void tempData(){
  // tegangan
  d_tegangan.add(tegangan_actual);

  // arus
  d_arus.add(arus_actual);
  
  // daya
  daya_actual = tegangan_actual*arus_actual;
  d_daya.add(daya_actual);
}

void upload(){
  if(WiFi.status() == WL_CONNECTED){
    serializeJson(doc, jsonData);
    lcd.setCursor(8, 1);
    lcd.print("Upload  ");
    
    //upload data
    int waktu = millis();
    const char* serverName = "http://tugasakhir.kubusoftware.com/penggunaan"; // Alamat server Online
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", api_key);
    int httpResponseCode = http.POST(jsonData);
  
    if (httpResponseCode == 201) {
      res_time = millis() - waktu;
      deleteData();
    }else{
      res_time = 0;
    }
    http.end();
    lcd.setCursor(8, 1);
    lcd.print("W:" + String(res_time) + "ms      ");
  }
}

void deleteData(){
    doc.clear();    
    JsonArray d_tegangan = doc.createNestedArray("tegangan");
    JsonArray d_arus = doc.createNestedArray("arus");
    JsonArray d_daya = doc.createNestedArray("daya");
}
