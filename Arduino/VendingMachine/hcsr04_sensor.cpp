// =============================================================
//  HCSR04Sensor - Implementation
// =============================================================
#include "hcsr04_sensor.h"
#include "config.h"

HCSR04Sensor::HCSR04Sensor(int fachNr, int trigPin, int echoPin)
    : fachNr(fachNr),
      trigPin(trigPin),
      echoPin(echoPin),
      letzterAbstand(-1.0f),
      referenzAbstand(-1.0f) {
}

void HCSR04Sensor::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
    DBGF("Sensor Fach %d initialisiert (TRIG=%d, ECHO=%d)\n",
         fachNr, trigPin, echoPin);
}

// Eine einzelne Messung per pulseIn()
// Ablauf:
//   1. Trigger-Pin kurz (10 µs) auf HIGH
//   2. Echo-Pin misst die Dauer des HIGH-Impulses
//   3. Distanz = Dauer * Schallgeschwindigkeit / 2 (Hin + Rück)
float HCSR04Sensor::messeEinzeln() {
    // Trigger-Impuls (10 µs HIGH)
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Echo-Dauer messen, max. HCSR04_TIMEOUT_US warten
    unsigned long dauerUs = pulseIn(echoPin, HIGH, HCSR04_TIMEOUT_US);

    if (dauerUs == 0) {
        // Timeout -> nichts in Reichweite oder Fehler
        return -1.0f;
    }

    // Umrechnung in Millimeter
    float distanzMm = (dauerUs * SCHALL_MM_PRO_US) / 2.0f;
    return distanzMm;
}

// Hilfsfunktion: Median aus 3 Werten
static float median3(float a, float b, float c) {
    if ((a >= b && a <= c) || (a <= b && a >= c)) return a;
    if ((b >= a && b <= c) || (b <= a && b >= c)) return b;
    return c;
}

// Median aus 3 Messungen -> robust gegen Ausreißer
float HCSR04Sensor::messen() {
    float m1 = messeEinzeln();
    delay(15);  // Kurze Pause, damit sich Echo-Signale nicht überlagern
    float m2 = messeEinzeln();
    delay(15);
    float m3 = messeEinzeln();

    // Fehlerwerte (-1) durch Nachbarn ersetzen (best effort)
    if (m1 < 0 && m2 >= 0) m1 = m2;
    if (m3 < 0 && m2 >= 0) m3 = m2;
    if (m2 < 0 && m1 >= 0) m2 = m1;

    // Wenn alle Messungen fehlgeschlagen sind
    if (m1 < 0 && m2 < 0 && m3 < 0) {
        letzterAbstand = -1.0f;
        return -1.0f;
    }

    float ergebnis = median3(m1, m2, m3);
    letzterAbstand = ergebnis;
    return ergebnis;
}

void HCSR04Sensor::kalibrieren() {
    float gemessen = messen();
    if (gemessen > 0) {
        referenzAbstand = gemessen;
        DBGF("Sensor Fach %d kalibriert: %.1f mm\n", fachNr, referenzAbstand);
    } else {
        DBGF("Sensor Fach %d: Kalibrierung fehlgeschlagen (Timeout)\n", fachNr);
    }
}

bool HCSR04Sensor::istLeer() {
    // Ohne Kalibrierung kann kein LEER-Status bestimmt werden
    if (referenzAbstand < 0 || letzterAbstand < 0) {
        return false;
    }
    // LEER = aktueller Abstand deutlich größer als Referenz
    return (letzterAbstand > (referenzAbstand + LEER_SCHWELLE_MM));
}
