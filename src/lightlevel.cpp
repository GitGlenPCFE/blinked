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
std::string lightState = myData.lightState ? "ON" : "OFF";

void readLightLevel() {
    int pubRate = 0;
    while(true) {
    // Read the light dependent resistor (situated above lighting) voltage
        myData.lightL = lightLevelVoltage.read() * 100; // Range of ADC.read 0->1
        if (myData.lightL > myData.setLightLevel + 5.0f) {
            myData.lightState = false; // Turn the lights off
            RGBgreen = 0; // Light state out of tolerance (above)
            sendPub(LIGHT_STATE_TOPIC, myData.lightState); // Sends lightState change to broker
        }
        else if (myData.lightL < myData.setLightLevel - 5.0f) {
            myData.lightState = true; // Turn the lights on (lumens configured to setLightLevel)
            RGBgreen = 0; // Light state out of tolerance (below)
            sendPub(LIGHT_STATE_TOPIC, myData.lightState); // Sends lightState change to broker
        }
        else { // If none of above were actioned, then do next line
            RGBgreen = 1; // Light state within tolerance
        }

        if (pubRate++ > PUB_PERIOD) {
            sendPub(LIGHT_LEVEL_TOPIC, myData.lightL);
            sendPub(LIGHT_SHIELD_TOPIC, myData.shieldPercentClosed); // Send shield closed %age to broker
            pubRate = 0;
        }
        ThisThread::sleep_for(500);
    }
}
