#pragma once

#include <cstdint>
#include <Arduino.h>

// Default pin assignments for the stepper driver (A4988 / DRV8825 compatible)
constexpr int DIR_PIN = 23;
constexpr int STEP_PIN = 22;
constexpr int M1_PIN = 21;  // Microstep select bit 0
constexpr int M2_PIN = 20;  // Microstep select bit 1
constexpr int M3_PIN  = 19; // Microstep select bit 2

constexpr float STEP_ANGLE = 1.8;                       // degrees per full step
constexpr int STEPS_PER_ROTATION = 360.0 / STEP_ANGLE; // full steps per revolution (200)

/**
 * Non-blocking stepper motor driver.
 *
 * Call update() from a fast loop or ISR; it manages STEP pulse timing
 * internally and never blocks. Direction changes are deferred until the
 * current pulse is complete to satisfy driver setup/hold-time requirements.
 */
class StepperMotor{
    public:
        /**
         * @param dirPin   GPIO pin connected to driver DIR input
         * @param stepPin  GPIO pin connected to driver STEP input
         * @param m1Pin    GPIO pin connected to driver MS1 (microstep select bit 0)
         * @param m2Pin    GPIO pin connected to driver MS2 (microstep select bit 1)
         * @param m3Pin    GPIO pin connected to driver MS3 (microstep select bit 2)
         */
        StepperMotor(int dirPin, int stepPin, int m1Pin, int m2Pin, int m3Pin);

        /** Configure all pins as outputs and drive them to their default states.
         *  Must be called once from Arduino setup() before the first update(). */
        void setup();

        /** Advance the step-pulse state machine.
         *  Must be called as frequently as possible (loop() or a timer ISR) to
         *  maintain accurate timing.  Never blocks. */
        void update();

        /** Set the motor speed.
         *  @param velocity  Desired speed in rotations per second (rev/s).
         *                   Pass 0.0 to stop.  Negative values are not supported;
         *                   use setDirection() to control sense of rotation. */
        void setVelocity(float velocity);

        /** Request a change in rotation direction.
         *  The change is applied after the current STEP pulse completes so that
         *  driver DIR setup-time requirements are always satisfied.
         *  @param clockwise  true = clockwise, false = counter-clockwise
         *                    (as viewed from the motor shaft end). */
        void setDirection(bool clockwise);

    private:
        int _dirPin;
        int _stepPin;
        int _m1Pin;
        int _m2Pin;
        int _m3Pin;

        bool     _stepHigh;               // true while STEP pin is being held HIGH
        uint32_t _lastStepTime;           // micros() timestamp of the most recent leading edge (µs)
        volatile uint32_t _stepInterval;  // time between successive leading edges (µs)
        volatile float    _velocity;      // current speed (rev/s)

        bool             _appliedDir;    // direction currently driven on the DIR pin
        volatile bool    _requestedDir;  // direction requested via setDirection()

        // Minimum STEP pulse width required by the stepper driver (µs)
        static constexpr uint32_t STEP_PULSE_WIDTH_US = 2;
};

