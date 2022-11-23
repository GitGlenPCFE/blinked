/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "stdio.h"
#include "constants.h"
#include "tempSense.h"
#include "lightlevel.h"

// Initialise the digital pin LED1 as an output
DigitalOut led(LED1);

int main()
{
    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
        float temperature = readTemp();
        printf("The temperature is: %2.1fC\n", temperature);
        float lightLevel = readLightLevel();
        printf("The light level is: %2.1f%c\n", lightLevel, '%');
    }
}