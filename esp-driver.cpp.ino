#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "env.h"
#include "sensor-helpers.h"
#include "thermo-sensor.h"
#include "relay.h"
#include "either.h"

const char* mqtt_server = MQTT_SERVER;
#define CHECK_TEMPERATURE_TOPIC "#"

void reconnect();

WiFiClient wifiClient;
PubSubClient client(wifiClient);
String macAddress;
double last_check = millis();

message_t wifi_connect(message_t msg, fn_call resolve, void (*reject)()){
  wl_status_t status = WiFi.begin(SSID, PASSWORD);
  macAddress = WiFi.macAddress();

  // Wait for connection
  long start = millis();
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
    if(millis() - start > RETRY_TIME){
      Serial.print("Failed to connect. Will retry soon");
      WiFi.disconnect(true);
      reject();
      return message_t();
    }
  }

  return resolve(msg);
}

message_t debug_wifi(message_t msg){
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(SSID);

  Serial.println("Reader started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  return msg;
}

void deep_sleep(){
  ESP.deepSleep(DEEP_SLEEP);
}

void shallow_sleep(){
  WiFi.disconnect(true);
  delay(DELAY_SLEEP);
}

void (*arduino_sleep)() = TO_DEEP_SLEEP ? deep_sleep : shallow_sleep;

message_t create_publish_message(message_t message){
  message.message = (String("{ \"r\": \"") + ROOM + "\", " + message.message + "}");
  return message;
}

message_t assign_echo(message_t message){
  return assign_action(ECHO_PIN, message);
}

message_t assign_dht(message_t message){
  return assign_pin(DHTPIN, message);
}

message_t assign_analog(message_t message){
  return assign_pin(A0, message);
}

message_t debug(message_t message){
  Serial.println(message.message);

  return message;
}

void errored(message_t* message){
  Serial.println("Something went wrong. " + message->message);
}

void publish(message_t* message){
  Serial.println("Publishing: " + message->message);
  String res = "" + client.publish("temperatures", message->message.c_str());
}

message_t read_temp_helper(message_t message){
  Serial.println("Preparing to read temperature");

  Right<message_t>* result = (new Right<message_t>(message))
    ->fmap(&clear_message)
    ->fmap(&debug)
    ->fmap(&assign_dht)
    ->fmap(&init_thermo_sensor)
    ->fmap(&debug)
    ->fmap(&read_temp_and_humidity)
    ->fmap(&debug)
    ->fmap(&free_thermo_sensor)
    ->fmap(&create_publish_message);

  result->fork(&errored, &publish);
  message_t data = result->getData();
  delete result;

  return data;
}

void reconnect() {
  // Loop until we're reconnected
  long startReconnect = millis();

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(macAddress.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe(CHECK_TEMPERATURE_TOPIC);

    } else {
      if(millis() - startReconnect > RETRY_TIME){
        arduino_sleep();
        return;
      }

      delay(5000);
    }
  }
}

void setup(void){
  Serial.begin(115200);
  WiFi.disconnect(true);

  delay(3000);
  Serial.print("Starting\n");

  // Connect to WiFi network
  wifi_connect(message_t(), debug_wifi, arduino_sleep);
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    Serial.println("Connecting to WIFI");

    wifi_connect(message_t(), debug_wifi, arduino_sleep);
  }


  Serial.println("Setting MQTT server");
  client.setServer(mqtt_server, MQTT_PORT);
  Serial.println("Setup complete");
}

void loop(void){
  Serial.println("Looping");
  wifi_connect(message_t(), debug_wifi, arduino_sleep);
  if (!client.connected())
    reconnect();

  message_t message;

  Serial.println("waiting");
  delay(2000);

  Serial.println("Reading");
  message = read_temp_helper(message);
  Serial.println(message.message);
  last_check = millis();

  arduino_sleep();
}
