
#include "Utils.h"
#include "State.h"
#include "Serial.h"

#include <arduino.h>

uint8_t rx_buffer_S1[];
uint8_t num_received = 0;
bool rx_ack_S1 = false;
const uint8_t start_marker = 0x3C;
const uint8_t end_marker = 0x3E;

bool new_data = false;

bool serial1Check(uint8_t *receive_buffer) {
  
    static bool receive_in_progress = false;
    static uint8_t ndx = 0;

    uint8_t rb;
  
    while (Serial1.available() > 0 && new_data == false) {

        rx_ack_S1 = false;
        rb = Serial1.read();

        //TODO: add counter to ignore partial messages
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

void parseNewData(uint8_t *receive_buffer) {

  // Current interface is a character byte followed by four bytes that make up a float - a key/value pair
  // would be great to update this in the future to be more general and able to handle more complex messages (JSON?)
  
  if (new_data == true) {
    uint8_t key = receive_buffer[0];
    union {float value; uint8_t convert[4];} u;

    for (uint8_t i=1; i < num_received; i++) {
      u.convert[i-1] = receive_buffer[i];
    }

    new_data = false; // overwrite the S1 rx buffer next round

    switch ((char)key) {

      case 'r': // incoming message is new setpoint
        r = u.value;
        SerialUSB.print("set r ");
        SerialUSB.println(u.value, DEC);
      break;

      case 'z':
        zero();
        //TODO: send a zeroing status report to the controller - via heartbeat
      break;
      
    }
  }
}

void sendData(char key, float value, int repeat) {
  
  union {float float_variable; uint8_t temp_array[4];} u;

  u.float_variable = value;

  for (int i = 0; i < repeat; i++) {
    Serial1.write(start_marker);
    Serial1.write(key);
    for (int i = 0; i<4; i++) {
      Serial1.write(u.temp_array[i]);
    }
    Serial1.write(end_marker);
  }
  
}

void sendStateMessage() {
  static float u_last = u;
  static float r_last = r;
  static float y_last = y;
  static float v_last = v;
  static float e_last = e;
  static float zero_angle_last = zero_angle;
  static char mode_last = mode;

  // check if state has changed and send an update to the controller
  // send some messages three times if they're unlikely to be updated often
  if (u != u_last)                    sendData('u', u, 1);
  if (r != r_last)                    sendData('r', r, 1);
  if (y != y_last)                    sendData('y', y, 1);
  if (v != v_last)                    sendData('v', v, 1);
  if (e != e_last)                    sendData('e', e, 1);
  if (zero_angle != zero_angle_last)  sendData('z', zero_angle, 3);
  if (mode != mode_last)              sendData('m', mode, 3);
  
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
