#include "stepper.h"
#include <string.h>

StepperMotor::StepperMotor(int dirPin, int stepPin){
    _dirPin = dirPin;
    _stepPin = stepPin;
    _stepState = false;

    // // TODO update stepInterval to depend upon the desired velocity
    // _stepInterval = 500;

    _lastStepTime = micros();
};
void StepperMotor::setup(){
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    setVelocity(0.0);
    setDirection(false);
};

void StepperMotor::update(){
    uint32_t currentTime = micros();
    Serial.print("Update fn: ");
    Serial.println(currentTime);
    if (_velocity > 0 and (currentTime - _lastStepTime) > _stepInterval){
        Serial.println("Inside");
        digitalWrite(_stepPin, !_stepState);

        // update state variables
        _stepState = !_stepState;
        _lastStepTime = currentTime;

        // TODO add counter update
    }

};

void StepperMotor::setVelocity(float velocity){
    _velocity = velocity;
    _stepInterval = velocity;
};

void StepperMotor::setDirection(bool clockwise){
    _clockwise = clockwise;
    digitalWrite(_dirPin, _clockwise);
};