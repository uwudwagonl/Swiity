// =============================================================
//  FachManager - Implementation
// =============================================================
#include "fach_manager.h"
#include "preferences_manager.h"

static const uint8_t STATUS_PINS[ANZAHL_FAECHER] = {
    STATUS_PIN_1, STATUS_PIN_2, STATUS_PIN_3, STATUS_PIN_4
};

FachManager* FachManager::singleton = nullptr;

FachManager::FachManager()
    : aktuellerSensor(0), letzteMessung(0), prefs(nullptr) {
    for (int i = 0; i < ANZAHL_FAECHER; i++) {
        sensoren[i]          = nullptr;
        letzterLeerStatus[i] = false;
    }
}

void FachManager::begin(PreferencesManager* prefsRef) {
    singleton = this;
    prefs     = prefsRef;

    sensoren[0] = new HCSR04Sensor(1, TRIG_FACH_1, ECHO_FACH_1);
    sensoren[1] = new HCSR04Sensor(2, TRIG_FACH_2, ECHO_FACH_2);
    sensoren[2] = new HCSR04Sensor(3, TRIG_FACH_3, ECHO_FACH_3);
    sensoren[3] = new HCSR04Sensor(4, TRIG_FACH_4, ECHO_FACH_4);

    for (int i = 0; i < ANZAHL_FAECHER; i++) {
        sensoren[i]->begin();

        float ref = prefs ? prefs->ladeKalibrierung(i + 1) : -1.0f;
        if (ref > 0) sensoren[i]->setReferenz(ref);

        letzterLeerStatus[i] = prefs ? prefs->ladeBestandStatus(i + 1) : false;

        // Status-Ausgang initialisieren
        pinMode(STATUS_PINS[i], OUTPUT);
        digitalWrite(STATUS_PINS[i], letzterLeerStatus[i] ? LOW : HIGH);
    }
    DBG("FachManager initialisiert");
}

void FachManager::update() {
    updateSensoren();
}

// Round-Robin: pro Intervall misst nur EIN Sensor.
void FachManager::updateSensoren() {
    if ((millis() - letzteMessung) < SENSOR_INTERVALL_MS) return;
    letzteMessung = millis();

    if (!sensoren[aktuellerSensor]) {
        aktuellerSensor = (aktuellerSensor + 1) % ANZAHL_FAECHER;
        return;
    }

    float mm = sensoren[aktuellerSensor]->messen();
    if (mm > 0) {
        pruefeStatusAenderung(aktuellerSensor + 1);
    }

    aktuellerSensor = (aktuellerSensor + 1) % ANZAHL_FAECHER;
}

void FachManager::pruefeStatusAenderung(int fach) {
    int  idx      = fach - 1;
    bool leerJetzt = sensoren[idx]->istLeer();
    if (leerJetzt != letzterLeerStatus[idx]) {
        letzterLeerStatus[idx] = leerJetzt;
        if (prefs) prefs->speichereBestandStatus(fach, leerJetzt);
        digitalWrite(STATUS_PINS[idx], leerJetzt ? LOW : HIGH);
        DBGF("Fach %d: %s\n", fach, leerJetzt ? "LEER" : "VOLL");
    }
}

void FachManager::kalibriereFach(int fach) {
    if (fach < 1 || fach > ANZAHL_FAECHER) return;
    sensoren[fach - 1]->kalibrieren();
    float ref = sensoren[fach - 1]->getReferenz();
    if (prefs && ref > 0) prefs->speichereKalibrierung(fach, ref);
    letzterLeerStatus[fach - 1] = false;
    if (prefs) prefs->speichereBestandStatus(fach, false);
    digitalWrite(STATUS_PINS[fach - 1], HIGH);
    DBGF("Fach %d kalibriert\n", fach);
}

bool FachManager::istLeer(int fach) {
    if (fach < 1 || fach > ANZAHL_FAECHER) return false;
    return sensoren[fach - 1]->istLeer();
}
