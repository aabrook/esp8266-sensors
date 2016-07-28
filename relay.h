#ifndef RELAY_H
#define RELAY_H

#include "sensor-helpers.h"

message_t wrap_switch(message_t, fn_call);
message_t turn_on(message_t);
message_t turn_off(message_t);

#endif
