#include "DHT.h"
#include <WiFi.h>
#include <FirebaseESP32.h>

//Provide the token generation process info.
#include <addons/TokenHelper.h>
//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Insert your network credentials
#define WIFI_SSID "Win Enan"
#define WIFI_PASSWORD "waqi21087"

// Insert Firebase project API Key
#define API_KEY "ipiIBYlZKeEjUo19QpY2ac8OLFiqMr0v19QAIASQ"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://cn-project-87b83-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define DHTPIN 27   
#define DHTTYPE DHT11   // DHT-11, instead of DHT22

#define soilWet 2200   // Define max value we consider soil 'wet'
#define soilDry 3200   // Define min value we consider soil 'dry'

String garden_status = "";

#define soilPin 36       //  // receiving analog input through an ADC pin which is GPIO-36, this pin is input ONLY      
const int relay = 26;  // GPIO-26 is pin with both ADC and DAC cabability.. it can output both digital and analog signal, as well as PWM..

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(soilPin, INPUT);
  pinMode(relay, OUTPUT); 
  dht.begin();
  delay(2000);
  
  initWiFi();
  
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(DATABASE_URL, API_KEY);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  Serial.println();
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
    garden_status = "Soil is too wet";
    Serial.println(garden_status);
  } 
  else if (moisture >= soilWet && moisture < soilDry) {
    garden_status = "Soil moisture is perfect";
    Serial.println(garden_status);
  } 
  else {
    garden_status = "Soil is too dry - time to water!";
    Serial.println(garden_status);
    digitalWrite(relay, HIGH);
    Serial.println("Motor Turned ON");
    delay(2000);
    digitalWrite(relay, LOW);
  }
  
  if (Firebase.ready()){
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setFloat(&fbdo, "weather_db/temp", temp)){
      Serial.println("Temparature PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    //Send more value
    if (Firebase.RTDB.setFloat(&fbdo, "weather_db/hum", hum)){
      Serial.println("Humidity PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "weather_db/soil", moisture)){
      Serial.println("Soil Moisture PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setString(&fbdo, "weather_db/status", garden_status)){
      Serial.println("Soil STATUS PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("");
  delay(4000);
          // Normally you should take reading perhaps once or twice a day

}

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
