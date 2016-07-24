#include "distance-sensor.h"

message_t* init_distance_sensor(message_t* message){
  return message;
}

message_t* read_distance(message_t* message){
  pinMode(message->pin, OUTPUT);
  pinMode(message->action, INPUT);
  digitalWrite(message->pin, LOW);
  delayMicroseconds(2);
  digitalWrite(message->pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(message->pin, LOW);
  delayMicroseconds(5);

  long duration = pulseIn(message->action, HIGH);
  message->message = String("\"d\": \"") + duration + "\"";

  return message;
}
