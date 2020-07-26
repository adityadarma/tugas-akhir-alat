#include <Firebase.h> 
#include <FirebaseArduino.h> 
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ModbusMaster.h>  // Komunikasi RS232/485 (via RTU protocol)
#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )

// komunikasi dengan PZEM-004T
SoftwareSerial pzem(D5,D6);  // (TX,RX) NodeMCU
ModbusMaster node; 

// konfigurasi wifi
const char* ssid = "WIFI_EXT"; // WIFI_EXT
const char* password = "!@#$%12345"; // terusmeganti

// konfigurasi firebase
#define FIREBASE_HOST "wattmeter-9f837.firebaseio.com"  
#define FIREBASE_AUTH "AdNZTE00d93ym038JuNFUo4bHWEIejNLjwI0Mh6K" 

// general
LiquidCrystal_I2C lcd(0x27 ,16,2);
int U, U_temp;
int P, P_temp;
float PF, PF_temp;
float I, I_temp;
uint8_t result;

void setup() {
  lcd.begin ();
  pzem.begin(9600);
  node.begin(1, pzem);
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Status wifi:");
  while(WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 1);
    lcd.print("Menghubungkan");
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
  lcd.clear();
}

void loop() {
  result = node.readInputRegisters(0x0000, 10);
  if (result == node.ku8MBSuccess)  {
    U      = (node.getResponseBuffer(0x00)/10.0f);   // V
    I      = (node.getResponseBuffer(0x01)/1000.000f);   //  A
    PF     = (node.getResponseBuffer(0x08)/100.0f);
    P      = (U * I) * PF;   //  W
  }
    
  lcd.setCursor(0, 0);
  lcd.print("V:" + String(U) +"   ");
  if(U_temp != U){
     Firebase.setInt("tegangan", U);
     U_temp = U;
  }
  
  lcd.setCursor(8, 0);
  lcd.print("I:" + String(I, 3) +"   ");
  if(I_temp != I){
    Firebase.setFloat("arus", I);
    I_temp = I;
  }
  
  lcd.setCursor(0, 1);
  lcd.print("F:" + String(PF));
  if(PF_temp != PF){
    Firebase.setFloat("faktor_daya", PF);
    PF_temp = PF;
  }
  
  lcd.setCursor(8, 1);
  lcd.print("P:" + String(P) +"   ");
  if(P_temp != P){
    Firebase.setInt("daya", P);
    P_temp = P;
  }
  delay(2000);
}
