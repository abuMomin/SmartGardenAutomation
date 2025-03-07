#include "DHT.h"

#define DHTPIN 27   
#define DHTTYPE DHT11   // DHT-11, instead of DHT22

#define soilWet 1800   // Define max value we consider soil 'wet'
#define soilDry 3200   // Define min value we consider soil 'dry'

#define soilPin 36       //  // receiving analog input through an ADC pin which is GPIO-36, this pin is input ONLY      
const int relay = 26;  // GPIO-26 is pin with both ADC and DAC cabability.. it can output both digital and analog signal, as well as PWM..

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(soilPin, INPUT);
  pinMode(relay, OUTPUT); 
  dht.begin();
  delay(2000);
  Serial.println("");
}

// Normally CLOSE configuration, send HIGH signal to start current flow ----->>>>>  I'm using Normally CLOSE config here
void loop() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%  Temperature: ");
  Serial.print(temp);
  Serial.println("°C ");

  int moisture = analogRead(soilPin);
  Serial.print("Analog Output of SOIL: ");
  Serial.println(moisture);

  if (moisture < soilWet) {
    Serial.println("Status: Soil is too wet");
  } 
  else if (moisture >= soilWet && moisture < soilDry) {
    Serial.println("Status: Soil moisture is perfect");
  } 
  else {
    Serial.println("Status: Soil is too dry - time to water!");
  }

  digitalWrite(relay, HIGH);
  Serial.println("Motor Turned ON");
  delay(2000);
  digitalWrite(relay, LOW);
  Serial.println("Motor Turned OFF");

  Serial.println("");
  delay(4000);
          // Normally you should take reading perhaps once or twice a day

}
