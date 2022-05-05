#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
#define DHTPIN 5          //pin where the dht22 is connected
DHT dht(DHTPIN, DHT22);
#define relay 7
#define ss 10
#define rst 9
#define dio0 2
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

String LoRaMessage = "";
int counter = 0;

const int AirValue = 590;   //you need to replace this value with Value_1
const int WaterValue = 300;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;


void setup() 
{
  Serial.begin(115200);
  dht.begin();
  sensors.begin(); // Dallas temperature
  pinMode(relay, OUTPUT);
  
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
}
 
void loop() 
{
  soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
 
  if (isnan(h) || isnan(t)) 
  {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilMoistureValue);
  
  Serial.print("Soil Moisture: ");
  Serial.print(soilmoisturepercent);
  Serial.println("%");
  
  Serial.print("Soil Temperature: ");
  Serial.print(temp);
  Serial.println("°C");
  
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("°C");
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");
  Serial.println("");
  
  Serial.print("Sending packet: ");
  Serial.println(counter);
  
    if (soilmoisturepercent >= 0 && soilmoisturepercent <= 40)
  {
    Serial.println("Plants need water..., notification sent");
    digitalWrite(relay, LOW);
    Serial.println("Motor is ON");
    //WidgetLED PumpLed(V5);
    //PumpLed.on();
   }
  else if (soilmoisturepercent > 30 && soilmoisturepercent <= 100)
  {
    Serial.println("Soil Moisture level looks good...");
    digitalWrite(relay, HIGH);
    Serial.println("Motor is OFF");
    //WidgetLED PumpLed(V5);
    //PumpLed.off();
  }
  
  LoRaMessage = String(counter) +"/" + String(soilMoistureValue) + "&" + String(soilmoisturepercent)
                + "#" + String(temp) + "@" + String(t) + "$" + String(h);
  
   // send packet
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
 
  counter++;
 
  delay(1500);
}
