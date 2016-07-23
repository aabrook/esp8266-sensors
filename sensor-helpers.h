#ifndef SENSOR_HELPERS_H
#define SENSOR_HELPERS_H

#include <arduino.h>

/*
 * Define a uniform structure to use when passing between states
 */
typedef struct {
  String message;
  int pin;
  int action;
} message_t;

/* Base function definition that we will use for all things */
typedef message_t* (*fn_call)(message_t*);

/* Use a 'next' to chain to the next function */
typedef message_t* (*fn_chain)(message_t*, fn_call);

/* Apply alternate processes to go to a 'success' or 'failed' state */
typedef message_t* (*fn_tracks)(message_t*, fn_call, fn_call);

message_t* clear_message(message_t*);
fn_call assign_pin(int, message_t*, fn_call);
fn_call assign_high_low(int, message_t*, fn_call);
#endif
