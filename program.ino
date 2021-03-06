#include <ESP8266WiFi.h>
#include <DHT.h>
#include "ThingSpeak.h"

#define tempHum D5
#define soilMos A0
#define subPump D3
#define subPumpLED D0
#define tempHIGH D1
#define tempLOW D2

DHT dht(tempHum, DHT22);

unsigned const long myChannelNumber = 1605663;
const char *myWriteAPIKey = "CBOTDSQPIJVPZECA";

const char *ssid = "infinitylectures.com";
const char *pass = "jeetwifi73";

WiFiClient Client;

double soilMoistureData = 0;
float h, t;
unsigned long prevMillis = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  dht.begin();
  ThingSpeak.begin(Client);
  pinMode(soilMos, INPUT);
  pinMode(subPump, OUTPUT);
  pinMode(subPumpLED, OUTPUT);
  pinMode(tempHIGH, OUTPUT);
  pinMode(tempLOW, OUTPUT);
}

void loop()
{
  connectwifi();
  h = dht.readHumidity();
  t = dht.readTemperature();
  double data = analogRead(soilMos);
  soilMoistureData = 100 - ((data / 1024) * 100);

  if (soilMoistureData > 50)
  {
    printData(0);
    digitalWrite(subPump, LOW);
    digitalWrite(subPumpLED, LOW);
    sendData2Thingspeak(0);
  }
  else
  {
    printData(1);
    digitalWrite(subPump, HIGH);
    digitalWrite(subPumpLED, HIGH);
    sendData2Thingspeak(1);
  }

  if(t < 25){
    digitalWrite(tempHIGH, LOW);// red color led
    digitalWrite(tempLOW, HIGH);// green color led
  }
  else{
    digitalWrite(tempHIGH, HIGH);
    digitalWrite(tempLOW, LOW);
  }
  
  Serial.println("Waiting....");
  delay(2000);
}

void connectwifi()
{
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(2000);
    }
    Serial.println();
    Serial.println("NodeMCU is connected");
    Serial.print(ssid);
    Serial.print(" :- ");
    Serial.println(WiFi.localIP());
  }
}

void printData(int s)
{
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("soil Moisture: ");
  Serial.println(soilMoistureData);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Submersible: ");
  
  if(s == 1)
    Serial.println("RUNNING");
  else
    Serial.println("STOP");
}

void sendData2Thingspeak(int s)
{
    ThingSpeak.writeField(myChannelNumber, 4, s, myWriteAPIKey);
//    delay(30000);
    ThingSpeak.writeField(myChannelNumber, 3, int(soilMoistureData), myWriteAPIKey);
//    delay(30000);
    ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
//    delay(30000);
    ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
//    delay(30000);
}
