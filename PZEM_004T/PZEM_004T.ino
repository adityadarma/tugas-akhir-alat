#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ModbusMaster.h>  // Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol)
#include <SoftwareSerial.h>  //  ( NODEMCU ESP8266 )
SoftwareSerial pzem(D5,D6);  // (RX,TX) connect to TX,RX of PZEM for NodeMCU
ModbusMaster node; 
LiquidCrystal_I2C lcd(0x27 ,16,2);

double  I_PR, PPR, PR_PF;
int U_PR, P_PR, PR_F;
uint8_t result;

void setup(){
  Serial.begin(115200);
  Serial.println("Start serial");
  pzem.begin(9600);
  Serial.println("Start PZEM serial");
  node.begin(1, pzem);
  Serial.println("Start PZEM"); // 1 = ID MODBUS
  lcd.begin ();
}

void loop(){

  result = node.readInputRegisters(0x0000, 10);
  if (result == node.ku8MBSuccess)  {
    U_PR      = (node.getResponseBuffer(0x00)/10.0f);
    I_PR      = (node.getResponseBuffer(0x01)/1000.000f);
    P_PR      = (node.getResponseBuffer(0x03)/10.0f);
    PR_F      = (node.getResponseBuffer(0x07)/10.0f);
    PR_PF     = (node.getResponseBuffer(0x08)/100.0f);
  }
  lcd.setCursor(0, 0);
  lcd.print("V:" + String(U_PR) +"   ");
  lcd.setCursor(8, 0);
  lcd.print("I:" + String(I_PR) +"   ");
  Serial.print("U_PR:     "); Serial.println(U_PR);   // V
  Serial.print("I_PR:     "); Serial.println(I_PR,3);   //  A
  Serial.print("P_PR:     "); Serial.println(P_PR);   //  W 
  Serial.print("PR_F:     "); Serial.println(PR_F);    // Hz
  Serial.print("PR_PF:    "); Serial.println(PR_PF);
  Serial.println("====================================================");

  delay(2000);
}
