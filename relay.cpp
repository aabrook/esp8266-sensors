#include "relay.h"

message_t wrap_switch(message_t message, fn_call next){
  return turn_off(next(turn_on(message)));
}

message_t turn_off(message_t message){
  pinMode(message.pin, OUTPUT);
  digitalWrite(message.pin, HIGH);

  return message;
}

message_t turn_on(message_t message){
  pinMode(message.pin, OUTPUT);
  digitalWrite(message.pin, LOW);

  return message;
}
