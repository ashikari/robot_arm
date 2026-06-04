#include "stepper.h"
#include <string.h>

StepperMotor::StepperMotor(int dirPin, int stepPin, int m1Pin, int m2Pin, int m3Pin){
    _dirPin = dirPin;
    _stepPin = stepPin;
    _m1Pin = m1Pin;
    _m2Pin = m2Pin;
    _m3Pin = m3Pin;

    _stepHigh = false;

    _requestedDir = false;
    _appliedDir = false;

    // // TODO update stepInterval to depend upon the desired velocity

    _lastStepTime = micros();
};
void StepperMotor::setup(){
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);

    pinMode(_m1Pin, OUTPUT);
    pinMode(_m2Pin, OUTPUT);
    pinMode(_m3Pin, OUTPUT);

    digitalWrite(_stepPin, LOW);
    digitalWrite(_dirPin, LOW);

    digitalWrite(_m1Pin, LOW);
    digitalWrite(_m2Pin, LOW);
    digitalWrite(_m3Pin, LOW);

    _requestedDir = false;
    _appliedDir = false;

    setVelocity(0.0);
    setDirection(false);
};

void StepperMotor::update(){
    uint32_t currentTime = micros();

    // The non-Leading Edge Code
    if (_stepHigh){
        if ((currentTime - _lastStepTime) >= STEP_PULSE_WIDTH_US){
            digitalWriteFast(_stepPin, LOW);
            _stepHigh = false;

            if (_requestedDir != _appliedDir){
                digitalWriteFast(_dirPin, _requestedDir);
                _appliedDir = _requestedDir;
            }
        }
        return;
    }
    if (_requestedDir != _appliedDir){
        digitalWriteFast(_dirPin, _requestedDir);
        _appliedDir = _requestedDir;
        return;
    }

    // leading edge code
    if (_velocity > 0 and (currentTime - _lastStepTime) >= _stepInterval){
        digitalWriteFast(_stepPin, HIGH);

        // update state variables
        _stepHigh = true;
        _lastStepTime = currentTime;

        // TODO add counter update
    }

};

void StepperMotor::setVelocity(float velocity){

    _velocity = velocity;
    _stepInterval = int(1 / (200 * velocity) * 1000000);
};

void StepperMotor::setDirection(bool requestedDir){
    _requestedDir = requestedDir;
};