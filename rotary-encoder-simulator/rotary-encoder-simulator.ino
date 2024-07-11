/*
 Outputs a square wave pulse to simulate a single channel pulsed rotary encoder

*/

#define PULSE_HZ 10
#define PULSE_DUTY 0.5

uint32_t timeHigh_mS = ((1.0/float(PULSE_HZ))*PULSE_DUTY)*1000;
uint32_t timeLow_mS = (1.0 - ((1.0/float(PULSE_HZ))*PULSE_DUTY))*1000;


void setup() {
Serial.begin(115200);
Serial.println("Rotary Encoder Simulator");
Serial.print("Pulse Hz: ");
Serial.print(PULSE_HZ);
Serial.print(", Duty: ");
Serial.print(PULSE_DUTY);
Serial.print(", timeHigh(mS):");
Serial.print(timeHigh_mS);
Serial.print(", timeLow(mS):");
Serial.print(timeLow_mS);
Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

}
