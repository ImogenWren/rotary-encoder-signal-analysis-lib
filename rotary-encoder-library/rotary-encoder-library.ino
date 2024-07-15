/* # Rotary Encoder Library

Library is written to analyse digital signals common to various rotary encoders.

Example sketch designed for RS Pro Radial Flow Turbine Flow Meter:
https://uk.rs-online.com/web/p/flow-sensors/0257149
This has two options:
257-149 - Grey liquid flow sensor,0.25 to 6.5 l/min, 500Hz, 4600 pulses/litre
257-133 - Beige liquid flow sensor,1.5 to 30 l/min, 600Hz, 1200 pulses/litre

Imogen Heard
15/07/2024

*/

#include "rotaryEncodeSensor.h"

#define PULSES_PER_LITRE 1200   // For typical rotary encoder this would be PULSES_PER_REVOLUTION, but this is a special use case, all methods that return a Revolutions-Per-X, instead will give value for Litres-per-X
#define INTERRUPT_PIN 2
#define ENCODER_TYPE ONE_WIRE
#define SENSOR_NAME "flow-meter"

rotaryEncodeSensor flowMeter(PULSES_PER_LITRE, INTERRUPT_PIN, ENCODER_TYPE, SENSOR_NAME);

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
  flowMeter.analyseSignal();    // Must analyse signal in loop to get other datas accuratly

  if (printDelay.millisDelay(PRINT_DELAYTIME_mS)) {
    //   flowMeter.printStats();
    flowMeter.printSignalStats();
    Serial.print("Litres Per Second: ");
    Serial.print(flowMeter.calculateSpeed_rps());
    Serial.print("  Litres Per Minute: ");
    Serial.print(flowMeter.calculateSpeed_rpm());
    Serial.println();
  }
}


void ISR_A() {
  flowMeter._isr_A();
}
