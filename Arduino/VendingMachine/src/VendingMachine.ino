// =============================================================
//  snackomat.ino - ESP32 Snackautomat
//  Projekt: Swiity Gruppe C (Berufsschul-Projekt)
//
//  Kommunikation zur PLC: 5 direkte Digitalleitungen
//   GPIO16-19 -> PLC DI: Fach-Status (HIGH=VOLL, LOW=LEER)
//   GPIO23    -> PLC DI: NFC erkannt (~2s HIGH-Puls bei beliebiger Karte)
//
//  Flow im loop():
//   1. NfcReader  -> Karte erkennen, NFC_OUT_PIN setzen
//   2. FachManager -> Sensoren messen, STATUS_PIN_x setzen
//   3. Watchdog zuruecksetzen
// =============================================================

#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "hcsr04_sensor.h"
#include "preferences_manager.h"
#include "fach_manager.h"
#include "nfc_reader.h"

PreferencesManager prefsManager;
FachManager        fachManager;
NfcReader          nfcReader;

void setup() {
    Serial.begin(DEBUG_BAUDRATE);
    delay(200);

    Serial.println();
    Serial.println("================================================");
    Serial.println("  Snackomat ESP32-Firmware (Swiity Gruppe C)");
    Serial.println("  Build: " __DATE__ " " __TIME__);
    Serial.println("================================================");

    // Watchdog aktivieren -> falls loop() haengt, Reboot
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);
    esp_task_wdt_add(NULL);

    prefsManager.begin();
    fachManager.begin(&prefsManager);
    nfcReader.begin();

    DBG("Setup abgeschlossen - Betrieb beginnt");
}

void loop() {
    nfcReader.update();
    fachManager.update();
    esp_task_wdt_reset();
}
