// =============================================================
//  Snackomat - Zentrale Konfigurationsdatei
//  Hardware: ESP32 + 4x HC-SR04 + PN532 NFC
//  Kommunikation: 5 digitale Leitungen zur B&R PLC
//   GPIO16-19: Fach-Status (HIGH=VOLL, LOW=LEER)
//   GPIO23:    NFC erkannt (~2s HIGH-Puls)
// =============================================================
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Debug-Ausgabe ---
#define DEBUG_MODUS     1
#define DEBUG_BAUDRATE  115200

// --- Anzahl der Faecher ---
#define ANZAHL_FAECHER  4

// =============================================================
//  PIN-BELEGUNG
// =============================================================

// HC-SR04 Ultraschall-Sensoren (Trigger = Output, Echo = Input)
#define TRIG_FACH_1  26
#define ECHO_FACH_1  27
#define TRIG_FACH_2  14
#define ECHO_FACH_2  12
#define TRIG_FACH_3  33
#define ECHO_FACH_3  32
#define TRIG_FACH_4  25
#define ECHO_FACH_4  35   // GPIO35 ist nur Input -> fuer Echo OK

// Fach-Status Ausgaenge -> PLC Digitaleingaenge
// HIGH = VOLL (Produkt vorhanden), LOW = LEER (Produkt fehlt)
#define STATUS_PIN_1  16
#define STATUS_PIN_2  17
#define STATUS_PIN_3  18
#define STATUS_PIN_4  19

// PN532 NFC-Leser (I2C)
#define PN532_SDA_PIN    21
#define PN532_SCL_PIN    22
#define PN532_IRQ_PIN    13
#define PN532_RST_PIN    15

// NFC-Erkennungs-Ausgang -> PLC Digitaleingang
// HIGH (~2s) wenn eine beliebige Karte erkannt wurde
#define NFC_OUT_PIN  23

// =============================================================
//  ZEIT- UND SCHWELLWERT-KONSTANTEN
// =============================================================
#define LEER_SCHWELLE_MM      50      // Abstand > Ref+50mm -> LEER
#define HCSR04_TIMEOUT_US     30000   // Max. 30ms auf Echo warten
#define SENSOR_INTERVALL_MS   250     // Ein Sensor alle 250ms (Round-Robin)
#define NFC_POLL_INTERVALL_MS 500     // NFC-Polling alle 500ms
#define NFC_OUT_DAUER_MS      2000    // NFC_OUT_PIN bleibt 2s HIGH

// Schallgeschwindigkeit in mm/us bei 20 °C
#define SCHALL_MM_PRO_US      0.343f

// Watchdog-Timeout (Sekunden)
#define WATCHDOG_TIMEOUT_S    10

// =============================================================
//  NVS (Preferences) Namespace
// =============================================================
#define NVS_NAMESPACE  "snackomat"

// =============================================================
//  DEBUG-MAKROS (mit Zeitstempel)
// =============================================================
#if DEBUG_MODUS
  #define DBG(x)    do { Serial.print("["); Serial.print(millis()); Serial.print("ms] "); Serial.println(x); } while(0)
  #define DBGF(...) do { Serial.print("["); Serial.print(millis()); Serial.print("ms] "); Serial.printf(__VA_ARGS__); } while(0)
#else
  #define DBG(x)
  #define DBGF(...)
#endif

#endif // CONFIG_H
