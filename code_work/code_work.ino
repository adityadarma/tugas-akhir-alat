#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "WIFI_EXT";
const char* password = "terusmeganti";
const char* api_key = "Bearer 3HhTHxVEj3LmFWUNIdewb7oEgNNBskInmOGWsCmQeuv9UmemvuoIZCepgtzy"; // API_KEY

HTTPClient http;
SimpleTimer timer;
StaticJsonDocument<4096> doc;
LiquidCrystal_I2C lcd(0x27 ,16,2);


JsonArray voltase = doc.createNestedArray("voltase");
JsonArray arus = doc.createNestedArray("arus");
JsonArray daya = doc.createNestedArray("daya");

int number = 0;
char jsonData[4096];
int u;
bool statusUpload = false;

void setup() {
  Serial.begin(9600);
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
  delay(2000);
  lcd.clear();

  timer.setInterval(1000, tempData);
  timer.setInterval(30000, upload);
}

void loop() {
  timer.run();
}

void deleteData(){
    doc.clear();    
    JsonArray voltase = doc.createNestedArray("voltase");
    JsonArray arus = doc.createNestedArray("arus");
    JsonArray daya = doc.createNestedArray("daya");
}

void tempData(){
  voltase.add(220);
  arus.add(48.75);
  daya.add(440);

  if(!statusUpload){
    lcd.setCursor(0, 0);
    lcd.print("V:" + String(220));
    lcd.setCursor(8, 0);
    lcd.print("A:" + String(48.75)); 
    lcd.setCursor(0, 1);
    lcd.print("P:" + String(440));
    delay(100);
  }
}

void upload(){
  int waktu = millis();
  serializeJson(doc, jsonData);
  statusUpload = true;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Koneksi :"); 
  lcd.setCursor(0, 1);
  lcd.print("Mengirim data"); 

  //const char* serverName = "http://192.168.1.28:8082/penggunaan"; // Alamat server Offline
  const char* serverName = "http://restapi-ta.kubusoftware.com/penggunaan"; // Alamat server Online
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", api_key);
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode == 201) {
    deleteData();
    String res =  String(millis() - waktu);
    lcd.setCursor(0, 1);
    lcd.print("Terkirim: " + res + "ms");
    String payload = http.getString();
    Serial.println(payload);
  }else{
    lcd.setCursor(0, 1);
    lcd.print("Pengiriman gagal");
  }
  delay(500);
  http.end();
  lcd.clear();
  statusUpload = false;
}
