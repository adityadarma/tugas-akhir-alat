#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>

const char* ssid = "WIFI_EXT";
const char* password = "terusmeganti";
const char* api_key = "Bearer 3HhTHxVEj3LmFWUNIdewb7oEgNNBskInmOGWsCmQeuv9UmemvuoIZCepgtzy"; // API_KEY

HTTPClient http;
SimpleTimer timer;
StaticJsonDocument<4096> doc;


JsonArray voltase = doc.createNestedArray("voltase");
JsonArray arus = doc.createNestedArray("arus");
JsonArray daya = doc.createNestedArray("daya");

int number = 0;
char jsonData[4096];

void setup() {
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

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
}

void upload(){
  int waktu = millis();
  serializeJson(doc, jsonData); // membuat array menjadi char

  //const char* serverName = "http://192.168.1.28:8082/penggunaan"; // Alamat server Offline
  const char* serverName = "http://restapi-ta.kubusoftware.com/penggunaan"; // Alamat server Online
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", api_key);

  int httpResponseCode = http.POST(jsonData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode == 201) {
    deleteData();
    String payload = http.getString();
    Serial.println(payload);
  }
  
  http.end();
  Serial.print("Waktu respon: ");
  Serial.println(millis() - waktu);
}
