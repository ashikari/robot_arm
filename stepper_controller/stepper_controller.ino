#include "stepper.h"
#include <IntervalTimer.h>

IntervalTimer timer;

StepperMotor m1(DIR_PIN, STEP_PIN, M1_PIN, M2_PIN, M3_PIN);

void stepperInteruptServiceRoutine(){
  m1.update();
}

void setup() {
  m1.setup();
  m1.setVelocity(2);

  timer.begin(stepperInteruptServiceRoutine, 30);
}

void loop() {
  delay(5000);
  m1.setDirection(true);
  delay(5000);
  m1.setDirection(false);
}
