/* This function reads the thermistor voltage and returns the temperature in
   Celsius using Steinhart Hart conversion from thermistor resistance
 */
#include "tempSense.h"
#include "constants.h"
#include "mbed.h"
#include "wifiTask.h"

//DigitalOut vdd(thermPower);
//DigitalOut gnd(thermGround);
AnalogIn temperatureVoltage(thermOut);
DigitalOut RGBred(TEMPHIGHLED);
DigitalOut RGBblue(TEMPLOWLED);
extern things_t myData;

void readTemp() {
    int pubRate = 0;
  //  Apply power to the thermistor
  //    gnd = false;
  //    vdd = true;
  //    vdd = false;
  while (true) {
    // Read the thermistor voltage
    float refVoltage =
        temperatureVoltage.read() * 2.4;     // Range of ADC 0->2*Vref
                                             // Cut power to thermistor
    float refCurrent = refVoltage / 10000.0; // 10k Reference Resistor
    float thermVoltage = 3.3 - refVoltage;   // Assume supply voltage is 3.3v
    float thermResistance = thermVoltage / refCurrent;
    float logrT = (float32_t)log((float64_t)thermResistance);
    /* Calculate temperature from the resistance of thermistor using
     * Steinhart-Hart Equation */
    float stEqn = (float32_t)((A_COEFF) + ((B_COEFF)*logrT) +
                              ((C_COEFF)*pow((float64)logrT, (float32)3)));
    float temperatureC = (float32_t)(((1.0 / stEqn) + ABSOLUTE_ZERO) + 0.05);
    myData.tempC = temperatureC;
    if (myData.tempC > myData.setTemp + 1.0f) { // If temp is 1 degC above setting then 
        myData.heaterState = false; // Ensure/turn the heater off
        RGBred = 1; RGBblue = 0; // RGB LED: Red on, blue off (to show temp is too high/hot)
        sendPub(HEATER_STATE_TOPIC, myData.heaterState); // Sends heaterState change (0.0) to broker
        sendPub(REDLED_TOPIC, RGBred); // Sends red LED overheated state (1.0) to broker (activating fans)
    }
    else if (myData.tempC < myData.setTemp - 1.0f) { // If t
        myData.heaterState = true; // Ensure/turn the heater on
        RGBred = 0; RGBblue = 1; // RGB LED: Red off, blue on (to show temp is too low/cold)
        sendPub(HEATER_STATE_TOPIC, myData.heaterState); // Sends heaterState change (1.0) to broker
        sendPub(REDLED_TOPIC, RGBred); // Sends red LED state (0.0) to broker (ensuring fans are off)
    }
    else { // If none of above were actioned, then do next line
        RGBred = 0; RGBblue = 0; // RGB LED: Red off, blue off (to show temp is within tolerance)
        sendPub(REDLED_TOPIC, RGBred); // Sends red LED state (0.0) to broker (ensuring fans are off)
    }

    if (pubRate++ > PUB_PERIOD) {
        sendPub(TEMPERATURE_TOPIC, myData.tempC);
        pubRate = 0;
    }

    ThisThread::sleep_for(500);

  }

  //    return temperatureC;
}
