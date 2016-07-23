#include "sensor-helpers.h"

message_t* clear_message(message_t* message){
  message->message = "";

  return message;
}

fn_call assign_pin(int pin, message_t* message, fn_call cb){
  message->pin = pin;
  return cb;
}

fn_call assign_high_low(int action, message_t* message, fn_call cb){
  message->action = action;
  return cb;
}
