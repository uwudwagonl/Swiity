// =============================================================
//  HCSR04Sensor - Klasse für einen HC-SR04 Ultraschall-Sensor
//  Misst den Abstand zur Oberseite des Produktstapels.
// =============================================================
#ifndef HCSR04_SENSOR_H
#define HCSR04_SENSOR_H

#include <Arduino.h>

class HCSR04Sensor {
public:
    // Konstruktor: Fach-Nr (1..4), Trigger-Pin, Echo-Pin
    HCSR04Sensor(int fachNr, int trigPin, int echoPin);

    // Initialisiert die Pins (im setup() aufrufen)
    void begin();

    // Führt eine einzelne Messung aus (Abstand in mm).
    // Rückgabe -1.0f = Timeout / Fehler
    float messeEinzeln();

    // Führt 3 Messungen durch und liefert den Median in mm
    float messen();

    // Speichert den aktuellen Abstand als "VOLL"-Referenz
    void kalibrieren();

    // true, wenn aktueller Abstand > Referenz + LEER_SCHWELLE_MM
    bool istLeer();

    // Letzter gemessener Abstand in mm
    float getAbstand() const { return letzterAbstand; }

    // Referenzwert (kalibriert) in mm
    float getReferenz() const { return referenzAbstand; }

    // Referenzwert extern setzen (z.B. aus NVS laden)
    void setReferenz(float ref) { referenzAbstand = ref; }

    // Fach-Nummer (1..4)
    int getFachNr() const { return fachNr; }

private:
    int fachNr;
    int trigPin;
    int echoPin;
    float letzterAbstand;    // in mm
    float referenzAbstand;   // in mm (Kalibrierwert)
};

#endif // HCSR04_SENSOR_H
