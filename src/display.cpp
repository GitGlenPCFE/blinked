#include "mbed.h"
#include "constants.h"
#include "vt100.h"
#include "display.h"
#include "LCDi2c.h"

LCDi2c lcd(I2C_SDA, I2C_SCL, LCD16x2, 0x27);

extern things_t myData;

void displayTask() {
    DigitalOut green(GREENLED);
    printf("\033[2J");  // Clear the terminal display
    ThisThread::sleep_for(100);
    printf("\033[?25l"); // Hide the Visible Cursor

    printf("\033[1;20H\033[1;37mEnvironmental Sensing\033[0;37m");
    ThisThread::sleep_for(100);

    while (true) {
      ThisThread::sleep_for(500);
      printf("\033[8;3HThe LED is %s\n",
             myData.buttonState ? "\033[1;32mOn \033[0;37m" 
                                : "\033[2;32mOff\033[0;37m");
      if (myData.buttonState == true) {
        green = true;
      } else {
        green = false;
      }
      ThisThread::sleep_for(1);
      printf("\033[10;3HTemperature: %2.1fC \n", myData.tempC);
      printf("\033[10;1H%s", myData.heaterState
                             ? "\033[41m \033[40m"
                             : "\033[42m \033[40m" );
      printf("\033[12;3HLight Level: %3.0f%c \033[J", myData.lightL, '%');
      printf("\033[12;1H%s", myData.lightState
                             ? "\033[41m \033[40m"
                             : "\033[42m \033[40m" );
      printf("\033[11;3HSet Light Level: %3.0f%c", myData.setLightLevel, '%');
      printf("\033[11;40HTemp set to: %3.1fc", myData.setTemp);

      ThisThread::sleep_for(500);
      
      // LCD temperature prefix, setting & level arrangement
      lcd.locate(0, 0); // Set cursor to datum on first row
      lcd.printf("TS=%2.1f%c TL=%2.1f", myData.setTemp, 0xDF, myData.tempC); // Show Temp Set & Lev to 1dp
      // LCD light prefix, setting & level arrangement
      lcd.locate(0, 1); // Set cursor to datum on second row
      lcd.printf("LS=%3.0f%c  LL=%3.0f", myData.setLightLevel,'%', myData.lightL); // Show Light Set & Lev to 0dp
    }
}