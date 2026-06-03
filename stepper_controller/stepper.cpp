#include "stepper.h"
#include <string.h>

StepperMotor::StepperMotor(int dirPin, int stepPin){
    _dirPin = dirPin;
    _stepPin = stepPin;
    _stepHigh = false;

    _requestedDir = false;
    _appliedDir = false;

    // // TODO update stepInterval to depend upon the desired velocity

    _lastStepTime = micros();
};
void StepperMotor::setup(){
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);

    digitalWrite(_stepPin, LOW);
    digitalWrite(_dirPin, LOW);

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
    _stepInterval = velocity;
};

void StepperMotor::setDirection(bool requestedDir){
    _requestedDir = requestedDir;
};