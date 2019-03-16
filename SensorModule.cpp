#include <Arduino.h>

#include <dht.h>
#include <string.h>
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 SX1278;

dht DHT;

const uint8_t LIGHT_SENSOR_PIN = A2;
const uint8_t SOIL_MOISTURE_SENSOR_PIN = A1;
const uint8_t DHT_SENSOR_PIN = 7;
const uint8_t RAIN_SENSOR_PIN = A3;

const String MODULE_NUMBER = "002";

String message;

int lightSensorValue;
int soilMoistureSensorValue;
int dhtSensorTemp;
int dhtSensorHum;
int rainSensorValue;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!SX1278.init())
    Serial.println("Notice:init failed");
  Serial.println("Init successful");
}

void loop() {
  lightSensorValue = analogRead(LIGHT_SENSOR_PIN);
  soilMoistureSensorValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  rainSensorValue = analogRead(RAIN_SENSOR_PIN);
  int chk = DHT.read11(DHT_SENSOR_PIN);
  dhtSensorTemp = DHT.temperature;
  dhtSensorHum = DHT.humidity;

//  String number = String(MODULE_NUMBER);

  message = (MODULE_NUMBER + ", " + soilMoistureSensorValue + ", " + lightSensorValue + ", " +
    + rainSensorValue + ", " + dhtSensorTemp + ", "  + dhtSensorHum);


  char charray[message.length() + 1];
  message.toCharArray(charray, message.length() + 1);

  SX1278.send(charray, sizeof(charray));
  Serial.println(charray);

  delay(5000);

}
