#include <string>

#ifndef CONSTANTS
#define CONSTANTS
// Blinking rate in milliseconds
#define BLINKING_RATE     2000
#define PUB_PERIOD 20
#define thermPower P10_3
#define thermGround P10_0
#define thermOut P10_1
#define GREENLED P0_5
#define PUSH_BUTTON P10_2 // P0_4 doesn't work when wifi is running
#define lightOut P10_4
#define RXLED P12_3
#define TEMPLOWLED P10_0
#define TEMPHIGHLED P10_3
#define LIGHTOKLED P10_5

/* Reference resistor in series with the thermistor is of 10kohm */
#define R_REFERENCE                         (float)(10000)

/* A constant of NCP18XH103F03RB thermistor is */
#define A_COEFF                          (float)(0.0009032679f)
/* B constant of NCP18XH103F03RB thermistor is */
#define B_COEFF                          (float)(0.000248772f)
/* C constant of NCP18XH103F03RB thermistor is */
#define C_COEFF                          (float)(0.0000002041094f)

/* Zero Kelvin in degree C */
#define ABSOLUTE_ZERO                    (float)(-273.15)
struct things_t {
    float tempC;
    float setTemp = 22.0f;
    bool heaterState = false;
    float lightL;
    float setLightLevel = 22.0f;
    bool lightState = false;
    bool buttonState;
    int shieldPercentClosed = 50;
};

// LCD Clock & Data
// #define I2C_SCL P6_0
// #define I2C_SDA P6_1

#endif