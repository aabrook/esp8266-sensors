#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "env.h"
#include "sensor-helpers.h"
#include "DHT.h"
#include <HttpClient.h>
#include "SSD1306.h"

DHT dht(DHTPIN, DHT22);

WiFiClient wifiClient;
String macAddress;

double last_check = 0;
// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, 2, 5);

double lastTemperature = 0.0;

void display_temperature() {
  display.clear();
  int range = (lastTemperature / 50) * 100;
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawProgressBar(0, 32, 120, 15, range);
  display.drawString(64, 15, String(ROOM) + ": " + String(lastTemperature));
  display.display();
}

void displayMessage(String message, int delayTime = 1500) {
  display.clear();
  display.drawString(0, 0, message);
  display.display();
  delay(delayTime);
}

bool wifi_connect(){
  WiFi.begin(SSID, PASSWORD);
  macAddress = WiFi.macAddress();
  String loader = "Starting";

  // Wait for connection
  long start = millis();
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    delay(500);
    loader = loader + ".";
    display.clear();
    display.drawString(0, 0, loader);
    display.display();

    Serial.print(".");
    if(millis() - start > RETRY_TIME){
      Serial.print("Failed to connect. Will retry soon");
      displayMessage("Failed to connect");

      WiFi.disconnect(true);
      return false;
    }
  }

  displayMessage(String("Connected to network\n") + SSID, 3000);
  return true;
}

message_t create_publish_message(message_t message){
  message.message = String(ROOM) + "/" + message.message;
  return message;
}

String errorMessage(int message) {
  switch(message) {
    case HTTP_SUCCESS: return "Success";
    case HTTP_ERROR_CONNECTION_FAILED: return "Connection failed";
    case HTTP_ERROR_API: return "ERROR API";
    case HTTP_ERROR_TIMED_OUT: return "Request timed out";
    case HTTP_ERROR_INVALID_RESPONSE: return "Invalid response";
    default: return "Error Unknown";
  }
}
void publish(message_t message){
  HttpClient httpClient(wifiClient);
  int result = httpClient.get(PUBLISH_SERVER, PUBLISH_PORT, (String("/") + message.message).c_str());

  Serial.println("Published to pi: [" + errorMessage(result) + "] " + httpClient.responseStatusCode());
  if (httpClient.responseStatusCode() != 200 || result != HTTP_SUCCESS) {
    displayMessage(String("Publish failed: ") + httpClient.responseStatusCode() + "\n" + errorMessage(result) + "\n", 3000);
    String response = httpClient.readString();
    Serial.println("Server response: " + response);
    displayMessage(response, 3000);
  }
} 

message_t read_temp(message_t message){
  Serial.println("Preparing to read temperature");
  lastTemperature = dht.readTemperature(false);
  message.message = String(lastTemperature) + "/" + String(dht.readHumidity());

  return message;
}

void setup(void){
  Serial.begin(115200);
  WiFi.disconnect(true);

  bool initialised = display.init();
  Serial.println("Intialised? " + String(initialised));

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  displayMessage("Starting", 3000);

  Serial.println("Starting");

  // Connect to WiFi network
  // wifi_connect();

  dht.begin();
}


void loop(void){
  if (millis() - last_check > DELAY_SLEEP || last_check == 0) {
    Serial.println("Prepare read");
    wifi_connect();

    message_t message;

    displayMessage("Reading...");

    Serial.println("Reading");
    message = read_temp(message);

    if (WiFi.status() == WL_CONNECTED && WiFi.isConnected()) {
      displayMessage("Publishing...", 0);
      message = create_publish_message(message);
      publish(message);
    } else {
      displayMessage("No WiFi connection");
    }
 
    Serial.println(message.message);

    WiFi.disconnect(true);

    last_check = millis();
  }

  display_temperature(); 
}
