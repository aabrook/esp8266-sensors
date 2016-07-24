#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "env.h"
#include "sensor-helpers.h"
#include "thermo-sensor.h"
#include "distance-sensor.h"

const char* mqtt_server = MQTT_SERVER;

void reconnect();

WiFiClient wifiClient;
PubSubClient client(wifiClient);
String macAddress;

message_t* wifi_connect(message_t* msg, fn_call resolve, void (*reject)()){
  wl_status_t status = WiFi.begin(SSID, PASSWORD);
  macAddress = WiFi.macAddress();

  // Wait for connection
  long start = millis();
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
    if(millis() - start > RETRY_TIME){
      reject();
    }
  }

  return resolve(msg);
}

message_t* debug_wifi(message_t* msg){
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
  delay(DELAY_SLEEP);
}

message_t* create_message(){
  return new message_t();
}

message_t* delete_message(message_t* message){
  delete message;
  return 0;
}

void (*arduino_sleep)() = TO_DEEP_SLEEP ? deep_sleep : shallow_sleep;

message_t* create_publish_message(message_t* message){
  message->message = (String("{ \"r\": \"") + ROOM + "\", " + message->message + "}");
}

void setup(void){
  Serial.begin(115200);

  delay(3000);
  Serial.print("Starting\n");

  // Connect to WiFi network
  message_t* message = new message_t();
  wifi_connect(message, debug_wifi, arduino_sleep);

  delete message;
  client.setServer(mqtt_server, MQTT_PORT);
}

void loop(void){
  if (!client.connected())
    reconnect();

  message_t* message = create_message();

  assign_pin(DHTPIN, message);
  fn_call thermo_fx[] = {
    clear_message,
    init_thermo_sensor,
    read_temp_and_humidity,
    free_thermo_sensor,
    create_publish_message
  };
  client.publish("temperatures", run_chain(thermo_fx, 5, message)->message.c_str());

  fn_call distance_fx[] = {
    clear_message,
    init_distance_sensor,
    read_distance,
    create_publish_message
  };
  assign_pin(TRIGGER_PIN, assign_action(ECHO_PIN, message));
  client.publish("distances", run_chain(distance_fx, 4, message)->message.c_str());

  delete_message(message);
  arduino_sleep();
}

message_t* run_chain(fn_call fx[], int count, message_t* message){
  for(int i = 0; i < count; ++i){
    fx[i](message);
  }

  return message;
}

void reconnect() {
  // Loop until we're reconnected
  long startReconnect = millis();

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(macAddress.c_str())) {
      Serial.println("connected");

    } else {
      if(millis() - startReconnect > RETRY_TIME){
        arduino_sleep();
        return;
      }

      delay(5000);
    }
  }
}
