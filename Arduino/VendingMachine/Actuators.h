/**
 * Actuators.h
 * Vending Machine Project – Actuator Pin Definitions & Helper Functions
 *
 * Board: Arduino Uno / Mega (or compatible)
 * Adapt pin numbers to match your physical wiring.
 */

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Pin assignments – actuators
// ---------------------------------------------------------------------------

/** Dispense motor enable – HIGH = motor running */
#define PIN_MOTOR_ENABLE      A1  // Digital output (PWM capable for speed control)

/**
 * Slot-select shift register (74HC595) pins.
 * Up to 8 slots are addressed via a 3-bit binary value on these pins.
 */
#define PIN_SLOT_BIT0         A2  // Digital output – slot address bit 0 (LSB)
#define PIN_SLOT_BIT1         A3  // Digital output – slot address bit 1
#define PIN_SLOT_BIT2         A4  // Digital output – slot address bit 2 (MSB)

/** Change mechanism solenoid – HIGH activates the coin-return solenoid */
#define PIN_CHANGE_SOLENOID   A5  // Digital output

/** Status LED – used for visual feedback and error blinking */
#define PIN_STATUS_LED        13  // Digital output (built-in LED on most boards)

/** Cooling fan relay – HIGH = fan on */
#define PIN_COOLING_FAN       A6  // Digital output

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------

/**
 * Configure all actuator pins as outputs and set them to their safe (OFF) state.
 * Call once from setup().
 */
inline void actuators_init() {
  uint8_t outPins[] = {
    PIN_MOTOR_ENABLE, PIN_SLOT_BIT0, PIN_SLOT_BIT1, PIN_SLOT_BIT2,
    PIN_CHANGE_SOLENOID, PIN_STATUS_LED, PIN_COOLING_FAN
  };

  for (uint8_t i = 0; i < sizeof(outPins) / sizeof(outPins[0]); i++) {
    pinMode(outPins[i], OUTPUT);
    digitalWrite(outPins[i], LOW);
  }
}

// ---------------------------------------------------------------------------
// Control helpers
// ---------------------------------------------------------------------------

/**
 * Enable or disable the dispense motor.
 * @param enable  true = motor on, false = motor off.
 */
inline void actuator_setMotor(bool enable) {
  digitalWrite(PIN_MOTOR_ENABLE, enable ? HIGH : LOW);
}

/**
 * Set the active product slot (0–7).
 * The 3-bit binary address is latched onto the slot-select lines.
 * @param slot  Slot index (0–7).
 */
inline void actuator_setSlot(uint8_t slot) {
  digitalWrite(PIN_SLOT_BIT0, (slot & 0x01) ? HIGH : LOW);
  digitalWrite(PIN_SLOT_BIT1, (slot & 0x02) ? HIGH : LOW);
  digitalWrite(PIN_SLOT_BIT2, (slot & 0x04) ? HIGH : LOW);
}

/**
 * Activate or deactivate the coin-return / change mechanism.
 * @param active  true = release change, false = idle.
 */
inline void actuator_setChange(bool active) {
  digitalWrite(PIN_CHANGE_SOLENOID, active ? HIGH : LOW);
}

/**
 * Set the status LED state.
 * @param on  true = LED on, false = LED off.
 */
inline void actuator_setStatusLED(bool on) {
  digitalWrite(PIN_STATUS_LED, on ? HIGH : LOW);
}

/**
 * Control the cooling fan.
 * @param on  true = fan on, false = fan off.
 */
inline void actuator_setCoolingFan(bool on) {
  digitalWrite(PIN_COOLING_FAN, on ? HIGH : LOW);
}

#endif // ACTUATORS_H
