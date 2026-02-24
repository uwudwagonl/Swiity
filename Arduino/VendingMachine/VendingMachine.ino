/**
 * VendingMachine.ino
 * Vending Machine Project – Arduino Main Sketch
 *
 * Responsibilities:
 *   - Read all sensors every loop cycle
 *   - Forward sensor data to the PLC via Serial (CSV format)
 *   - Receive actuator commands from the PLC via Serial
 *   - Drive actuator outputs accordingly
 *
 * Serial protocol (115 200 baud):
 *   Arduino → PLC  (sensor report, sent every REPORT_INTERVAL_MS):
 *     "S,<coin>,<product>,<door>,<temp>,<stock0>,...,<stock7>\n"
 *
 *   PLC → Arduino  (actuator command):
 *     "A,<motorEn>,<slot>,<change>,<statusLED>,<fan>\n"
 *
 * Board:  Arduino Uno / Mega (or compatible)
 */

#include "Sensors.h"
#include "Actuators.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

/** How often the Arduino sends a sensor report to the PLC (milliseconds). */
static const unsigned long REPORT_INTERVAL_MS = 100UL;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static unsigned long lastReportTime = 0;

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

void sendSensorReport();
void handleSerialCommand(const String &line);

// ---------------------------------------------------------------------------
// setup()
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  sensors_init();
  actuators_init();
  Serial.println("# VendingMachine Arduino ready");
}

// ---------------------------------------------------------------------------
// loop()
// ---------------------------------------------------------------------------

void loop() {
  unsigned long now = millis();

  // Send periodic sensor report to PLC
  if (now - lastReportTime >= REPORT_INTERVAL_MS) {
    lastReportTime = now;
    sendSensorReport();
  }

  // Process incoming actuator commands from PLC
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      handleSerialCommand(line);
    }
  }
}

// ---------------------------------------------------------------------------
// sendSensorReport()
// ---------------------------------------------------------------------------

/**
 * Build and transmit the sensor CSV frame:
 * "S,<coin>,<product>,<door>,<temp>,<s0>,<s1>,<s2>,<s3>,<s4>,<s5>,<s6>,<s7>"
 */
void sendSensorReport() {
  Serial.print("S,");
  Serial.print(sensor_readCoin()    ? 1 : 0); Serial.print(',');
  Serial.print(sensor_readProduct() ? 1 : 0); Serial.print(',');
  Serial.print(sensor_readDoor()    ? 1 : 0); Serial.print(',');
  Serial.print(sensor_readTemperature(), 1);

  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(',');
    Serial.print(sensor_readStock(i) ? 1 : 0);
  }
  Serial.println();
}

// ---------------------------------------------------------------------------
// handleSerialCommand()
// ---------------------------------------------------------------------------

/**
 * Parse an actuator command line received from the PLC.
 * Expected format: "A,<motorEn>,<slot>,<change>,<led>,<fan>"
 *
 * Example: "A,1,3,0,1,0"  → motor ON, slot 3 selected, change OFF, LED ON, fan OFF
 */
void handleSerialCommand(const String &line) {
  if (line.charAt(0) != 'A') return;  // Only handle actuator frames

  // Tokenise by comma
  String tokens[6];
  uint8_t tokenIdx = 0;
  int     start    = 2; // skip "A,"

  for (int i = start; i <= (int)line.length() && tokenIdx < 6; i++) {
    if (i == (int)line.length() || line.charAt(i) == ',') {
      tokens[tokenIdx++] = line.substring(start, i);
      start = i + 1;
    }
  }

  if (tokenIdx < 6) return; // Incomplete frame – ignore

  bool    motorEn   = tokens[0].toInt() != 0;
  uint8_t slot      = (uint8_t)tokens[1].toInt();
  bool    changeAct = tokens[2].toInt() != 0;
  bool    ledOn     = tokens[3].toInt() != 0;
  bool    fanOn     = tokens[4].toInt() != 0;

  actuator_setMotor(motorEn);
  actuator_setSlot(slot);
  actuator_setChange(changeAct);
  actuator_setStatusLED(ledOn);
  actuator_setCoolingFan(fanOn);
}
