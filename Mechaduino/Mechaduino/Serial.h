#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <cstdint>

  const int num_bytes = 32;
  extern uint8_t rx_buffer_S1[num_bytes];
  
  void serialUSBCheck();            //checks serial port for commands.  Must include this in loop() for serial interface to work

  bool serial1Check(uint8_t receive_buffer[]);              //checks serial port for commands.  Must include this in loop() for serial interface to work

  void parseNewData(uint8_t receive_buffer[]);

  void sendData(char key, float value);

#endif
