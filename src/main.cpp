#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "*****";
const char* password = "*****";

int sensor = D1;

void setupWifi () {
  delay(1000);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void sendData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;    //Declare object of class HTTPClient
    String postData = "{ \"motion\": true }";

    http.begin("http://office-eyes.herokuapp.com/sensor/event/motion"); //Specify request destination
    http.addHeader("Content-Type", "application/json"); //Specify content-type header

    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void setup() {
    Serial.begin(9600);
    setupWifi();
    pinMode(sensor, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int sensor_value = digitalRead(sensor);
  if (sensor_value == 1) {
    Serial.println("- - - - - - - - - - M O V E M E N T - - - - - - - - - -");
    sendData();
  }
}
