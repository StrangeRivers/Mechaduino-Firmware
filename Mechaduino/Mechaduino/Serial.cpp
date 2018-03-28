
#include "Utils.h"
#include "State.h"
#include "Serial.h"

#include <SPI.h>

uint8_t S1_received_bytes[];
uint8_t num_received = 0;

boolean new_data = false;

bool serial1Check(uint8_t *receive_buffer) {
  
    static boolean receive_in_progress = false;
    static uint8_t ndx = 0;
    
    uint8_t start_marker = 0x3C;
    uint8_t end_marker = 0x3E;
    uint8_t rb;
  
    while (Serial1.available() > 0 && new_data == false) {
      
        rb = Serial1.read();
                 
        if (receive_in_progress == true) {
          
            if (rb != end_marker) {
                receive_buffer[ndx] = rb;
                ndx++;
                if (ndx >= num_bytes) {
                    ndx = num_bytes - 1;
                }
            }
            
            else {
                receive_buffer[ndx] = '\0'; // terminate the string
                receive_in_progress = false;
                num_received = ndx;  // save the number for use when printing
                ndx = 0;
                new_data = true;
            }
        }

        else if (rb == start_marker) {
            receive_in_progress = true;
        }
    }

  return new_data;
}

void showNewData() {
    if (new_data == true) {
        
        //SerialUSB.print("This just in (HEX values)... ");
        
//        for (byte n = 0; n < num_received; n++) {
//            SerialUSB.print(S1_received_bytes[n], HEX);
//            SerialUSB.print(' ');
//        }
        SerialUSB.println();
        new_data = false; // clear the S1 receive buffer
    }
}

void parseNewData(uint8_t *receive_buffer[]) {
  
  if (new_data == true) {

      switch (receive_buffer[0]) {

        case 'r':
          uint8_t temp_buffer[4];
          
          memcpy(temp_buffer, receive_buffer, 4);
//          for (int i=1; i <= sizeof(receive_buffer); i++) {
//            temp_buffer[i] = receive_buffer[i];
//          }

          r = bytes_2_float(&temp_buffer);
          
        break;
      }
      
    }
}


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

void float_2_bytes(float val, uint8_t *bytes_array){
  
  // Create union of shared memory space
  union {
    float float_variable;
    uint8_t temp_array[4];
  } u;
  
  // Overwrite bytes of union with float variable
  u.float_variable = val;
  
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}

float bytes_2_float(uint8_t *bytes_array){
  
  // Create union of shared memory space
  union {
    float float_variable;
    uint8_t temp_array[4];
  } u;
  
  // Overwrite bytes of union with float variable
  memcpy(u.temp_array, bytes_array, 4);
  
  // Assign bytes to input array
  return u.float_variable;
  
}


