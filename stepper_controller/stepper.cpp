#include "stepper.h"
#include <string.h>

StepperMotor::StepperMotor(int dirPin, int stepPin, int m1Pin, int m2Pin, int m3Pin){
    // Store pin assignments
    _dirPin = dirPin;
    _stepPin = stepPin;
    _m1Pin = m1Pin;
    _m2Pin = m2Pin;
    _m3Pin = m3Pin;

    // Initialize pulse-tracking state
    _stepHigh = false;

    // Both direction fields start in the same state so no spurious DIR change fires on first update()
    _requestedDir = false;
    _appliedDir = false;

    // Seed the step timer so the first pulse isn't fired before setup() is called
    _lastStepTime = micros();
};
void StepperMotor::setup(){
    // Configure all driver control lines as outputs
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    pinMode(_m1Pin, OUTPUT);
    pinMode(_m2Pin, OUTPUT);
    pinMode(_m3Pin, OUTPUT);

    // Drive all pins to a known idle state (full-step mode, motor stopped)
    digitalWrite(_stepPin, LOW);
    digitalWrite(_dirPin, LOW);
    digitalWrite(_m1Pin, LOW);
    digitalWrite(_m2Pin, LOW);
    digitalWrite(_m3Pin, LOW);

    // Sync software direction state to the hardware idle state
    _requestedDir = false;
    _appliedDir = false;

    setVelocity(0.0);
    setDirection(false);
};

void StepperMotor::update(){
    uint32_t currentTime = micros();

    // --- Trailing edge: end an in-progress STEP pulse ---
    // While the STEP pin is HIGH we wait here until the minimum pulse width
    // has elapsed, then pull it LOW to complete the pulse.
    if (_stepHigh){
        if ((currentTime - _lastStepTime) >= STEP_PULSE_WIDTH_US){
            digitalWriteFast(_stepPin, LOW);
            _stepHigh = false;

            // Apply any pending direction change now that STEP is low.
            // Changing DIR immediately after the falling edge satisfies the
            // driver's DIR hold-time requirement.
            if (_requestedDir != _appliedDir){
                digitalWriteFast(_dirPin, _requestedDir);
                _appliedDir = _requestedDir;
            }
        }
        // Still within pulse width — nothing else to do this cycle
        return;
    }

    // --- Direction settling: apply a pending direction change before stepping ---
    // If DIR needs to change and we are not mid-pulse, update the pin and
    // return without firing a step so the driver has a full cycle to see the
    // new DIR level before the next leading edge.
    if (_requestedDir != _appliedDir){
        digitalWriteFast(_dirPin, _requestedDir);
        _appliedDir = _requestedDir;
        return;
    }

    // --- Leading edge: fire the next STEP pulse when the interval has elapsed ---
    if (_velocity > 0 and (currentTime - _lastStepTime) >= _stepInterval){
        digitalWriteFast(_stepPin, HIGH);

        // Record timestamp and mark pulse as active so the trailing-edge
        // section above will pull STEP low after STEP_PULSE_WIDTH_US
        _stepHigh = true;
        _lastStepTime = currentTime;

        // TODO add step counter update
    }

};

void StepperMotor::setVelocity(float velocity){
    // Disable interrupts while updating the two variables that update() reads
    // together, so the ISR never sees a new velocity paired with a stale interval.
    noInterrupts();
    _velocity = velocity;
    // Convert rev/s → microseconds per step.
    // At 200 full steps/rev: interval (µs) = 1,000,000 / (200 × velocity)
    _stepInterval = int(1 / (200 * velocity) * 1000000);
    interrupts();
};

void StepperMotor::setDirection(bool requestedDir){
    _requestedDir = requestedDir;
};