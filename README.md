# esp8266-sensors

Messing around with C functional programming and arduinos! Trying to setup
chains of code that will run consecutively across my required sensors.

# Setup
cp/mv env.h.example to env.h and update the constants to match your environment and the device's location.

# Data Format
The data forwarded is in the format

```
{
  "t": "Temperature",
  "h": "Humidity",
  "r": "Room that you have specified in env.h"
}
```
