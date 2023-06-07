/*  rotary-encoder-angle

    Uses interrupts to track the angle in pulses, degrees and radians of a 4 wire rotary encoder.


*/

#include <autoDelay.h>
#include <avr/dtostrf.h>  // For some reason SAMD21 libraries does not include this 

#define ENCODER_RED power
#define ENCODER_BLACK ground
#define ENCODER_WHITE 2
#define ENCODER_GREEN 3

#define ROTARY_PPR 1200  // Pulse per (half) revolution




volatile int encoderPos = 0;
volatile int encoderPosLast = 0;
int lastReportedPos = 1;
bool A_set = false;
bool B_set = false;

volatile float encoderDeg;
volatile float encoderRad;

autoDelay printDelay;


void setup() {
  Serial.begin(115200);
  pinMode(ENCODER_WHITE, INPUT_PULLUP);
  pinMode(ENCODER_GREEN, INPUT_PULLUP);
  Serial.println("Starting Penduino - Encoder Only");
  attachInterrupt(digitalPinToInterrupt(ENCODER_WHITE), doEncoderA, CHANGE);  // interrupt 0 is pin 2
  attachInterrupt(digitalPinToInterrupt(ENCODER_GREEN), doEncoderB, CHANGE);  // interrupt 0 is pin 2
}


void loop() {
  convertPosDeg();
  convertPosRad();

  char buffer[64];
  char timeString[16];
  char degString[12];
  char radString[12];

  dtostrf(secondsElapsed(), 8, 3, timeString);   // Convert float to char string as sprintf() does not work with floats in arduino
  dtostrf(encoderDeg, 8, 2, degString);   // Convert float to char string as sprintf() does not work with floats in arduino
  dtostrf(encoderRad, 8, 2, radString);

  sprintf(buffer, "%s -> %5i, %s deg, %s rad.", timeString, encoderPos, degString, radString);   // Do it this way to keep coloumns of data in line.

  Serial.println(buffer);
}

float secondsElapsed(){
  float seconds = millis()/1000.0;
  return seconds;
}


void convertPosDeg(){
encoderDeg = (360.0/float(ROTARY_PPR*2))*float(encoderPos);
}

void convertPosRad(){
encoderRad = ((2*PI)/float(ROTARY_PPR*2))*float(encoderPos);
}

// Interrupt on A changing state
void doEncoderA() {
  // Test transition
  A_set = digitalRead(ENCODER_WHITE) == HIGH;     // A_set = 1 if digitalRead == HIGH else 0
  // and adjust counter + if A leads B
  encoderPosLast = encoderPos;
  encoderPos += (A_set != B_set) ? +1 : -1;      // encoderPos +  IF (A_set is not equal to B_set) -> 1, ELSE -> -1
  encoderWrap();  
}

// Interrupt on B changing state
void doEncoderB() {
  // Test transition
  B_set = digitalRead(ENCODER_GREEN) == HIGH;
  // and adjust counter + if B follows A
  encoderPosLast = encoderPos;
  encoderPos += (A_set == B_set) ? +1 : -1;
  encoderWrap();
}

void encoderWrap(void){
  if (encoderPos > ROTARY_PPR) {
    encoderPos -= ROTARY_PPR*2;
    } else if (encoderPos < -ROTARY_PPR) {
      encoderPos += ROTARY_PPR*2;
      }
}