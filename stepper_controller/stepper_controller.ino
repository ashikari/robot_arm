#include "stepper.h"
#include <IntervalTimer.h>

IntervalTimer timer;

StepperMotor m1(DIR_PIN, STEP_PIN);

void stepperInteruptServiceRoutine(){
  m1.update();
}

void setup() {
  m1.setup();
  m1.setVelocity(30);

  timer.begin(stepperInteruptServiceRoutine, 10);
}

void loop() {
  delay(5000);
  m1.setDirection(true);
  delay(5000);
  m1.setDirection(false);
}
