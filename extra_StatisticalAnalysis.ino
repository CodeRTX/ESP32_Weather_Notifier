// Import required libraries
#include "WiFi.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "My-2G";
const char* password = "Qwerty12";

#define DHTTYPE    DHT11     // DHT 11

#define DHTPIN 4            // Digital pin connected to the DHT sensor

DHT dht(DHTPIN, DHTTYPE);

const int numReadings = 1000;                                     // Number of readings to collect
float readingsTemp[numReadings],readingsHumidity[numReadings];   // Arrays to store readings

float sum = 0;
float mean = 0,variance = 0,standardDeviation = 0,median = 0,mode_s = 0;
int i=0,count=1;

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else
   return String(t);
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else
   return String(h);
}

// Replaces placeholder with DHT values
String processor(const String& var){

  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

void performStatisticalAnalysis(float arr[]) {
  int len=sizeof(arr) / sizeof(float);
  // Calculate the mean
  sum = 0;
  for (int j = 0; j < len; j++) {
    sum += arr[j];
  }
  mean = sum / len;
  // Calculate the variance
  variance = 0;
  for (int j = 0; j < len; j++) {
    variance += pow((arr[j] - mean), 2);
  }
  variance /= len;
  // Calculate the standard deviation
  standardDeviation = sqrt(variance);
  for (int j = 0; j < len - 1; j++) {
    for (int k = j+1; k < len; k++) {
      if (arr[j] > arr[k]) {
        float temp = arr[j];
        arr[j] = arr[k];
        arr[k] = temp;
      }
    }
  }
  median = (len % 2 == 0) ? (arr[(len/ 2) - 1] + arr[len/ 2]) / 2 : arr[(int)(len/ 2)];

  // Calculate the mode
  int maxCount = 0;
  for (int i = 0; i < len; i++) {
    int c = 0;
    for (int j = 0; j < len; j++) {
      if (arr[j] == arr[i])
        c++;
    }
    if (c > maxCount) {
      maxCount = c;
      mode_s = arr[i];
    }
  }
  Serial.print("Mean: ");
  Serial.print(mean);
  Serial.print(", Variance: ");
  Serial.print(variance);
  Serial.print(", Standard Deviation: ");
  Serial.print(standardDeviation);
  Serial.print(", Median: ");
  Serial.print(median);
  Serial.print(", Mode: ");
  Serial.println(mode_s);
}

void setup(){
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop(){
  readingsTemp[i]=readDHTTemperature().toFloat();
  readingsHumidity[i]=readDHTHumidity().toFloat();
  Serial.print("count= ");
  Serial.println(count);
  i++;
  count++;
  delay(500);
  if(i==numReadings)
  {
   Serial.println("Analysis of Temperature: ---");
   performStatisticalAnalysis(readingsTemp);
   Serial.println("Analysis of Humidity: ---");
   performStatisticalAnalysis(readingsHumidity);
   while(1);
  }
}
