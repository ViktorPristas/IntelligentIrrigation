#include <dht.h>
#include <string.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <EEPROM.h>

RH_RF95 SX1278;

dht DHT;

const uint8_t LIGHT_SENSOR_PIN = A2;
const uint8_t SOIL_MOISTURE_SENSOR_PIN = A1;
const uint8_t DHT_SENSOR_PIN = 7;
const uint8_t RAIN_SENSOR_PIN = A3;
const uint8_t LED_PIN = 3;

const String MODULE_NUMBER = String(EEPROM.read(0));

String message;

int lightSensorValue;
int soilMoistureSensorValue;
int dhtSensorTemp;
int dhtSensorHum;
int rainSensorValue;

int checksum;

 void sendData() {
   lightSensorValue = analogRead(LIGHT_SENSOR_PIN);
   soilMoistureSensorValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
   rainSensorValue = analogRead(RAIN_SENSOR_PIN);
   int chk = DHT.read11(DHT_SENSOR_PIN);
   dhtSensorTemp = DHT.temperature;
   dhtSensorHum = DHT.humidity;

   checksum = MODULE_NUMBER.toInt() + lightSensorValue + soilMoistureSensorValue + rainSensorValue + dhtSensorTemp + dhtSensorHum;

   message = (MODULE_NUMBER + ", " + soilMoistureSensorValue + ", " + lightSensorValue + ", " +
    + rainSensorValue + ", " + dhtSensorTemp + ", "  + dhtSensorHum + ", " + checksum);  

   char charray[message.length() + 1];
   message.toCharArray(charray, message.length() + 1);
  
   SX1278.send(charray, sizeof(charray));
   Serial.println(charray);
 }

 boolean parseMessage(String s) {
   Serial.println(s);
   int commaIndex1 = s.indexOf(',');
   String value1 = s.substring(0, commaIndex1);
   if (value1.toInt() == MODULE_NUMBER.toInt()) {
     int commaIndex2 = s.indexOf(',', commaIndex1 + 1);
     int commaIndex3 = s.indexOf(',', commaIndex2 + 1);
     
     String value2 = s.substring(commaIndex1 + 1, commaIndex2);
     String value3 = s.substring(commaIndex2 + 1, commaIndex3);

     int onOff = value2.toInt();
     int wateringTime = value3.toInt() * 1000;
     
     if (onOff == 1) {
       digitalWrite(LED_PIN, HIGH);
       delay(wateringTime);
       digitalWrite(LED_PIN, LOW);
     }
     return true;
   }
   return false;
 }

 boolean receiveData() {
   if (SX1278.available()) {
    // Should be a message for us now
     uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
     uint8_t len = sizeof(buf);
     if (SX1278.recv(buf, &len)) {
       if (parseMessage((String) ((char*) buf))) {
        SX1278.setModeTx();
         return true;
       }      
     }
   }
   return false;
 }


 void setup() {
  pinMode(LED_PIN, OUTPUT);
   Serial.begin(9600);
   while (!Serial);

   if (!SX1278.init())
     Serial.println("Notice:init failed");
   Serial.println("Init successful");
 }
 

 void loop() {
   boolean reply = false;
   sendData();
   long timeNow = millis() + 10 * 1000 * 60 + random(3000);
   while(timeNow  > millis()) {
     if (receiveData()) {
       reply = true;
       break;
     }
   }
   if (reply) {
    delay(30000 + random(5000));
   }
  
 }
