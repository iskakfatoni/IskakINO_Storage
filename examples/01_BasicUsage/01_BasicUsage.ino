/*
 * examples/01_BasicUsage/01_BasicUsage.ino
 * Demonstrasi dasar simpan dan baca struct dengan proteksi CRC32.
 */

#include <IskakINO_Storage.h>

// Definisikan struktur data yang ingin disimpan
struct MySettings {
  int deviceID;
  float targetTemp;
  bool autoMode;
};

void setup() {
  Serial.begin(115200);
  delay(2000); // Tunggu serial monitor siap

  // 1. Inisialisasi engine (Namespace: "basic_app", Debug: true)
  IskakStorage.begin("basic_app", true);

  Serial.println(F("\n--- STEP 1: MENYIMPAN DATA ---"));
  MySettings initialConfig = {101, 25.5, true};
  
  // Simpan data ke index 0
  IskakStorage.save(0, initialConfig);
  Serial.println(F("Data berhasil disimpan ke memori."));

  Serial.println(F("\n--- STEP 2: MEMBACA DATA ---"));
  MySettings loadedConfig;
  
  // Load data dari index 0
  if (IskakStorage.load(0, loadedConfig)) {
    Serial.println(F("Load Berhasil! Data Valid."));
    Serial.print(F("ID Perangkat: ")); Serial.println(loadedConfig.deviceID);
    Serial.print(F("Target Suhu : ")); Serial.println(loadedConfig.targetTemp);
    Serial.print(F("Mode Auto   : ")); Serial.println(loadedConfig.autoMode ? "ON" : "OFF");
  } else {
    Serial.println(F("Load Gagal! Data korup atau belum ada data di index tersebut."));
  }
}

void loop() {
  // Kosong
}
