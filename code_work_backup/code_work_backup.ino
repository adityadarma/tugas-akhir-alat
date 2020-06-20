#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ModbusMaster.h>  // Komunikasi RS232/485 (via RTU protocol)
#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )

// komunikasi dengan PZEM-004T
SoftwareSerial pzem(D5,D6);  // (TX,RX) NodeMCU
ModbusMaster node; 

// http server
HTTPClient http;
const char* ssid = "WIFI_EXT";
const char* password = "terusmeganti";
const char* api_key = "Bearer 3HhTHxVEj3LmFWUNIdewb7oEgNNBskInmOGWsCmQeuv9UmemvuoIZCepgtzy"; // API_KEY

// general
SimpleTimer timer;
LiquidCrystal_I2C lcd(0x27 ,16,2);
int U_PR;
double I_PR;
double PF_PR;
int P_PR;
int res_time = 0;

// array dan object json
StaticJsonDocument<4096> doc;
JsonArray daya_aktif = doc.createNestedArray("daya_aktif");
char jsonData[4096];

void setup() {
  Serial.begin(115200);
  lcd.begin ();
  pzem.begin(9600);
  node.begin(1, pzem);
  WiFi.begin(ssid, password);
  
  timer.setInterval(1000, saveData);
  timer.setInterval(3000, readData);
  timer.setInterval(15000, uploadData);
  
  delay(1000);
  lcd.clear();
}

void loop() {
  timer.run();
    
  lcd.setCursor(0, 0);
  lcd.print("V:" + String(U_PR) +"   ");
  lcd.setCursor(8, 0);
  lcd.print("I:" + String(I_PR) +"   ");

  lcd.setCursor(15, 0);
  if(WiFi.status() == WL_CONNECTED){
    lcd.print("C");
  }else{
    lcd.print("D");
  }

  lcd.setCursor(0, 1);
  lcd.print("F:" + String(PF_PR));
  lcd.setCursor(8, 1);
  lcd.print("P:" + String(P_PR) +" ");
}

void readData(){
  uint8_t result = node.readInputRegisters(0x0000, 10);
  if (result == node.ku8MBSuccess)  {
    U_PR      = (node.getResponseBuffer(0x00)/10.0f);   // V
    I_PR      = (node.getResponseBuffer(0x01)/1000.000f);   //  A
    P_PR      = (node.getResponseBuffer(0x03)/10.0f);   //  W
    PF_PR     = (node.getResponseBuffer(0x08)/100.0f);
  }else{
    U_PR      = 0;
    I_PR      = 0;
    P_PR      = 0;
    PF_PR     = 0;
  }
}

void saveData(){
  daya_aktif.add(P_PR);
}

void uploadData(){
  if(WiFi.status() == WL_CONNECTED){
    doc["tegangan"] = U_PR;
    doc["arus"] = I_PR;
    doc["faktor_daya"] = PF_PR;
    doc["daya"] = P_PR;
    serializeJson(doc, jsonData);
    
    // proses mulai upload data
    int waktu = millis();
    const char* serverName = "http://restapi-ta.kubusoftware.com/penggunaan";
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", api_key);
    int httpResponseCode = http.POST(jsonData);
  
    if (httpResponseCode == 201) {
      res_time = millis() - waktu;
      doc.clear();
      JsonArray daya_aktif = doc.createNestedArray("daya_aktif");
    }else{
      res_time = 0;
    }
    http.end();
    // slese upload data
    
    Serial.println(String(res_time) + " ms");
  }
}
