#include "sensor-helpers.h"

message_t clear_message(message_t message){
  message.message = "";

  return message;
}

message_t assign_pin(int pin, message_t message){
  message.pin = pin;
  return message;
}

message_t assign_action(int action, message_t message){
  message.action = action;
  return message;
}

message_t debug_message(message_t message){
  printf("PIN: %d ACTION: %d Message: %s\n",
    message.pin,
    message.action,
    message.message.c_str());

  return message;
}

message_t run_chain(fn_call fx[], int count, message_t msg){
  for(int i = 0; i < count; ++i){
    msg = fx[i](msg);
    Serial.println("Message: " + msg.message);
  }

  return msg;
}
