#include <DHT.h> // include DHT library
#include <Wire.h> // include Arduino Wire library
#include "Ultrasonic.h" // include Seeed Studio ultrasonic ranger library
#include <ArduinoHttpClient.h> // include Arduino HTTP Client library
#include <WiFiNINA.h> // include WiFi NINA module library
//#include "secret.h"

//Constants
#define ID  3 // Arduino unique ID
#define DHTPIN  1 // data PIN DHT-22 is connected to
#define SoilsensorPin A0 // data PIN Soil Moisture Sensor is connected to
#define DHTTYPE DHT22 // DHT Type: 22 (AM2302)
#define RANGERPIN 2 // define ultrasonic ranger data pin

// Air sensor
float air_hum;  //Stores humidity value
float air_temp; //Stores temperature value

// Globals
char ssid[] = "GoTan.13";
char pass[] = "";
const char serverAddress[] = "192.168.43.215";  // Server address
int port = 8000; // Port number

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// initialize WiFi and HTTP Clients
WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

// initialize ultrasonic library
Ultrasonic ultrasonic(RANGERPIN);
// Water Stock dimensions
int radius = 4;
int height = 9;
int consumed; // Consumed water


// Soil sensor
float SoilsensorValue;
int SoilLimit = 300;
int soil_hum = 30;  // Soil humidity
int soil_temp = 20; // Soil temperature

void setup() {

  Serial.begin(9600);
  while(!Serial);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID)
    status = WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network
  }

  // print the SSID of the network you're connected to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Start the Air sensor (DHT22)
  dht.begin();
  // Setup the Arduino output pin for the soil sensor
  pinMode(13, OUTPUT);
}

void loop() {
  sensor_air();
  sensor_water();
  sensor_soil();
  post_request();
  lance_pompe();
}

void sensor_air() {
  // int chk; 
  delay(2000);
  //Read data from Air sensor (DHT22) and store it to variables hum and temp
  air_hum = dht.readHumidity();
  air_temp = dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(air_hum);
  Serial.print(" %, Temp: ");
  Serial.print(air_temp);
  Serial.println(" Celsius");
}

void sensor_water() {
  char _buffer[7];
  int centimeters;
  int inches;

  // get distance in centimeters
  centimeters = ultrasonic.MeasureInCentimeters();
  delay(250); // wait 250 milliseconds between readings

  // print distance in cm
  sprintf(_buffer, "%03u cm", centimeters);
  consumed = (centimeters * radius * radius);
  Serial.print("\rDistance: ");
  Serial.println(_buffer);  // print distance (in cm) on serial monitor
}

/**
  * // to preserve the sensor pins:
  *    either start sensor 5 seconds before read
  *    or 
  *    thicken the pcb 
  */
void sensor_soil() {

  SoilsensorValue = analogRead(SoilsensorPin); 
  Serial.println("Soil humidite: ");
  Serial.println(SoilsensorValue);
 
  if (SoilsensorValue < SoilLimit) {
    digitalWrite(13, HIGH); 
  } else {
    digitalWrite(13, LOW); 
  }
  delay(1000);
}

void lance_pompe() {
  if(air_temp > 30 && soil_hum < 50.0) {
    // lancer la pompe
  } else {
    // arreter la pompe
  }
}

void post_request() {
  String path = "/";
  String contentType = "application/json";

  // id=ID; info=air-water-soil 
  // assemble the body of the POST message:
  String postData = "{\"id\":\""; 
  postData += String(ID);
  postData += "\",\"info\":{\"water\":\"";
  postData += String(12);
  postData += "\",\"air\":\"";
  postData += String(11);
  postData += "\",\"soil\":\"";
  postData += String(soil_hum);
  postData += "\"}}";

  Serial.println(postData); // Print the body of the POST message

  Serial.println("making POST request");
  client.post(path, contentType, postData); // send the POST request

  // read the status code and body of the response:
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  // Print the status code and body of the response from the web server:
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  delay(5000);
}
