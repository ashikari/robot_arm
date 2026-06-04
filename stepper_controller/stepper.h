#pragma once

#include <cstdint>
#include <Arduino.h>

constexpr int DIR_PIN = 23;
constexpr int STEP_PIN = 22;
constexpr int M1_PIN = 21; 
constexpr int M2_PIN = 20; 
constexpr int M3_PIN  = 19; 

constexpr float STEP_ANGLE = 1.8; // degrees
constexpr int STEPS_PER_ROTATION = 360.0 / STEP_ANGLE;

class StepperMotor{
    public:
        StepperMotor(int dirPin, int stepPin, int m1Pin, int m2Pin, int m3Pin);
        void setup();
        void update();
        void setVelocity(float velocity);
        void setDirection(bool clockwise);

    private:
        int _dirPin;
        int _stepPin;
        int _m1Pin;
        int _m2Pin;
        int _m3Pin;

        bool _stepHigh;
        uint32_t _lastStepTime;
        uint32_t _stepInterval;
        float _velocity; // RPM

        bool _appliedDir;
        bool _requestedDir;

        static constexpr uint32_t STEP_PULSE_WIDTH_US = 2;
};

