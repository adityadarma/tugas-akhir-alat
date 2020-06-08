#include <SoftwareSerial.h> 
#include <PZEM004Tv30.h>
SoftwareSerial mySerial(D5,D6);  // (RX,TX) NodeMCU

//PZEM004Tv30 pzem(10,11);   /// Software Serial pin 10 (RX) & 11 (TX) for arduino uno
PZEM004Tv30 pzem(1,mySerial);  // (RX)PZEM004Tv30 -> Tx2Arduino ; (TX)PZEM004Tv30 -> Rx2ArduinoMega for arduio mega2560

void setup() {
  Serial.begin(115200);
}

void loop() {
    float voltage = pzem.voltage();
    if(voltage != NAN){
        Serial.print("Tegangan: "); Serial.print(voltage); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }

    float current = pzem.current();
    if(current != NAN){
        Serial.print("Arus: "); Serial.print(current); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }

    float power = pzem.power();
    if(current != NAN){
        Serial.print("Daya: "); Serial.print(power); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }

    float energy = pzem.energy();
    if(current != NAN){
        Serial.print("Energi: "); Serial.print(energy,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }

    float frequency = pzem.frequency();
    if(current != NAN){
        Serial.print("Frekuensi: "); Serial.print(frequency, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }

    float pf = pzem.pf();
    if(current != NAN){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        Serial.println("Error reading power factor");
    }

    Serial.println();
    delay(2000);
}
