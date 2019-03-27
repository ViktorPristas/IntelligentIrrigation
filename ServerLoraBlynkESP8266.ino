// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX

/*--------------------------------------------------------------
  Program:      Comunication RMF96 LoRa & ESP8266 12E - Reception

  Description:  Example of LoRa Reception Communication with the RFM96 Modem with ESP8266 12E,
                set to 915.0 MHz

                Ejemplo de Comunicacion Recepcion LoRa con el Modem RFM96 con ESP8266 12E, 
                ajustado a 915.0 MHz

  
  Hardware:     ESP8266 12E NodeMCU Lolin .
                Modem RFM96 - HopeRF electronics
             
                
  Software:     Arduino IDE v1.8.3
  
  Date:         7 Mar 2018
   
  Modified or Created:       PDAControl   http://pdacontroles.com   http://pdacontrolen.com

  Complete Tutorial English:            http://pdacontrolen.com/comunication-lora-esp8266-radio-rfm95-1/  
  Tutorial Completo Español             http://pdacontroles.com/comunicacion-lora-esp8266-radio-rfm96-1/

  Based: RH_RF95.h library              https://github.com/PaulStoffregen/RadioHead/tree/master/examples
          Adafruit                      https://github.com/adafruit/RadioHead/tree/master/examples

         RH_RF95.h API                  http://www.airspayce.com/mikem/arduino/RadioHead/RH__RF95_8h_source.html 


         Gpio15/D8   –   NSS
         Gpio13/D7   –   MOSI
         Gpio12/D6   –   MISO
         Gpio14/D5   –   SCK
         Gpio05/D1   –   DIO0
         Gpio02/D4   –   DIO1
         Gpio16/D0   –   RST
         VCC         –   3.3V
         GND         –   GND
 
--------------------------------------------------------------*/
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <RH_RF95.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include <avr/wdt.h>

#define RFM95_CS 15
#define RFM95_RST 16
#define RFM95_INT 5

#define RF95_FREQ 434.0

char auth[] = "ec9541a64ade4a7e8f3f18d447aa6f69";
char ssid[] = "Wi-Fi";
char pass[] = "rcad74ujhtbh";

BlynkTimer timer;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint8_t model;
int soilValue;
int lightValue;
int rainValue;
uint8_t temperatureValue;
uint8_t humidityValue;
uint8_t minutesOfWatering;

int checksum;

uint8_t commaIndex1;
uint8_t commaIndex2;
uint8_t commaIndex3;
uint8_t commaIndex4;
uint8_t commaIndex5;
uint8_t commaIndex6;

String value1;
String value2;
String value3;
String value4;
String value5;
String value6;
String value7;

uint8_t parseMessage(String s) {
//   Serial.println(s);
   commaIndex1 = s.indexOf(',');
   commaIndex2 = s.indexOf(',', commaIndex1 + 1);
   commaIndex3 = s.indexOf(',', commaIndex2 + 1);
   commaIndex4 = s.indexOf(',', commaIndex3 + 1);
   commaIndex5 = s.indexOf(',', commaIndex4 + 1);
   commaIndex6 = s.indexOf(',', commaIndex5 + 1);

   value1 = s.substring(0, commaIndex1);
   value2 = s.substring(commaIndex1 + 1, commaIndex2);
   value3 = s.substring(commaIndex2 + 1, commaIndex3);
   value4 = s.substring(commaIndex3 + 1, commaIndex4);
   value5 = s.substring(commaIndex4 + 1, commaIndex5);
   value6 = s.substring(commaIndex5 + 1, commaIndex6);
   value7 = s.substring(commaIndex6 + 1, s.length());

   model = value1.toInt();
   soilValue = value2.toInt();
   lightValue = value3.toInt();
   rainValue = value4.toInt();
   temperatureValue = value5.toInt();
   humidityValue = value6.toInt();
   checksum = value7.toInt();


   int sum = model + soilValue + lightValue + rainValue + temperatureValue + humidityValue;

   if (model + soilValue + lightValue + rainValue + temperatureValue + humidityValue - checksum != 0) {
//    Serial.print("Sum of values: ");
//    Serial.print(sum);
//    Serial.print("\t Checksum: ");
//    Serial.println(checksum);
     return -1;
   }

   return model;
 }

  void sendReply() {
   String message; 
   if (lightValue < 700) {
     message = String(model) + ", 1, " + (String) minutesOfWatering; 
     char charray[message.length() + 1];
     message.toCharArray(charray, message.length() + 1);
     rf95.send((uint8_t *) charray, sizeof(charray));
   } else {
     message = String(model) + ", 0, "  + (String) minutesOfWatering; 
     char charray[message.length() + 1];
     message.toCharArray(charray, message.length() + 1);
     rf95.send((uint8_t *) charray, sizeof(charray));
   }

//   Serial.println(message);
 }

  void getData() {
   if (rf95.available()) {
     // Should be a message for us now
     uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
     uint8_t len = sizeof(buf);
     if (rf95.recv(buf, &len)) {
      Serial.println();
      Serial.println((char*) buf);
       int sender = parseMessage((String) ((char*) buf));
       if (sender == -1) {
         Serial.println("ERROR - Wrong data");
         return;
       } else if (sender == 1) {
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
       sendReply();
     }
   
   }
//   Serial.print("*");
//   Serial.print(millis());
//   Serial.print(" ");
//   wdt_reset();
 }

  BLYNK_WRITE(V20) {
  minutesOfWatering = param.asInt(); // assigning incoming value from pin V1 to a variable
}

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(9600);
  while (!Serial);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
     // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95 / 96/97/98 modules using the transmitter pin PA_BOOST, then
    // you can set transmission powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);

    Blynk.begin(auth, ssid, pass);
    timer.setInterval(1L, getData);
//    wdt_enable(WDTO_2S);
}



void loop()
{
  Blynk.run();
   timer.run();
//   wdt_reset();
}
