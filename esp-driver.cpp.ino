#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "env.h"
#include "sensor-helpers.h"
#include "thermo-sensor.h"

const char* ssid     = SSID;
const char* password = PASSWORD;

const char* mqtt_server = MQTT_SERVER;

void reconnect();

WiFiClient wifiClient;
PubSubClient client(wifiClient);
String macAddress;

void setup(void){
  Serial.begin(115200);

  delay(3000);
  Serial.print("Starting\n");

  // Connect to WiFi network
  wl_status_t status = WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect: ");
  Serial.println(WiFi.macAddress());

  // Wait for connection
  long start = millis();
  while (WiFi.status() != WL_CONNECTED && !WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
    if(TO_DEEP_SLEEP){
      if(millis() - start > RETRY_TIME){
        ESP.deepSleep(DEEP_SLEEP);
        return;
      }
    }
  }

  client.setServer(mqtt_server, MQTT_PORT);

#ifdef DEBUG
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
#endif

  Serial.println("Reader started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(void){
  if (!client.connected())
    reconnect();

  message_t* message = new message_t();
  fn_call fx[] = {
    clear_message,
    assign_pin(DHTPIN, message, init_thermo_sensor),
    read_temp_and_humidity,
    free_thermo_sensor
  };
  run_chain(fx, 4, message);
  Serial.println(message->message);
  client.publish("temperatures", (String("{ \"r\": \"") + ROOM + "\", "
    + message->message + "}").c_str());

  if(TO_DEEP_SLEEP){
    ESP.deepSleep(DEEP_SLEEP);
  }else{
    delay(DELAY_SLEEP);
  }
  return;
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
      if(TO_DEEP_SLEEP){
        if(millis() - startReconnect > RETRY_TIME){
          ESP.deepSleep(DEEP_SLEEP);
          return;
        }
      }

      delay(5000);
    }
  }
}
