// =============================================================
//  NfcReader - PN532 NFC-Leser ueber I2C
//  Setzt NFC_OUT_PIN fuer ~2s HIGH wenn eine beliebige Karte
//  erkannt wird. Keine Whitelist-Pruefung.
// =============================================================
#ifndef NFC_READER_H
#define NFC_READER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

class NfcReader {
public:
    NfcReader();

    void begin();
    void update();

    static String uidToHex(const uint8_t* uid, uint8_t length);
    String getLetzteUid() const { return letzteUid; }

private:
    Adafruit_PN532* pn532;
    unsigned long   letztesPolling;
    unsigned long   nfcAusgebenBisMs;
    String          letzteUid;
    bool            hwBereit;
};

#endif
