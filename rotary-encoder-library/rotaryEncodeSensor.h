
/*   rotaryEncodeSensor.h

  arduino/c++ library for sensing of rotary encoders & other Pulsed signals

  Imogen Heard
    11/07/2024


*/

#pragma once

#ifndef rotaryEncodeSensor_h
#define rotaryEncodeSensor_h



#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



// define type for rotary encoder
typedef enum {
  UNDEFINED,
  ONE_WIRE,
  TWO_WIRE,
  THREE_WIRE
} encoderTypes;




class rotaryEncodeSensor {


public:


  // Constructor

  rotaryEncodeSensor(uint16_t _pulses_per_revolution = 1200, const int _interruptPin = 2, encoderTypes _encoderType = ONE_WIRE, const char _moduleID[32] = { "encode" })
    : pulses_per_revolution(_pulses_per_revolution),
      interruptPinA(_interruptPin),
      encoderType(_encoderType) {
    strcpy(moduleName, _moduleID);
    instance = this;                // Store the instance pointer
        attachInterrupt(digitalPinToInterrupt(interruptPinA), _isr_A, CHANGE);  // interrupt 0 is pin 2
  }

  // Constants
  int16_t pulses_per_revolution;
  int interruptPinA;
  int interruptPinB;
  encoderTypes encoderType = ONE_WIRE;


  // Variables
  char moduleName[32] = { "rotary-encoder" };

  bool ISR_triggered = false;



  // Signal Variables

  bool A_state = false;  // Last state of the A input
  bool B_state = false;
  uint32_t signal_time_high = 0;
  uint32_t signal_time_low = 0;
  uint32_t signal_period_uS;
  float signal_Hz = 0.0;
  float signal_duty = 0.0;

  // Encoder Position Variables
  int16_t encoderPos;
  int16_t encoderLastPos;
  int16_t encoderDeg;
  int16_t encoderRad;

  // Encoder velocity
  int16_t encoder_rpm;
  int16_t encoder_rps;
  int16_t encoder_radSec;
  int16_t encoder_degSec;





  // Constants


  //Data Structure


  typedef enum {
    UNDEFINED,
    CLOCKWISE,
    COUNTER_CLOCKWISE
  } direction;

  direction currentDirection = UNDEFINED;

  // struct dataArray {
  //   uint16_t i_data[8];        // Always holds the direct ADC value read from data aquisition module
  //   float f_data[8];           // holds the calculated voltage or current value, or a copy of the ADC as a float
  //   uint32_t timeStamp_mS[8];  // holds the time data was sampled in mS from time of power up
  // } d_array;

  // Digital Data Structures


  // Functional Methods
  void begin();
  void analyseSignal();


  // API - Use to interact with library
  void printSignalStats();
  float getFreq();
  float getDuty();
  float calculateSpeed_rps();
  float calculateSpeed_rpm();

  void calibrate_position();



  void convertPosDeg();

  void convertPosRad();

  void calculateSpeed_radSec();



  void printPosStats();





  void _isr_A();




private:

  float secondsElapsed();
  void encoderWrap();


  uint32_t currentSampleTime_uS = 0;
  uint32_t previousSampleTime_uS = 0;
  int16_t posOffset = 0;

  static rotaryEncodeSensor* instance;

  static void isrHandler();
};


#endif