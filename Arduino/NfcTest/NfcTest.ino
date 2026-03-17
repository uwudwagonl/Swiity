/**
 * NfcTest.ino
 * Hardware test sketch for Elechouse NFC Module V3 (PN532) over I2C.
 *
 * Wiring (Arduino Nano):
 *   SDA → A4
 *   SCL → A5
 *   VCC → 5 V
 *   GND → GND
 *
 * Board: Arduino Nano
 */

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

PN532_I2C pn532i2c(Wire);
PN532     nfc(pn532i2c);

void setup() {
  Serial.begin(115200);
  Serial.println("# NfcTest starting");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("ERROR: PN532 not found – check wiring");
    while (1);
  }

  Serial.print("PN5"); Serial.print((versiondata >> 24) & 0xFF, HEX);
  Serial.print(" firmware v");
  Serial.print((versiondata >> 16) & 0xFF, DEC); Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
  Serial.println("Waiting for NFC tag...");
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLen;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen)) {
    Serial.print("UID:");
    for (uint8_t i = 0; i < uidLen; i++) {
      Serial.print(' ');
      if (uid[i] < 0x10) Serial.print('0');
      Serial.print(uid[i], HEX);
    }
    Serial.println();
    delay(1000);
  }
}
