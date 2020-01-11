#include "DHT2.h"

#ifndef _ALT_DHT_H
#define _ALT_DHT_H_

dht DHT2;

double readHumidity() {
    DHT2.read22(A0);

    Serial.print("Current humidity = ");
    Serial.print(DHT2.humidity);
    Serial.print("%  ");

    return DHT2.humidity;
}

double readTemperature() {
    DHT2.read22(A0);
    
    Serial.print("temperature = ");
    Serial.print(DHT2.temperature); 
    Serial.println("C  ");

    return DHT2.temperature;
}

#endif