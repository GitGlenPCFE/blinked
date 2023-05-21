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
    if (myData.tempC > myData.setTemp + 1.0f) {
        myData.heaterState = false; // turn the heater off
    }
    else if (myData.tempC < myData.setTemp - 1.0f) {
        myData.heaterState = true;  // turn the heater on
    }
    if (pubRate++ > PUB_PERIOD) {
        sendPub(TEMPERATURE_TOPIC, myData.tempC);
        pubRate = 0;
    }

    ThisThread::sleep_for(500);

  }

  //    return temperatureC;
}
