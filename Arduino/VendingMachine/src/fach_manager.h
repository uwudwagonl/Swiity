// =============================================================
//  FachManager - Verwaltet alle 4 Faecher (Sensoren + GPIO-Ausgaenge)
//  Setzt STATUS_PIN_1..4 HIGH (VOLL) oder LOW (LEER) je nach Messung.
// =============================================================
#ifndef FACH_MANAGER_H
#define FACH_MANAGER_H

#include <Arduino.h>
#include "hcsr04_sensor.h"
#include "config.h"

class PreferencesManager;

class FachManager {
public:
    FachManager();

    void begin(PreferencesManager* prefs);
    void update();

    void kalibriereFach(int fach);
    bool istLeer(int fach);

private:
    void updateSensoren();
    void pruefeStatusAenderung(int fach);

    HCSR04Sensor* sensoren[ANZAHL_FAECHER];
    bool          letzterLeerStatus[ANZAHL_FAECHER];

    int           aktuellerSensor;
    unsigned long letzteMessung;

    static FachManager* singleton;
    PreferencesManager* prefs;
};

#endif
