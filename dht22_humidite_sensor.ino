#include <DHT.h>;
#include <Wire.h>        // include Arduino Wire library
#include "Ultrasonic.h"  // include Seeed Studio ultrasonic ranger library
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "secret.h"
#define ID 3

//Constants
#define DHTPIN 1     // data PIN DHT-22 is connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define RANGERPIN   2 // define ultrasonic ranger data pin

char ssid[] = SSID;
char pass[] = PASSWORD;

const char serverAddress[] = "192.168.43.215";  // server address
int port = 8000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE); 
// initialize ultrasonic library
Ultrasonic ultrasonic(RANGERPIN);


// Water Stock dimensions
int radius = 4 ;
int height = 9;
// Air sensor
float hum;  //Stores humidity value
float temp; //Stores temperature value

// Consumed water
int consumed;

// Soil humidity
const int soil = 30;


void setup()
{
  Serial.begin(9600);
  while(!Serial);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  dht.begin();
}

void loop()
{
 sensor_air();
 sensor_water();
 //post_request();

 // lance_pompe()
}

void sensor_air() {
    int chk; 
     delay(2000);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
  }

void sensor_water() {
   char _buffer[7];
  int centimeters;
  int inches;
 
  // get distance in centimeters
  centimeters = ultrasonic.MeasureInCentimeters();
  delay(250);  // wait 250 milliseconds between readings
  
 
  // print distance in cm
  sprintf( _buffer, "%03u cm", centimeters );
  consumed = (centimeters*radius*radius);
  Serial.print("\rDistance: ");
  Serial.println(_buffer);  // print distance (in cm) on serial monitor

 
}

void post_request() {

  String path = "/";
  String contentType = "application/json";
  
 // id info=air-water-soil
 
  // assemble the body of the POST message:
  String postData = "{\"id\":\""; 
  postData += String(ID);
  postData += "\",\"info\":{\"water\":\"";
  postData += String(12);
  postData += "\",\"air\":\"";
  postData += String(11);
  postData += "\",\"soil\":\"";
  postData += String(soil);
  postData += "\"}}";
  
  Serial.println(postData);

  Serial.println("making POST request");

  // send the POST request
  client.post(path, contentType, postData);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  delay(5000);
}

 



/*

void lance_pompe() {
  // formule pour lancer pompe
  // dependant de la plante
}


 * // to preserve the sensor pins:
 *    either start sensor 5 seconds before read
 *    or 
 *    thicken the pcb 
 *    
int sensorPin = A0; 
int sensorValue;  
int limit = 300; 

void setup() {
 Serial.begin(9600);
 pinMode(13, OUTPUT);
}

void loop() {

 sensorValue = analogRead(sensorPin); 
 Serial.println("Analog Value : ");
 Serial.println(sensorValue);
 
 if (sensorValue<limit) {
 digitalWrite(13, HIGH); 
 }
 else {
 digitalWrite(13, LOW); 
 }
 
 delay(1000); 
}
*/
