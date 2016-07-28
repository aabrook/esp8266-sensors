#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H
#include "sensor-helpers.h"

message_t init_distance_sensor(message_t);

message_t read_distance(message_t);

#endif
