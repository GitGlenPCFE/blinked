/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

// Blinking rate in milliseconds
#define BLINKING_RATE     2s
#define thermPower P10_3
#define thermGround P10_0
#define thermOut P10_1



/* Reference resistor in series with the thermistor is of 10kohm */
#define R_REFERENCE                         (float)(10000)

/* A constant of NCP18XH103F03RB thermistor is */
#define A_COEFF                          (float)(0.0009032679f)
/* B constant of NCP18XH103F03RB thermistor is */
#define B_COEFF                          (float)(0.000248772f)
/* C constant of NCP18XH103F03RB thermistor is */
#define C_COEFF                          (float)(0.0000002041094f)

/* Zero Kelvin in degree C */
#define ABSOLUTE_ZERO                       (float)(-273.15)


// Initialise the digital pin LED1 as an output
DigitalOut led(LED1);
DigitalOut vdd(thermPower);
DigitalOut gnd(thermGround);
AnalogIn temperatureVoltage( thermOut );

// prototype for function calls
float readTemp();

int main()
{
    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
        gnd = false;
        vdd = true;
        float temperature = readTemp();
        vdd = false;

        printf("The temperature is: %2.1fC\n", temperature);
    }
}
float readTemp() {
    float refVoltage = temperatureVoltage.read() * 2.4; // Range of ADC 0->2*Vref
    float refCurrent = refVoltage  / 10000.0; // 10k Reference Resistor
    float thermVoltage = 3.3 - refVoltage;    // Assume supply voltage is 3.3v
    float thermResistance = thermVoltage / refCurrent; 
    float logrT = (float32_t)log((float64_t)thermResistance);
    /* Calculate temperature from the resistance of thermistor using Steinhart-Hart Equation */
    float stEqn = (float32_t)((A_COEFF) + ((B_COEFF) * logrT) + 
                             ((C_COEFF) * pow((float64)logrT, (float32)3)));
    float temperatureC = (float32_t)(((1.0 / stEqn) + ABSOLUTE_ZERO)  + 0.05);
    return temperatureC;
}

