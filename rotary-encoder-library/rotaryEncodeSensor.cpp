

#include "rotaryEncodeSensor.h"


void rotaryEncodeSensor::begin() {
  if (encoderType == ONE_WIRE || encoderType == TWO_WIRE) {
    pinMode(interruptPinA, INPUT_PULLUP);
    //  attachInterrupt(digitalPinToInterrupt(ENCODER_WHITE), doEncoderA, CHANGE);  // interrupt 0 is pin 2
  }
  if (encoderType == TWO_WIRE) {
    pinMode(interruptPinB, INPUT_PULLUP);
    //  attachInterrupt(digitalPinToInterrupt(ENCODER_GREEN), doEncoderB, CHANGE);  // interrupt 0 is pin 2
  }
}




void rotaryEncodeSensor::printPosStats() {
  char buffer[64];
  char timeString[16];
  char degString[12];
  char radString[12];
  dtostrf(secondsElapsed(), 8, 3, timeString);  // Convert float to char string as sprintf() does not work with floats in arduino
  dtostrf(encoderDeg, 8, 2, degString);         // Convert float to char string as sprintf() does not work with floats in arduino
  dtostrf(encoderRad, 8, 2, radString);
  sprintf(buffer, "%s -> %5i, %s deg, %s rad.", timeString, encoderPos, degString, radString);  // Do it this way to keep coloumns of data in line.
  Serial.println(buffer);
}



void rotaryEncodeSensor::printSignalStats() {
  char buffer[64];
  char timeString[16];
  char timeHighString[12];
  char timeLowString[12];
  char periodString[12];
  char HzString[12];
  char dutyString[12];
  dtostrf(secondsElapsed(), 8, 3, timeString);  // Convert float to char string as sprintf() does not work with floats in arduino
                                                // dtostrf(timeHighString, 8, 2, timeHighString);         // Convert float to char string as sprintf() does not work with floats in arduino
  dtostrf(signal_period, 4, 2, dutyString);
  sprintf(buffer, "%s -> High: %5i, Low: %5i, Period: %3i, Hz: %3i, Duty: %s", timeString, signal_time_high, signal_time_low, signal_period, signal_Hz, dutyString);  // Do it this way to keep coloumns of data in line.
  Serial.println(buffer);
}

float rotaryEncodeSensor::secondsElapsed() {
  float seconds = millis() / 1000.0;
  return seconds;
}



void rotaryEncodeSensor::convertPosDeg() {
  encoderDeg = (360.0 / float(pulses_per_revolution * 2)) * float(encoderPos);
}

void rotaryEncodeSensor::convertPosRad() {
  encoderRad = ((2 * PI) / float(pulses_per_revolution * 2)) * float(encoderPos);
}


void rotaryEncodeSensor::analyseSignal() {
  if (ISR_triggered) {
    if (!A_state) {  // if A_state is currently low, then we are calculating the HIGH period
      signal_time_low = currentSampleTime_uS - previousSampleTime_uS;
    } else {
      signal_time_high = currentSampleTime_uS - previousSampleTime_uS;
    }
    signal_period = signal_time_low + signal_time_high;
    signal_Hz = 1.0 / (signal_period / 10000000);
    signal_duty = float(signal_period) / float(signal_time_high);
  //  signal_duty = signal_time_high / signal_time_low;
    ISR_triggered = false;
  }
}


void rotaryEncodeSensor::calculateSpeed_radSec() {
}

void rotaryEncodeSensor::calculateSpeed_rpm() {
}

void rotaryEncodeSensor::calculateSpeed_rps() {
}




void rotaryEncodeSensor::encoderWrap(void) {
  if (encoderPos > pulses_per_revolution) {
    encoderPos -= pulses_per_revolution * 2;
  } else if (encoderPos < (-1 * pulses_per_revolution)) {
    encoderPos += pulses_per_revolution * 2;
  }
}

void rotaryEncodeSensor::_isr_A() {
  A_state = digitalRead(interruptPinA) == HIGH;
  previousSampleTime_uS = currentSampleTime_uS;
  currentSampleTime_uS = micros();
  encoderLastPos = encoderPos;
  // could below be done outside of ISR?
  encoderPos += (A_state == B_state) ? +1 : -1;
  rotaryEncodeSensor::encoderWrap();
  ISR_triggered = true;
}