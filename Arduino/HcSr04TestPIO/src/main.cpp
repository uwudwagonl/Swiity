#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LittleFS.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "dashboard.h"

// AP credentials
const char* ssid = "Swiity ESP32";
const char* password = "12345678";

// --- HC-SR04 config ---
#define NUM_SLOTS 4
#define SOUND_SPEED 0.034

const int pins[NUM_SLOTS][2] = {
  {5, 18},   // Slot 1
  {16, 17},  // Slot 2
  {32, 33},  // Slot 3
  {25, 26},  // Slot 4
};

float distance[NUM_SLOTS];
float threshold[NUM_SLOTS] = {15.0, 15.0, 15.0, 15.0};

// --- NFC config ---
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

String lastUid = "";
unsigned long lastNfcScan = 0;
bool nfcReady = false;

// --- Analytics DB ---
#define MAX_RECORDS 168
#define MAX_UIDS_PER_HOUR 32
#define DB_PATH "/analytics.bin"

struct HourlyRecord {
  uint32_t hour;
  uint16_t nfcScans;
  uint16_t uniqueUids;
  uint8_t slotOccupancy[NUM_SLOTS];
};

HourlyRecord records[MAX_RECORDS];
int recordCount = 0;

// --- Transaction log ---
#define MAX_TRANSACTIONS 200
#define TX_PATH "/transactions.bin"

struct Transaction {
  uint32_t timestamp;   // seconds since boot
  uint8_t uid[7];
  uint8_t uidLen;
};

Transaction transactions[MAX_TRANSACTIONS];
int txCount = 0;

// Current hour tracking
uint32_t currentHour = 0;
uint16_t curNfcScans = 0;
String curUids[MAX_UIDS_PER_HOUR];
uint16_t curUidCount = 0;
uint32_t curSlotPresentSamples[NUM_SLOTS] = {0};
uint32_t curTotalSamples = 0;
unsigned long lastHourCheck = 0;
unsigned long lastSampleTime = 0;
unsigned long bootTime = 0;

// Debounce: ignore same card within 2 seconds
unsigned long lastTxTime = 0;
String lastTxUid = "";

// --- Functions ---

float readSensor(int slot) {
  int trig = pins[slot][0];
  int echo = pins[slot][1];
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * SOUND_SPEED / 2;
}

// --- Transaction persistence ---

void saveTx() {
  File f = LittleFS.open(TX_PATH, "w");
  if (!f) return;
  f.write((uint8_t*)&txCount, sizeof(txCount));
  f.write((uint8_t*)transactions, sizeof(Transaction) * txCount);
  f.close();
}

void loadTx() {
  if (!LittleFS.exists(TX_PATH)) {
    txCount = 0;
    saveTx();
    return;
  }
  File f = LittleFS.open(TX_PATH, "r");
  if (!f) { txCount = 0; return; }
  f.read((uint8_t*)&txCount, sizeof(txCount));
  if (txCount < 0 || txCount > MAX_TRANSACTIONS) { txCount = 0; f.close(); return; }
  f.read((uint8_t*)transactions, sizeof(Transaction) * txCount);
  f.close();
}

void addTransaction(uint8_t* uid, uint8_t uidLen) {
  if (txCount >= MAX_TRANSACTIONS) {
    memmove(&transactions[0], &transactions[1], sizeof(Transaction) * (MAX_TRANSACTIONS - 1));
    txCount = MAX_TRANSACTIONS - 1;
  }
  Transaction& tx = transactions[txCount++];
  tx.timestamp = (millis() - bootTime) / 1000;
  tx.uidLen = uidLen > 7 ? 7 : uidLen;
  memcpy(tx.uid, uid, tx.uidLen);
  saveTx();
}

String uidToString(uint8_t* uid, uint8_t len) {
  String s = "";
  for (uint8_t i = 0; i < len; i++) {
    if (i > 0) s += ":";
    if (uid[i] < 0x10) s += "0";
    s += String(uid[i], HEX);
  }
  s.toUpperCase();
  return s;
}

// --- NFC ---

