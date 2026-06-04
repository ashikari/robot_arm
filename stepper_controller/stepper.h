#pragma once

#include <cstdint>
#include <Arduino.h>

// Default pin assignments for the Adafruit A4988 stepper driver breakout.
// The A4988 defaults to 1/16 microstep mode; MS1/MS2/MS3 select the resolution.
// Hardware: https://www.adafruit.com/product/6109
constexpr int DIR_PIN = 23;
constexpr int STEP_PIN = 22;
constexpr int M1_PIN = 21;  // Microstep select bit 0 (MS1)
constexpr int M2_PIN = 20;  // Microstep select bit 1 (MS2)
constexpr int M3_PIN  = 19; // Microstep select bit 2 (MS3)

constexpr float STEP_ANGLE = 1.8;                       // degrees per full step
constexpr int STEPS_PER_ROTATION = 360.0 / STEP_ANGLE; // full steps per revolution (200)

/** Microstep resolution modes supported by the A4988 MS1/MS2/MS3 pin configuration. */
enum class MicrostepMode { FULL, HALF, QUARTER, EIGHTH, SIXTEENTH };

/** Pin states and effective steps/revolution for each MicrostepMode.
 *  Indexed by static_cast<int>(MicrostepMode). */
struct MicrostepConfig {
    bool ms1, ms2, ms3;
    int stepsPerRotation;
};

constexpr MicrostepConfig MICROSTEP_TABLE[] = {
    {LOW,  LOW,  LOW,   200},  // FULL      — 2-phase
    {HIGH, LOW,  LOW,   400},  // HALF      — 1-2 phase
    {LOW,  HIGH, LOW,   800},  // QUARTER   — W1-2 phase
    {HIGH, HIGH, LOW,  1600},  // EIGHTH    — 2W1-2 phase
    {HIGH, HIGH, HIGH, 3200},  // SIXTEENTH — 4W1-2 phase
};

/**
 * Non-blocking stepper motor driver for the Adafruit A4988 breakout.
 *
 * Call update() from a fast loop or ISR; it manages STEP pulse timing
 * internally and never blocks. Direction changes are deferred until the
 * current pulse is complete to satisfy A4988 DIR setup/hold-time requirements.
 */
class StepperMotor{
    public:
        /**
         * @param dirPin   GPIO pin connected to driver DIR input
         * @param stepPin  GPIO pin connected to driver STEP input
         * @param ms1Pin   GPIO pin connected to A4988 MS1 (microstep select bit 0)
         * @param ms2Pin   GPIO pin connected to A4988 MS2 (microstep select bit 1)
         * @param ms3Pin   GPIO pin connected to A4988 MS3 (microstep select bit 2)
         */
        StepperMotor(int dirPin, int stepPin, int ms1Pin, int ms2Pin, int ms3Pin);

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

        /** Set the microstep resolution by writing the MS1/MS2/MS3 pins and
         *  updating the internal steps-per-rotation used by setVelocity().
         *  Safe to call at runtime while the motor is running; _microstepMode
         *  and _stepsPerRotation are updated atomically under noInterrupts().
         *  Call setVelocity() again afterward to recalculate the step interval
         *  at the new resolution.
         *  @param mode  Desired MicrostepMode; defaults to FULL on setup(). */
        void setMicrostepMode(MicrostepMode mode);

    private:
        int _dirPin;
        int _stepPin;
        int _ms1Pin;
        int _ms2Pin;
        int _ms3Pin;

        bool     _stepHigh;               // true while STEP pin is being held HIGH
        uint32_t _lastStepTime;           // micros() timestamp of the most recent leading edge (µs)
        volatile uint32_t _stepInterval;  // time between successive leading edges (µs)
        volatile float    _velocity;      // current speed (rev/s)

        bool             _appliedDir;    // direction currently driven on the DIR pin
        volatile bool    _requestedDir;  // direction requested via setDirection()

        MicrostepMode     _microstepMode;        // current microstep resolution
        volatile int      _stepsPerRotation;     // effective steps/rev for the active microstep mode

        // Minimum STEP pulse width required by the A4988 is 1 µs; 2 µs provides ample margin
        static constexpr uint32_t STEP_PULSE_WIDTH_US = 2;
};

