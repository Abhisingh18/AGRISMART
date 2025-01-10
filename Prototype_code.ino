#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

const char* ssid = "PU_HACKATHON";
const char* password = "Parul%2025";
unsigned long myChannelNumber = 2800421;
const char* myWriteAPIKey = "NDCWC397CZJLULAB";

#define DHTPIN 4  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define WATER_SENSOR_PIN 5 
#define N_PIN A0  
#define SOIL_MOISTURE_PIN A0 

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(N_PIN, INPUT);

  dht.begin();

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  ThingSpeak.begin(client);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int waterLevelRaw = analogRead(WATER_SENSOR_PIN);
  float waterLevel = map(waterLevelRaw, 0, 1023, 0, 100);

  int nValue = analogRead(N_PIN);
  float nVoltage = (nValue / 1023.0) * 3.3 * 10; 

  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisture = map(soilMoistureRaw, 0, 1023, 100, 0);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Water Level: "); 
  Serial.println(waterLevel); 
  Serial.print("NPK Sensor Data: ");
  Serial.println(nVoltage);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, waterLevel);
  ThingSpeak.setField(4, nVoltage);
  ThingSpeak.setField(5, soilMoisture);

  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (statusCode == 200) {
    Serial.println("Data uploaded to ThingSpeak successfully!");
  } else {
    Serial.println("Failed to upload data. Error code: " + String(statusCode));
  }
  delay(20000);
}

