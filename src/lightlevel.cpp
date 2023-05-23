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
    // Read the light dependent resistor (situated above lighting) voltage
        myData.lightL = lightLevelVoltage.read() * 100; // Range of ADC.read 0->1
        if ((myData.lightL > myData.setLightLevel + 5.0f) && (myData.shieldPercentClosed < 100.0)) { // If light level is 5 above setting & shields not fully closed, do next 5 lines
            myData.lightState = false; // Ensure/turn the lights off
            myData.shieldPercentClosed += 1.0; // Close shields by 1% increment up to site limits (100% max program limit)
            RGBgreen = 0; // Light state out of tolerance (above)
            sendPub(LIGHT_STATE_TOPIC, myData.lightState); // Sends lightState change (0.0) to broker
            sendPub(LIGHT_SHIELD_TOPIC, myData.shieldPercentClosed); // Send shield closed %age to broker
        }
        else if ((myData.lightL < myData.setLightLevel - 5.0f) && (myData.shieldPercentClosed > 0.0)) { // If light level is 5 below setting & shields not fully open, do next 3 lines
            myData.shieldPercentClosed -= 1.0; // Open shields by 1% increment up to site limits (0% min program limit)
            RGBgreen = 0; // Light state out of tolerance (below)
            sendPub(LIGHT_SHIELD_TOPIC, myData.shieldPercentClosed); // Send shield closed %age to broker
        }
        else if (myData.lightL < myData.setLightLevel - 5.0f) {
            myData.lightState = true; // Ensure/turn the lights on (lumens configured to setLightLevel) as available natural light is too low
            RGBgreen = 0; // Light state out of tolerance (below)
            sendPub(LIGHT_STATE_TOPIC, myData.lightState); // Sends lightState change (1.0) to broker
        }
        else { // If none of above were actioned, then do next line
            RGBgreen = 1; // Light state within tolerance
        }

        if (pubRate++ > PUB_PERIOD) {
            sendPub(LIGHT_LEVEL_TOPIC, myData.lightL);
            pubRate = 0;
        }
        ThisThread::sleep_for(500);
    }
}
