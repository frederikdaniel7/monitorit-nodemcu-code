
#include "ESP8266WiFi.h"
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>
#define FIREBASE_HOST "https://homesecurityapp-8e740-default-rtdb.asia-southeast1.firebasedatabase.app/devices/[DEVICE NODE]" //Isikan DEVICE NODE dengan nilai yang ada pada aplikasi
#define FIREBASE_AUTH "AIzaSyB9E1Y4fNo7EeDZpdVXKTvHq2Tzf1vNEF0"
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define MQ2pin 0

float sensorValue;
FirebaseData firebaseData;
DHT dht(DHT_PIN, DHT_TYPE);

const int MQ2 = 13;
const int ledPin = 12; 
const char* ssid = "[WIFI SSID]"; //NAMA WIFI YANG AKAN DIGUNAKAN 
const char* password = "[WIFI PASSWORD]"; //PASSWORD WIFI YANG AKAN DIGUNAKAN 
const int Smoke = 0;
void setup(void)
{ 
  Serial.begin(115200);
  // Connect to WiFi
  pinMode(MQ2, INPUT);
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() 
{
 if (Firebase.getJSON(firebaseData, "/devices/-NYtnY7cgzqV4vISTEH9")) {
    String jsonData = firebaseData.jsonString();
    Serial.println("Received JSON data: " + jsonData);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonData);
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    const char* id = doc["id"];
    bool isopen = doc["isopen"];
    const char* name = doc["name"];
    const char* groupid = doc["groupid"];
    if (isopen) {
      Serial.println("The door is open!");
      digitalWrite(ledPin, HIGH); // Turn on LED
      // Add your own logic here
    } else {
      Serial.println("The door is closed!");
      digitalWrite(ledPin, LOW); // Turn on LED
      // Add your own logic here
    }
    Serial.println("Parsed data:");
    Serial.println("id: " + String(id));
    Serial.println("isopen: " + String(isopen));
    Serial.println("name: " + String(name));
    Serial.println("groupid: " + String(groupid));
    delay(5000);
    if (!isnan(temperature) && !isnan(humidity)) {
      // Upload the temperature and humidity to Firebase
      Serial.println("Temperature: " + String(temperature) + " °C");
      Firebase.setFloat(firebaseData, "/devices/[DEVICE NODE]/temperature", temperature);
      Serial.println("Humidity: " + String(humidity) + " %");
      Firebase.setFloat(firebaseData, "/devices/[DEVICE NODE]/humidity", humidity);
      // Print the temperature and humidity to the serial monitor
      
    } else {
      Serial.println("Failed to read temperature and humidity from DHT11 sensor!");
    }// Wait for 10 seconds before reading again
    sensorValue = analogRead(MQ2pin);
    Serial.println("Nilai Partikel: " + String(sensorValue));
   if (sensorValue > 400.0) { // Smoke is detected
        Firebase.setString(firebaseData, "/devices/[DEVICE NODE]/smokedetected", "True");
        Serial.println("Smoke detected!");
      } else { // Smoke is not detected
        Firebase.setString(firebaseData, "/devices/[DEVICE NODE]/smokedetected", "False");
        Serial.println("No smoke detected.");
      }
  
  }
  else {
    Serial.println("Failed to read data from Firebase!");
    delay(5000); // Retry after 5 seconds if reading failed
  }
}