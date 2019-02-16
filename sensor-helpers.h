#ifndef SENSOR_HELPERS_H
#define SENSOR_HELPERS_H

#include <Arduino.h>

/*
 * Define a uniform structure to use when passing between states
 */
typedef struct {
  String message;
  int pin;
  int action;
} message_t;

/* Base function definition that we will use for all things */
typedef message_t (*fn_call)(message_t);

/* Use a 'next' to chain to the next function */
typedef message_t (*fn_chain)(message_t, fn_call);

/* Apply alternate processes to go to a 'success' or 'failed' state */
typedef message_t (*fn_tracks)(message_t, fn_call, fn_call);

message_t clear_message(message_t);
message_t assign_pin(int, message_t);
message_t assign_action(int, message_t);
message_t debug_message(message_t);

/* Run the functions as a chain */
message_t run_chain(fn_call fx[], int count, message_t msg);

#endif