void checkNfc() {
  uint8_t uid[7];
  uint8_t uidLen;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50)) {
    lastUid = uidToString(uid, uidLen);
    lastNfcScan = millis();
    Serial.println("NFC UID: " + lastUid);

    // Debounce: max 1 scan per 2.5s
    unsigned long now = millis();
    if ((now - lastTxTime) > 2500) {
      lastTxUid = lastUid;
      lastTxTime = now;

      // Log transaction
      addTransaction(uid, uidLen);

      // Track for hourly analytics
      curNfcScans++;
      bool found = false;
      for (uint16_t i = 0; i < curUidCount; i++) {
        if (curUids[i] == lastUid) { found = true; break; }
      }
      if (!found && curUidCount < MAX_UIDS_PER_HOUR) {
        curUids[curUidCount++] = lastUid;
      }
    }
  }
}

// --- Hourly DB persistence ---

void saveDb() {
  File f = LittleFS.open(DB_PATH, "w");
  if (!f) return;
  f.write((uint8_t*)&recordCount, sizeof(recordCount));
  f.write((uint8_t*)records, sizeof(HourlyRecord) * recordCount);
  f.close();
}

void loadDb() {
  if (!LittleFS.exists(DB_PATH)) {
    recordCount = 0;
    saveDb();
    return;
  }
  File f = LittleFS.open(DB_PATH, "r");
  if (!f) { recordCount = 0; return; }
  f.read((uint8_t*)&recordCount, sizeof(recordCount));
  if (recordCount < 0 || recordCount > MAX_RECORDS) { recordCount = 0; f.close(); return; }
  f.read((uint8_t*)records, sizeof(HourlyRecord) * recordCount);
  f.close();
  if (recordCount > 0) {
    currentHour = records[recordCount - 1].hour + 1;
  }
}

void commitHour() {
  HourlyRecord rec;
  rec.hour = currentHour;
  rec.nfcScans = curNfcScans;
  rec.uniqueUids = curUidCount;
  for (int i = 0; i < NUM_SLOTS; i++) {
    rec.slotOccupancy[i] = curTotalSamples > 0
      ? (uint8_t)((curSlotPresentSamples[i] * 100) / curTotalSamples) : 0;
  }
  if (recordCount >= MAX_RECORDS) {
    memmove(&records[0], &records[1], sizeof(HourlyRecord) * (MAX_RECORDS - 1));
    recordCount = MAX_RECORDS - 1;
  }
  records[recordCount++] = rec;
  saveDb();

  currentHour++;
  curNfcScans = 0;
  curUidCount = 0;
  for (int i = 0; i < NUM_SLOTS; i++) curSlotPresentSamples[i] = 0;
  curTotalSamples = 0;
  Serial.println("Hour " + String(rec.hour) + " committed (" + String(recordCount) + " records)");
}

void sampleSlots() {
  curTotalSamples++;
  for (int i = 0; i < NUM_SLOTS; i++) {
    if (distance[i] >= 0 && distance[i] < threshold[i]) {
      curSlotPresentSamples[i]++;
    }
  }
}

WebServer server(80);

