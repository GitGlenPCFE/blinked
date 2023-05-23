#include "mbed.h"
#include "constants.h"
#include "button.h"
#include "wifiTask.h" // Added to incorporate STATUSLED_TOPIC

extern things_t myData;
DigitalIn userButton(PUSH_BUTTON);
void readButtonState() {
    /* set stuff up */
    bool lastSwitchState = userButton;
    bool pressed = false;
    while(true){ /* loop forever */
        ThisThread::sleep_for(50);
        if (userButton == true) {
            pressed = true;
        }
        else if (userButton == false && pressed == true) {
            myData.buttonState = !myData.buttonState;
            pressed = false;
            sendPub(STATUSLED_TOPIC, myData.buttonState); // Sends push button state change (0.0 or 1.0) to broker
        }
    }
}
