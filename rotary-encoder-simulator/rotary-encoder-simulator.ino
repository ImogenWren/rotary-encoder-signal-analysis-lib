/*
 Outputs a square wave pulse to simulate a single channel pulsed rotary encoder

*/

#define DEBUG_OUTPUT true

#define OUTPUT_PIN 2

#define ROTATION_HZ 0.00093
#define PULSE_DUTY 0.8

#define PULSES_PER_REVOLUTION 1200

float pulse_Hz = ROTATION_HZ * PULSES_PER_REVOLUTION;
uint32_t period_uS = 1.0 / float(pulse_Hz) * 1000000;
uint32_t timeHigh_uS = period_uS * PULSE_DUTY;
uint32_t timeLow_uS = period_uS - timeHigh_uS;


void setup() {
  Serial.begin(115200);
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.println("\nRotary Encoder Simulator");
  Serial.print("Rotation Hz: ");
  Serial.print(ROTATION_HZ);
  Serial.print(", Pulse Hz: ");

  Serial.print(", Duty: ");
  Serial.print(PULSE_DUTY);
  Serial.print(", Period(uS): ");
  Serial.print(period_uS);
  Serial.print(", timeHigh(uS):");
  Serial.print(timeHigh_uS);
  Serial.print(", timeLow(uS):");
  Serial.print(timeLow_uS);
  Serial.println();
  delay(5000);
}

bool current_op_state = false;
bool last_op_state = true;
uint32_t lastEvent_uS = 0;
uint32_t delayTime_uS = timeLow_uS;

uint32_t testTime = 0;
int32_t timeDifference;

uint16_t printLoop = 1200;
uint16_t iteration = 0;

void loop() {
  if (micros() - lastEvent_uS >= delayTime_uS) {
    last_op_state = current_op_state;
    current_op_state = !current_op_state;
    digitalWrite(OUTPUT_PIN, current_op_state);
    testTime = micros() - lastEvent_uS;
    lastEvent_uS = micros();

#if DEBUG_OUTPUT == true
    if (iteration >= printLoop) {
      Serial.print("Timing Test: State:  ");
      Serial.print(current_op_state);
      Serial.print(", Time (uS): ");
      Serial.print(testTime);
      Serial.print(", delayTime (uS): ");
      Serial.print(delayTime_uS);
      Serial.print(", Difference (uS): ");
      timeDifference = testTime - delayTime_uS;
      Serial.println(timeDifference);
      if (iteration >= printLoop + 3) {
        iteration = 0;
      }
    }
#endif
    if (current_op_state) {
      delayTime_uS = timeHigh_uS;
    } else {
      delayTime_uS = timeLow_uS;
    }
  }

  iteration++;
}
