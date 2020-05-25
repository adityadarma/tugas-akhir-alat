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
bool statusUpload = false;
float frequency = 50;
int daya_actual = 0;

// waktu,lcd, add1115
SimpleTimer timer;
LiquidCrystal_I2C lcd(0x27 ,16,2);
Adafruit_ADS1115 ads(0x48);

// array dan object json
StaticJsonDocument<200> json;
JsonObject object = json.to<JsonObject>();
char jsonReal[200];

StaticJsonDocument<4096> doc;
JsonArray d_voltase = doc.createNestedArray("voltase");
JsonArray d_arus = doc.createNestedArray("arus");
JsonArray d_daya = doc.createNestedArray("daya");
char jsonData[4096];

// sensor zmpt101b
#define ZMPT101B A0
float windowLength = 100/frequency;    
int voltase_actual = 0;
int RawValue = 0;
unsigned long printPeriod = 2500;
unsigned long previousMillis = 0;

// sensor acs712
int pin_acs = 0;
int zero;
float arus_actual = 0;

RunningStatistics inputStats;

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
  }
  lcd.setCursor(0, 1);
  lcd.print("Terhubung       ");
  delay(1000);
  lcd.clear();

  ads.begin();
  inputStats.setWindowSecs(windowLength);
  calibrate_acs712();

  timer.setInterval(1000, tempData);
  timer.setInterval(10000, ubah);
  timer.setInterval(30000, upload);
}

void loop() {
  timer.run();
  RawValue = analogRead(ZMPT101B);
  inputStats.input(RawValue);
  if((unsigned long)(millis() - previousMillis) >= printPeriod) {
    previousMillis = millis();
    voltase_actual = inputStats.sigma() * 2.5;
    get_arus();
    
    lcd.setCursor(0, 0);
    lcd.print("V:" + String(voltase_actual) +"   ");
  }
}

void tempData(){
  // voltase
  d_voltase.add(voltase_actual);

  // arus
  d_arus.add(arus_actual);
  
  // daya
  daya_actual = voltase_actual*arus_actual;
  lcd.setCursor(0, 1);
  lcd.print("P:" + String(daya_actual) +"   ");
  d_daya.add(daya_actual);
}

void ubah(){
  object["voltase"] = voltase_actual;
  object["arus"] = arus_actual;
  object["daya"] = daya_actual;
  serializeJson(object, jsonReal);
  
  const char* serverName = "http://restapi-ta.kubusoftware.com/pantauan"; // Alamat server Online
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", api_key);
  int httpResponseCode = http.POST(jsonReal);
}

void upload(){
  if(WiFi.status() == WL_CONNECTED){
    serializeJson(doc, jsonData);
    lcd.setCursor(8, 1);
    lcd.print("Upload");
    
    //upload data
    int waktu = millis();
    const char* serverName = "http://restapi-ta.kubusoftware.com/penggunaan"; // Alamat server Online
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", api_key);
    int httpResponseCode = http.POST(jsonData);
  
    if (httpResponseCode == 201) {
      res_time = millis() - waktu;
      deleteData();
      String payload = http.getString();
    }else{
      res_time = 0;
    }
    http.end();
    delay(100);
    lcd.setCursor(8, 1);
    lcd.print("W:" + String(res_time) + "ms ");
  }
}

void deleteData(){
    doc.clear();    
    JsonArray d_voltase = doc.createNestedArray("voltase");
    JsonArray d_arus = doc.createNestedArray("arus");
    JsonArray d_daya = doc.createNestedArray("daya");
}

void get_arus(){
  uint32_t period = 1000;
  uint32_t t_start = micros();

  int16_t dataMax = 0;
  int16_t Inow;
  uint32_t Isum = 0;
  int measurements_count = 0;

  while (micros() - t_start < period) {
    for (int i = 0; i < 10; i++) {
      int adc = ads.readADC_SingleEnded(pin_acs);
      if (adc > dataMax) dataMax = adc;
    }
    Inow = dataMax - zero;
    Isum += Inow * Inow;
    measurements_count++;
  }
  arus_actual = sqrt(Isum / measurements_count) / 32767 * 6.144 / 0.066;
  lcd.setCursor(8, 0);
  lcd.print("I:" + String(arus_actual) +"   "); 
}
