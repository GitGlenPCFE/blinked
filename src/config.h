#ifndef CONFIG
#define CONFIG
/* MQTT configuration file
 *
 * Set your unique THING_NAME
*/

#define BROKER          "192.168.1.176" // Or (test.mosquitto.org), (public.mqtthq.com), (broker.hivemq.com), etc
#define PORT            1883

#define THING_NAME      "gr_thing"

//#define TEMPERATURE_TOPIC   "/temperatureC"
//#define LIGHT_LEVEL_TOPIC   "/lightlevel"
#define ANNOUNCE_TOPIC      "/announce"
#define LIGHT_SET_TOPIC "gr_thing/lightSet"
#define TEMP_SET_TOPIC "gr_thing/tempSet"

#endif