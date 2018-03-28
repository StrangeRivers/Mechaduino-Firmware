#ifndef __UTILS_H__
#define __UTIL_H__
#include <cstdint>

  const int num_bytes = 32;
  extern uint8_t S1_received_bytes[num_bytes];
  
  void serialUSBCheck();            //checks serial port for commands.  Must include this in loop() for serial interface to work

  bool serial1Check(uint8_t receive_buffer[]);              //checks serial port for commands.  Must include this in loop() for serial interface to work

  void showNewData();               //reprints Serial1 incoming data on SerialUSB for debug

  float bytes_2_float(uint8_t *bytes_array[]);

#endif
