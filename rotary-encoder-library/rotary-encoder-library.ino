

#include "rotaryEncodeSensor.h"

#define PULSES_PER_REVOLUTION 1200
#define INTERRUPT_PIN 2
#define ENCODER_TYPE ONE_WIRE
#define SENSOR_NAME "flow-meter"

rotaryEncodeSensor flowMeter(PULSES_PER_REVOLUTION, INTERRUPT_PIN, ENCODER_TYPE, SENSOR_NAME);

#include <autoDelay.h>

autoDelay printDelay;
#define PRINT_DELAYTIME_mS 2000

void setup() {
  Serial.begin(115200);
  flowMeter.begin();
  attachInterrupt(digitalPinToInterrupt(flowMeter.interruptPinA), ISR_A, CHANGE);  // interrupt 0 is pin 2
}

void loop() {
  //  flowMeter.convertPosDeg();
  //  flowMeter.convertPosRad();
  flowMeter.analyseSignal();

  if (printDelay.millisDelay(PRINT_DELAYTIME_mS)) {
    //   flowMeter.printStats();
    flowMeter.printSignalStats();
  }
}


void ISR_A() {
  flowMeter._isr_A();
}
