#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "env.h"
#include "sensor-helpers.h"
#include "thermo-sensor.h"
#include "distance-sensor.h"
#include "moisture-sensor.h"
#include "relay.h"

const char* mqtt_server = MQTT_SERVER;
#define CHECK_TEMPERATURE_TOPIC "#"

void reconnect();

WiFiClient wifiClient;
PubSubClient client(wifiClient);
String macAddress;
double last_check = millis();

void subscriber(char* topic, uint8_t* payload, unsigned int length){
  Serial.println(String("Received ") + topic);
  if(String(topic) == String(CHECK_TEMPERATURE_TOPIC)) {
    read_temp_helper(message_t());
  }
}

message_t wifi_connect(message_t msg, fn_call resolve, void (*reject)()){
  wl_status_t status = WiFi.begin(SSID, PASSWORD);
  macAddress = WiFi.macAddress();

  // Wait for connection
  long start = millis();
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
    if(millis() - start > RETRY_TIME){
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
  delay(100);
}

void (*arduino_sleep)() = TO_DEEP_SLEEP ? deep_sleep : shallow_sleep;

message_t assign_relay(message_t message){
  return assign_pin(RELAY_PIN, message);
}

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

message_t read_distance_helper(message_t message){
  fn_call distance_fx[] = {
    assign_relay,
    turn_on,
    assign_echo,
    clear_message,
    init_distance_sensor,
    read_distance,
    create_publish_message,
    assign_relay,
    turn_off
  };

  message = assign_pin(TRIGGER_PIN, message);
  client.publish("distances", run_chain(distance_fx, 9, message).message.c_str());
  return message;
}

message_t read_temp_helper(message_t message){
  fn_call thermo_fx[] = {
    clear_message,
    assign_dht,
    init_thermo_sensor,
    debug_message,
    read_temp_and_humidity,
    debug_message,
    free_thermo_sensor,
    create_publish_message
  };

  message = run_chain(thermo_fx, 8, message);
  Serial.println("Publishing: " + message.message);
  client.publish("temperatures", message.message.c_str());
  return message;
}

message_t read_moisture_helper(message_t message){
  fn_call moisture_fx[] = {
    clear_message,
    assign_analog,
    read_moisture,
    create_publish_message
  };

  message = assign_pin(TRIGGER_PIN, message);
  client.publish("moisture", run_chain(moisture_fx, 4, message).message.c_str());
  return message;
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

  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(subscriber);
}

void loop(void){
  if (!client.connected())
    reconnect();

  message_t message;
  delay(2000);
  message = read_moisture_helper(message);
  Serial.println(message.message);
  last_check = millis();

  arduino_sleep();
}
