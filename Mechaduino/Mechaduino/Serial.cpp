
#include "Utils.h"
#include "State.h"

#include <SPI.h>

void serialUSBCheck() {        //Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.

  if (SerialUSB.available()) {

    char inChar = (char)SerialUSB.read();

    switch (inChar) {


      case 'p':             //print
        print_angle();
        break;

      case 's':             //step
        oneStep();
        print_angle();
        break;

      case 'd':             //dir
        if (dir) {
          dir = false;
        }
        else {
          dir = true;
        }
        break;

      case 'w':                //old command
        calibrate();           //cal routine
        break;
        
      case 'c':
        calibrate();           //cal routine
        break;        

      case 'e':
        readEncoderDiagnostics();   //encoder error?
        break;

      case 'y':
        enableTCInterrupts();      //enable closed loop
        break;

      case 'n':
        disableTCInterrupts();      //disable closed loop
        break;

      case 'r':             //new setpoint
        SerialUSB.println("Enter setpoint:");
        while (SerialUSB.available() == 0)  {}
        r = SerialUSB.parseFloat();
        SerialUSB.println(r);
        break;

      case 'x':
        mode = 'x';           //position loop
        break;

      case 'v':
        mode = 'v';           //velocity loop
        break;

      case 't':
        mode = 't';           //torque loop
        break;

      case 'h':               //hybrid mode
        mode = 'h';
        break;

      case 'q':
        parameterQuery();     // prints copy-able parameters
        break;

      case 'a':             //anticogging
        antiCoggingCal();
        break;

      case 'k':
        parameterEditmain();
        break;
        
      case 'g':
        sineGen();
        break;

      case 'm':
        serialMenu();
        break;
        
      case 'j':
        stepResponse();
        break;

      case 'z':
        zero();
        break;

      default:
        break;
    }
  }

}

void serial1Check() {
  if (Serial1.available()) {
    char inChar = (char)Serial1.read();

    
  }
}


