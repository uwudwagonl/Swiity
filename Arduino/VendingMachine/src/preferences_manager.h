// =============================================================
//  PreferencesManager - Persistenter Speicher im ESP32-NVS
//  Speichert: Kalibrier-Referenzwerte und LEER/VOLL-Status pro Fach.
// =============================================================
#ifndef PREFERENCES_MANAGER_H
#define PREFERENCES_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class PreferencesManager {
public:
    PreferencesManager();

    // NVS oeffnen (im setup() aufrufen)
    void begin();

    // Kalibrier-Referenzwerte (mm) laden/speichern
    float ladeKalibrierung(int fach);                 // liefert -1.0f wenn nicht gesetzt
    void  speichereKalibrierung(int fach, float wertMm);

    // LEER/VOLL-Status laden/speichern (fuer Restore nach Reboot)
    bool  ladeBestandStatus(int fach);                // default false = VOLL
    void  speichereBestandStatus(int fach, bool leer);

    // Alle Werte verwerfen (Factory-Reset)
    void loescheAlles();

private:
    Preferences prefs;
    bool geoeffnet;
};

#endif
