// =============================================================
//  NfcReader - Implementation (PN532 I2C)
//  Jede erkannte Karte (beliebige UID) setzt NFC_OUT_PIN fuer
//  NFC_OUT_DAUER_MS auf HIGH -> PLC wertet das als Kartenbestaetigung.
// =============================================================
#include "nfc_reader.h"
#include "config.h"

NfcReader::NfcReader()
    : pn532(nullptr),
      letztesPolling(0),
      nfcAusgebenBisMs(0),
      hwBereit(false) {}

void NfcReader::begin() {
    Wire.begin(PN532_SDA_PIN, PN532_SCL_PIN);

    pn532 = new Adafruit_PN532(PN532_IRQ_PIN, PN532_RST_PIN, &Wire);
    pn532->begin();

    uint32_t version = pn532->getFirmwareVersion();
    if (!version) {
        DBG("PN532 nicht gefunden! Verdrahtung pruefen.");
        hwBereit = false;
        return;
    }
    DBGF("PN532 gefunden - Firmware 0x%08X\n", version);

    pn532->SAMConfig();
    hwBereit = true;

    pinMode(NFC_OUT_PIN, OUTPUT);
    digitalWrite(NFC_OUT_PIN, LOW);
    DBG("NFC-Leser bereit");
}

void NfcReader::update() {
    if (!hwBereit) return;

    // NFC_OUT_PIN automatisch zuruecksetzen nach NFC_OUT_DAUER_MS
    if (digitalRead(NFC_OUT_PIN) == HIGH && millis() >= nfcAusgebenBisMs) {
        digitalWrite(NFC_OUT_PIN, LOW);
    }

    if ((millis() - letztesPolling) < NFC_POLL_INTERVALL_MS) return;
    letztesPolling = millis();

    uint8_t uidBuf[7];
    uint8_t uidLen = 0;

    bool gefunden = pn532->readPassiveTargetID(
        PN532_MIFARE_ISO14443A, uidBuf, &uidLen, 50);

    if (!gefunden) return;

    letzteUid = uidToHex(uidBuf, uidLen);
    DBGF("NFC: Karte erkannt UID=%s\n", letzteUid.c_str());

    digitalWrite(NFC_OUT_PIN, HIGH);
    nfcAusgebenBisMs = millis() + NFC_OUT_DAUER_MS;
}

String NfcReader::uidToHex(const uint8_t* uid, uint8_t length) {
    String result = "";
    for (int i = 0; i < length; i++) {
        if (uid[i] < 0x10) result += "0";
        result += String(uid[i], HEX);
    }
    result.toUpperCase();
    return result;
}
