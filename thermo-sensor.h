#ifndef THERMO_SENSOR_H
#define THERMO_SENSOR_H

#include "sensor-helpers.h"

message_t init_thermo_sensor(message_t);
message_t free_thermo_sensor(message_t);

message_t read_temp(message_t);
message_t read_humidity(message_t);
message_t read_temp_and_humidity(message_t);

#endif
