// #include "thermo-sensor.h"
// #include "DHT.h"
// #include "env.h"


// message_t init_thermo_sensor(message_t init){
//   //dht = DHT(init.pin, DHT22);
//   return init;
// }

// message_t free_thermo_sensor(message_t message){
//   // delete dht;
//   return message;
// }

// message_t read_temp(message_t message){
//   message.message += "\"t\": \""
//     + String(dht.readTemperature())
//     + "\"";

//   return message;
// }

// message_t read_humidity(message_t message){
//   message.message += "\"h\": \""
//     + String(dht.readHumidity())
//     + "\"";

//   return message;

// }

// message_t read_temp_and_humidity(message_t message){
//   String temp_result = read_temp(clear_message(message)).message;
//   String humidity_result = read_humidity(clear_message(message)).message;

//   message.message = temp_result + "," + humidity_result;

//   return message;
// }
