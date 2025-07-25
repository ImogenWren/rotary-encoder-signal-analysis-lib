/*  rotary-encoder-angle

    Uses interrupts to track the angle in pulses, degrees and radians of a 4 wire rotary encoder.


https://gammon.com.au/interrupts  <- Finally a good resource on AVR interrupts! (unfortunatly we are in SAMD terriritory now)

*/

#include "rotaryEncodeSensor.h"
#include <autoDelay.h>
#include <avr/dtostrf.h>  // For some reason SAMD21 libraries does not include this

#ifdef ARDUINO_ARCH_SAMD
#include "wiring_private.h"
#endif


#define ENCODER_RED power
#define ENCODER_BLACK ground
#define ENCODER_WHITE 13  // interrupt 3 // Pin A7 -> Numeric Pin Number: 13
#define ENCODER_GREEN A7  // interrupt 1 // Pin D13 -> Numeric Pin Number: 21
#define ROTARY_PPR 600    // Pulse per (half) revolution
#define ENCODER_TYPE TWO_WIRE
#define SENSOR_NAME "encoder"

#define RED_LED A5

#define LEVEL_SHIFT_ENABLE_PIN A1



rotaryEncodeSensor encoder(ROTARY_PPR, ENCODER_WHITE, ENCODER_GREEN, ENCODER_TYPE, SENSOR_NAME);


autoDelay printDelay;

bool ISR_A_TRIGGERED = false;
bool ISR_B_TRIGGERED = false;



#ifdef ARDUINO_ARCH_SAMD

void configureEIC() {
  // Enable GCLK for EIC
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_EIC) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // Wait for sync

  // Enable APB clock for EIC
  PM->APBCMASK.bit.EIC = 1;  // Enable EIC periphera//PM->APBCMASK.reg |= PM_APBCMASK_EIC;

  // Manually configure PB03 (EXTINT3) and PA17 (EXTINT1) for external interrupt
  PORT->Group[1].PINCFG[3].reg |= PORT_PINCFG_PMUXEN;   // PB03
  PORT->Group[0].PINCFG[17].reg |= PORT_PINCFG_PMUXEN;  // PA17

  // Set EIC to detect BOTH RISING & FALLING edges (CHANGE)
  EIC->CONFIG[0].reg |= EIC_CONFIG_SENSE3_BOTH;  // PB03 (EXTINT3)
  EIC->CONFIG[1].reg |= EIC_CONFIG_SENSE1_BOTH;  // PA17 (EXTINT1)

  // Enable interrupts for EXTINT3 (PB03) and EXTINT1 (PA17)
  EIC->INTENSET.reg = EIC_INTENSET_EXTINT3 | EIC_INTENSET_EXTINT1;

  // Enable EIC
  EIC->CTRL.bit.ENABLE = 1;
  while (EIC->STATUS.bit.SYNCBUSY)
    ;  // Wait for sync

  // Enable NVIC for EIC
  NVIC_EnableIRQ(EIC_IRQn);
  NVIC_SetPriority(EIC_IRQn, 3);  // Set priority
}

#endif

void setup() {
  Serial.begin(115200);
  encoder.begin();
  // while (!Serial.available()) {
  //   ;
  // }
  //delay(2000);
  while (!Serial)
    ;
  Serial.println("SAMD Angle Example");
  // pinMode(ENCODER_WHITE, INPUT_PULLUP);
  //  pinMode(ENCODER_GREEN, INPUT_PULLUP);
  Serial.println("Starting - Encoder Only");
  pinMode(LEVEL_SHIFT_ENABLE_PIN, OUTPUT);
  digitalWrite(LEVEL_SHIFT_ENABLE_PIN, true);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, false);

#ifdef ARDUINO_ARCH_SAMD
  configureEIC();
#else
  attachInterrupt(digitalPinToInterrupt(ENCODER_WHITE), isr_A, CHANGE);  // interrupt 0 is pin 2
  attachInterrupt(digitalPinToInterrupt(ENCODER_GREEN), isr_B, CHANGE);  // interrupt 0 is pin 2
#endif
}


void loop() {
  //  convertPosDeg();
  // convertPosRad();
  if (printDelay.millisDelay(2000)) {
    Serial.println("is isr blocking>?");
  }
  encoder.convertPosDeg();
  encoder.convertPosRad();
  encoder.printPosStats();

  if (ISR_A_TRIGGERED) {
    //  Serial.println("isr A Triggered");
    ISR_A_TRIGGERED = false;
  }

  if (ISR_B_TRIGGERED) {
    //  Serial.println("isr B Triggered");
    ISR_B_TRIGGERED = false;
  }

  // char buffer[64];
  // char timeString[16];
  // char degString[12];
  // char radString[12];

  // dtostrf(secondsElapsed(), 8, 3, timeString);  // Convert float to char string as sprintf() does not work with floats in arduino
  // dtostrf(encoder., 8, 2, degString);         // Convert float to char string as sprintf() does not work with floats in arduino
  // dtostrf(encoderRad, 8, 2, radString);

  // sprintf(buffer, "%s -> %5i, %s deg, %s rad.", timeString, encoderPos, degString, radString);  // Do it this way to keep coloumns of data in line.

  /// Serial.println(buffer);
}

float secondsElapsed() {
  float seconds = millis() / 1000.0;
  return seconds;
}


#ifdef ARDUINO_ARCH_SAMD
//  The main interrupt handler for ALL external interrupts
void EIC_Handler() {
  // Check which interrupt flag is set and call the respective function

  if (EIC->INTFLAG.bit.EXTINT3) {  // PB03 Interrupt
    EIC->INTFLAG.bit.EXTINT3 = 1;  // Clear flag
    encoder._isr_A();
  }

  if (EIC->INTFLAG.bit.EXTINT1) {  // PA17 Interrupt
    EIC->INTFLAG.bit.EXTINT1 = 1;  // Clear flag
    encoder._isr_B();
  }
}
#else
// Interrupt on A changing state
void isr_A() {
  encoder._isr_A();
  ISR_A_TRIGGERED = true;
}

// Interrupt on B changing state
void isr_B() {
  encoder._isr_B();
  ISR_B_TRIGGERED = true;
}
#endif
