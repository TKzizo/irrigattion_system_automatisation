/*****************************************************/
/***************** Calling Libraries *****************/
#include <DHT.h> // include DHT library
#include <Wire.h> // include Arduino Wire library
#include "Ultrasonic.h" // include Seeed Studio ultrasonic ranger library
#include <ArduinoHttpClient.h> // include Arduino HTTP Client library
#include <WiFiNINA.h> // include WiFi NINA module library
#include <Arduino_JSON.h> // use json functions
//#include "secret.h"  //  SSID and Password


/******************************************************************/
/***************** Constants and Global Variables *****************/
// Constants
#define ID  3 // Arduino unique ID
#define DHTPIN  1 // data PIN DHT-22 is connected to
#define SoilsensorPin A0 // data PIN Soil Moisture Sensor is connected to
#define SoilPinOut 5 // Setup the Arduino output pin for the soil sensor
#define DHTTYPE DHT22 // DHT Type: 22 (AM2302)
#define RANGERPIN 2 // define ultrasonic ranger data pin
#define RelayPin1 3 // define The 1st relay output pin
#define RelayPin2 4 // define the 2nd relay output pin

// Global Variables
char ssid[] = "SSID"; // Wifi name (SSID)
char pass[] = "PASSWORD"; // Wifi password
const char serverAddress[] = "IP";  // The Web Server (RaspBerry PI) IP address
int port = "PORT" ; // The Web Server (RaspBerry PI) Port number

// Air sensor
float air_hum;  //Stores humidity value
float air_temp; //Stores temperature value
float MaxTemp = 30.0; // Max temperature
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

WiFiClient wifi; // Initialize WiFi module
HttpClient client = HttpClient(wifi, serverAddress, port); // Initialize HTTP (Web) Client
int status = WL_IDLE_STATUS;

// Initialize ultrasonic library
Ultrasonic ultrasonic(RANGERPIN);
// Water Stock dimensions
int radius = 2;
int height = 9;
int consumed; // Consumed water

// Soil sensor
int SoilsensorValue = 1023;
int SoilValue;
int SoilLimit = 600;

/******************************************************/
/***************** The SETUP Function *****************/
/******************************************************/
void setup() {

  Serial.begin(9600);
  while(status != WL_CONNECTED) {
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
  pinMode(SoilPinOut, OUTPUT);
  // Setup the 2 relay module output pin
  pinMode(RelayPin1, OUTPUT);
  //pinMode(RelayPin2, OUTPUT);
}
/******************************************************/

/***************** The Loop Function ******************/
/******************************************************/
void loop() {
  sensor_air();
  sensor_water();
  sensor_soil();
  //post_request();
  lance_pompe();
}
/******************************************************/

/******************************************************/
/***************** Sensors Functions ******************/
/******************************************************/
void sensor_air() {
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

  centimeters = ultrasonic.MeasureInCentimeters(); // get distance in centimeters
  delay(250); // wait 250 milliseconds between readings

  // Print the distance in centimeters between the Ultrasonic sensor and water to the Serial monitor
  sprintf(_buffer, "%03u cm", centimeters);
  consumed = (centimeters * radius * radius * 3.14); // The consumed water amount
  Serial.print("\rDistance: ");
  Serial.println(_buffer);  // print distance (in cm) on serial monitor
  Serial.print("\rConsumed: ");
  Serial.print(consumed * 0.001);
  Serial.println(" Litre");
}

/**
  * // to preserve the sensor pins:
  *    either start sensor 5 seconds before read
  *    or 
  *    thicken the pcb 
  */
void sensor_soil() {
  SoilsensorValue = analogRead(SoilsensorPin); 
  Serial.print("Soil humidite: ");
  SoilValue = 100 - (SoilsensorValue * 100 / 1023);
  Serial.print(SoilValue);
  Serial.println(" %");
 
  if (SoilsensorValue < SoilLimit) {
    digitalWrite(SoilPinOut, HIGH); 
  } else {
    digitalWrite(SoilPinOut, LOW); 
  }
}
/**********************************************************/

/***************** Pump control Function ******************/
void lance_pompe() {
  if(air_temp > MaxTemp || SoilsensorValue > SoilLimit) {
    digitalWrite(RelayPin1, LOW);
  } else {
    digitalWrite(RelayPin1, HIGH);
  }
}
/******************************************************************/

/***************** Server communication Function ******************/
void post_request() {

  JSONVar thepost;

  String path = "/";
  String contentType = "application/json";
  
  thepost["id"] = ID;
  thepost["info"]["air"]["hum"] = air_hum;
  thepost["info"]["air"]["temp"] = air_temp;
  thepost["info"]["water"] = consumed;
  thepost["info"]["soil"] = SoilValue;
  Serial.println(thepost);
  Serial.println("making POST request");

  // send the POST request
  
  client.post(path, contentType, JSON.stringify(thepost));

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  delay(5000);
}
/******************************************************************/
/******************************************************************/
