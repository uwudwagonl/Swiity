// =============================================================
//  PreferencesManager - Implementation
// =============================================================
#include "preferences_manager.h"
#include "config.h"

PreferencesManager::PreferencesManager() : geoeffnet(false) {}

void PreferencesManager::begin() {
    // NVS im Read/Write-Modus oeffnen
    geoeffnet = prefs.begin(NVS_NAMESPACE, false);
    if (geoeffnet) {
        DBG("NVS geoeffnet (Namespace: " NVS_NAMESPACE ")");
    } else {
        DBG("NVS FEHLER - Werte werden nicht persistiert!");
    }
}

float PreferencesManager::ladeKalibrierung(int fach) {
    if (!geoeffnet) return -1.0f;
    String key = "ref" + String(fach);
    float wert = prefs.getFloat(key.c_str(), -1.0f);
    DBGF("NVS lese %s = %.1f\n", key.c_str(), wert);
    return wert;
}

void PreferencesManager::speichereKalibrierung(int fach, float wertMm) {
    if (!geoeffnet) return;
    String key = "ref" + String(fach);
    prefs.putFloat(key.c_str(), wertMm);
    DBGF("NVS schreibe %s = %.1f mm\n", key.c_str(), wertMm);
}

bool PreferencesManager::ladeBestandStatus(int fach) {
    if (!geoeffnet) return false;
    String key = "leer" + String(fach);
    return prefs.getBool(key.c_str(), false);
}

void PreferencesManager::speichereBestandStatus(int fach, bool leer) {
    if (!geoeffnet) return;
    String key = "leer" + String(fach);
    prefs.putBool(key.c_str(), leer);
}

unsigned long PreferencesManager::ladeMotorLaufzeit(int fach) {
    if (!geoeffnet) return MOTOR_LAUFZEIT_DEFAULT_MS;
    String key = "mt" + String(fach);
    return prefs.getULong(key.c_str(), MOTOR_LAUFZEIT_DEFAULT_MS);
}

void PreferencesManager::speichereMotorLaufzeit(int fach, unsigned long ms) {
    if (!geoeffnet) return;
    String key = "mt" + String(fach);
    prefs.putULong(key.c_str(), ms);
    DBGF("NVS schreibe %s = %lu ms\n", key.c_str(), ms);
}

void PreferencesManager::loescheAlles() {
    if (!geoeffnet) return;
    prefs.clear();
    DBG("NVS gelöscht");
}
