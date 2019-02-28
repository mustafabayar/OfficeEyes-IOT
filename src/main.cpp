#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "*****";
const char* password = "*****";

const char* ip = "*****";

const int trigPin = D1;
const int echoPin = D2;

// defines variables
long duration;
int distance;

int avgDistance = 0;
int count = 0;
int total = 0;

int totalConsecutiveOutlier = 0;

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

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void sendData(int avgDistance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;    //Declare object of class HTTPClient
    String dist = String(avgDistance);
    String postData = "{ \"motion\": true, \"distance\": " + dist + "}";

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
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  if (distance > 3 && distance < 500) {
    totalConsecutiveOutlier = 0;
    count = count + 1;
    // Prints to serial
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm");
    Serial.println();

    total = total + distance;

    if (count == 3) {
      avgDistance = total / 3;
      count = 0;
      if (avgDistance < 200) {
        Serial.print("---------Movement!---------");
        Serial.println();
        sendData(avgDistance);
      }
      total = 0;
    }

  } else {
    totalConsecutiveOutlier = totalConsecutiveOutlier + 1;
    Serial.print("Outlier: ");
    Serial.print(distance);
    Serial.print(" cm");
    Serial.println();

    if (totalConsecutiveOutlier > 100) {
      Serial.println("Sensor is Kaputt!");
      Serial.println();
    }
  }

  delay(1000);
}
