/**
 * Sensors.h
 * Vending Machine Project – Sensor Pin Definitions & Helper Functions
 *
 * Board: Arduino Uno / Mega (or compatible)
 * Adapt pin numbers to match your physical wiring.
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Pin assignments – sensors
// ---------------------------------------------------------------------------

/** Coin drop detection – HIGH when a coin passes the optical gate */
#define PIN_COIN_SENSOR       2   // Digital input (interrupt-capable)

/** Product dispensed / collected – IR break-beam sensor at output chute */
#define PIN_PRODUCT_SENSOR    3   // Digital input (interrupt-capable)

/** Service door – magnetic reed switch, HIGH = door open */
#define PIN_DOOR_SENSOR       4   // Digital input

/** Temperature sensor data line (DS18B20 one-wire bus) */
#define PIN_TEMP_SENSOR       A0  // Analog input  (or digital for one-wire)

/**
 * Per-slot stock sensors (8 slots).
 * Each is a reflective IR sensor: HIGH = product present.
 */
static const uint8_t PIN_STOCK_SENSOR[8] = { 5, 6, 7, 8, 9, 10, 11, 12 };

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------

/**
 * Configure all sensor pins as inputs with internal pull-ups where appropriate.
 * Call once from setup().
 */
inline void sensors_init() {
  pinMode(PIN_COIN_SENSOR,    INPUT);
  pinMode(PIN_PRODUCT_SENSOR, INPUT);
  pinMode(PIN_DOOR_SENSOR,    INPUT_PULLUP);

  for (uint8_t i = 0; i < 8; i++) {
    pinMode(PIN_STOCK_SENSOR[i], INPUT_PULLUP);
  }
}

// ---------------------------------------------------------------------------
// Read helpers
// ---------------------------------------------------------------------------

/** Returns true when a coin-drop event is detected (active HIGH). */
inline bool sensor_readCoin() {
  return digitalRead(PIN_COIN_SENSOR) == HIGH;
}

/** Returns true when a product has cleared the output chute. */
inline bool sensor_readProduct() {
  return digitalRead(PIN_PRODUCT_SENSOR) == HIGH;
}

/** Returns true when the service door is open. */
inline bool sensor_readDoor() {
  return digitalRead(PIN_DOOR_SENSOR) == LOW; // reed switch pulls LOW when open
}

/**
 * Read temperature from an NTC thermistor on PIN_TEMP_SENSOR.
 * Replace with a proper DS18B20 / DHT22 library call for better accuracy.
 * @return Approximate temperature in degrees Celsius.
 */
inline float sensor_readTemperature() {
  int   raw     = analogRead(PIN_TEMP_SENSOR);
  float voltage = raw * (5.0f / 1023.0f);
  // Simple linear approximation – replace with sensor-specific calculation
  float tempC   = (voltage - 0.5f) * 100.0f;
  return tempC;
}

/**
 * Read stock presence for a given slot index (0–7).
 * @param slot  Slot index.
 * @return      true if product is present in the slot.
 */
inline bool sensor_readStock(uint8_t slot) {
  if (slot >= 8) return false;
  return digitalRead(PIN_STOCK_SENSOR[slot]) == LOW; // active-low sensor
}

#endif // SENSORS_H
