#include <Arduino.h>

/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  WARNING :
  For this example you'll need Adafruit DHT sensor libraries:
    https://github.com/adafruit/Adafruit_Sensor
    https://github.com/adafruit/DHT-sensor-library

  App project setup:
    Value Display widget attached to V5
    Value Display widget attached to V6
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <RH_RF95.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ec9541a64ade4a7e8f3f18d447aa6f69";

//DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
RH_RF95 SX1278(8, 2);

int model;
int soilValue;
int lightValue;
int rainValue;
int temperatureValue;
int humidityValue;

int parseMessage(String s) {
    Serial.println(s);
    int commaIndex1 = s.indexOf(',');
    int commaIndex2 = s.indexOf(',', commaIndex1 + 1);
    int commaIndex3 = s.indexOf(',', commaIndex2 + 1);
    int commaIndex4 = s.indexOf(',', commaIndex3 + 1);
    int commaIndex5 = s.indexOf(',', commaIndex4 + 1);

    String value1 = s.substring(0, commaIndex1);
    String value2 = s.substring(commaIndex1 + 1, commaIndex2);
    String value3 = s.substring(commaIndex2 + 1, commaIndex3); // To the end of the string
    String value4 = s.substring(commaIndex3 + 1, commaIndex4);
    String value5 = s.substring(commaIndex4 + 1, commaIndex5);
    String value6 = s.substring(commaIndex5 + 1);

    model = value1.toInt();
    soilValue = value2.toInt();
    lightValue = value3.toInt();
    rainValue = value4.toInt();
    temperatureValue = value5.toInt();
    humidityValue = value6.toInt();

    return model;
 }

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor() {
   if (SX1278.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (SX1278.recv(buf, &len)) {
      int sender = parseMessage((String) ((char*) buf));
      if (sender == 1) {
        Blynk.virtualWrite(V0, soilValue);
        Blynk.virtualWrite(V1, lightValue);
        Blynk.virtualWrite(V2, rainValue);
        Blynk.virtualWrite(V3, temperatureValue);
        Blynk.virtualWrite(V4, humidityValue);
      } else if (sender == 2) {
        Blynk.virtualWrite(V10, soilValue);
        Blynk.virtualWrite(V11, lightValue);
        Blynk.virtualWrite(V12, rainValue);
        Blynk.virtualWrite(V13, temperatureValue);
        Blynk.virtualWrite(V14, humidityValue);
      }
    }
  }
  Serial.println("Not available");
}

void setup()
{

  Serial.begin(9600);
  while (!Serial);
  if (!SX1278.init())
    Serial.println("init failed");
  Serial.println("end setup");

  // Debug console
//  Serial.begin(9600);

  Blynk.begin(auth);

//  dht.begin();

  // Setup a function to be called every second
  timer.setInterval(1L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