void setup() {
  Serial.begin(115200);
  bootTime = millis();

  if (!LittleFS.begin(true)) Serial.println("LittleFS mount failed");

  for (int i = 0; i < NUM_SLOTS; i++) {
    pinMode(pins[i][0], OUTPUT);
    pinMode(pins[i][1], INPUT);
  }

  // Init NFC
  Wire.begin(21, 22);
  Wire.setClock(100000);
  Wire.beginTransmission(0x24);
  if (Wire.endTransmission() == 0) {
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (versiondata) {
      nfcReady = true;
      nfc.SAMConfig();
      Serial.print("PN5");
      Serial.print((versiondata >> 24) & 0xFF, HEX);
      Serial.print(" fw v");
      Serial.print((versiondata >> 16) & 0xFF);
      Serial.print(".");
      Serial.println((versiondata >> 8) & 0xFF);
    }
  }
  if (!nfcReady) Serial.println("NFC not found - continuing without it");

  loadDb();
  loadTx();
  Serial.println("Loaded " + String(recordCount) + " hourly records, " + String(txCount) + " transactions");

  WiFi.softAP(ssid, password);
  Serial.println("AP started: " + String(ssid));
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // --- Routes ---

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", dashboard_html);
  });

  server.on("/status", HTTP_GET, []() {
    String json = "{\"slots\":[";
    for (int i = 0; i < NUM_SLOTS; i++) {
      if (i > 0) json += ",";
      json += "{\"slot\":" + String(i + 1);
      json += ",\"distance\":" + String(distance[i], 1);
      json += ",\"threshold\":" + String(threshold[i], 1);
      json += ",\"present\":" + String(distance[i] >= 0 && distance[i] < threshold[i] ? "true" : "false");
      json += "}";
    }
    json += "],\"nfc\":{\"ready\":" + String(nfcReady ? "true" : "false");
    json += ",\"uid\":\"" + lastUid + "\"";
    json += ",\"age\":" + String(lastUid.length() > 0 ? (millis() - lastNfcScan) / 1000 : -1);
    json += "},\"uptime\":" + String((millis() - bootTime) / 1000);
    json += ",\"currentHour\":" + String(currentHour);
    json += ",\"dbRecords\":" + String(recordCount);
    json += ",\"txCount\":" + String(txCount);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/analytics", HTTP_GET, []() {
    int from = 0;
    int to = recordCount;
    if (server.hasArg("last")) {
      int hours = server.arg("last").toInt();
      if (hours > 0 && hours < recordCount) from = recordCount - hours;
    }
    if (server.hasArg("from")) from = max(0, (int)server.arg("from").toInt());
    if (server.hasArg("to")) to = min(recordCount, (int)server.arg("to").toInt());

    String json = "{\"records\":[";
    for (int i = from; i < to; i++) {
      if (i > from) json += ",";
      json += "{\"h\":" + String(records[i].hour);
      json += ",\"nfc\":" + String(records[i].nfcScans);
      json += ",\"uids\":" + String(records[i].uniqueUids);
      json += ",\"occ\":[";
      for (int s = 0; s < NUM_SLOTS; s++) {
        if (s > 0) json += ",";
        json += String(records[i].slotOccupancy[s]);
      }
      json += "]}";
    }
    json += "],\"current\":{\"h\":" + String(currentHour);
    json += ",\"nfc\":" + String(curNfcScans);
    json += ",\"uids\":" + String(curUidCount);
    json += ",\"occ\":[";
    for (int s = 0; s < NUM_SLOTS; s++) {
      if (s > 0) json += ",";
      json += String(curTotalSamples > 0
        ? (curSlotPresentSamples[s] * 100) / curTotalSamples : 0);
    }
    json += "],\"samples\":" + String(curTotalSamples);
    json += "},\"total\":" + String(recordCount);
    json += ",\"uptime\":" + String((millis() - bootTime) / 1000);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/transactions", HTTP_GET, []() {
    int limit = txCount;
    int offset = 0;
    if (server.hasArg("last")) {
      int n = server.arg("last").toInt();
      if (n > 0 && n < txCount) offset = txCount - n;
    }
    String json = "{\"transactions\":[";
    bool first = true;
    for (int i = offset; i < txCount; i++) {
      if (!first) json += ",";
      first = false;
      json += "{\"t\":" + String(transactions[i].timestamp);
      json += ",\"uid\":\"" + uidToString(transactions[i].uid, transactions[i].uidLen) + "\"}";
    }
    json += "],\"total\":" + String(txCount);
    json += ",\"uptime\":" + String((millis() - bootTime) / 1000);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/analytics/clear", HTTP_POST, []() {
    recordCount = 0;
    currentHour = 0;
    txCount = 0;
    saveDb();
    saveTx();
    server.send(200, "application/json", "{\"ok\":true}");
  });

  server.on("/threshold", HTTP_POST, []() {
    if (server.hasArg("slot") && server.hasArg("value")) {
      int slot = server.arg("slot").toInt() - 1;
      float val = server.arg("value").toFloat();
      if (slot >= 0 && slot < NUM_SLOTS && val > 0) {
        threshold[slot] = val;
        server.send(200, "application/json", "{\"ok\":true}");
        return;
      }
    }
    server.send(400, "application/json", "{\"ok\":false}");
  });

  server.on("/nfc/clear", HTTP_POST, []() {
    lastUid = "";
    server.send(200, "application/json", "{\"ok\":true}");
  });

  server.begin();
  Serial.println("Web server started");

  lastHourCheck = millis();
  lastSampleTime = millis();
}

void loop() {
  server.handleClient();

  for (int i = 0; i < NUM_SLOTS; i++) {
    distance[i] = readSensor(i);
  }

  if (nfcReady) checkNfc();

  unsigned long now = millis();
  if (now - lastSampleTime >= 10000) {
    lastSampleTime = now;
    sampleSlots();
  }

  if (now - lastHourCheck >= 3600000UL) {
    lastHourCheck = now;
    commitHour();
  }

  delay(100);
}
