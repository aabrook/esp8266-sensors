#include "moisture-sensor.h"

message_t read_moisture(message_t msg){
  pinMode(msg.pin, msg.action);
  int reading = analogRead(msg.pin);

  msg.message = String("\"m\": \"") + reading + "\"";
  return msg;
}
