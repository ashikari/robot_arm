#pragma once

#include <cstdint>
#include <Arduino.h>

constexpr int DIR_PIN = 23;
constexpr int STEP_PIN = 22;


constexpr float STEP_ANGLE = 1.8; // degrees
constexpr int STEPS_PER_ROTATION = 360.0 / STEP_ANGLE;

class StepperMotor{
    public:
        StepperMotor(int dirPin, int stepPin);
        void setup();
        void update();
        void setVelocity(float velocity);
        void setDirection(bool clockwise);

    private:
        int _dirPin;
        int _stepPin;
        bool _stepState;
        uint32_t _lastStepTime;
        uint32_t _stepInterval;
        float _velocity; // RPM
        bool _clockwise;
};

