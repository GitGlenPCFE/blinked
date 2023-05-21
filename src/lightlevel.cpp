/* This function reads the thermistor voltage and returns the temperature in Celsius
   using Steinhart Hart conversion from thermistor resistance
 */
#include "mbed.h"
#include "constants.h"
#include "lightlevel.h"
#include "wifiTask.h"

AnalogIn lightLevelVoltage( lightOut );
DigitalOut RGBgreen(LIGHTOKLED);
extern things_t myData;

void readLightLevel() {
    int pubRate = 0;
    while(true) {
    // Read the light dependent resistor voltage
        myData.lightL = lightLevelVoltage.read() * 100; // Range of ADC.read 0->1
        if (myData.lightL > myData.setLightLevel + 5.0f) {
            myData.lightState = false; // turn the lights off
        }
        else if (myData.lightL < myData.setLightLevel - 5.0f) {
            myData.lightState = true;  // turn the lights on
        }
        if (pubRate++ > PUB_PERIOD) {
            sendPub(LIGHT_LEVEL_TOPIC, myData.lightL);
            sendPub(LIGHT_STATE_TOPIC, myData.lightState);
            pubRate = 0;
        }
        ThisThread::sleep_for(500);
    }
}
