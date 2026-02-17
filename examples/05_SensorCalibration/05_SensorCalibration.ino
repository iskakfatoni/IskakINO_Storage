/*
 * examples/05_SensorCalibration/05_SensorCalibration.ino
 * Menyimpan data kalibrasi sensor (Float) dan history pembacaan terakhir.
 */

#include <IskakINO_Storage.h>
IskakINO_Storage IskakStorage;

// Struktur untuk kalibrasi sensor
struct CalibrationData {
  float offset;
  float multiplier;
  float lastReadings[5]; // Menyimpan 5 data terakhir
  uint32_t lastCalibrated;
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inisialisasi (Namespace: "sensor_lab", Debug: ON)
  IskakStorage.begin("sensor_lab", true);

  CalibrationData sensor1;

  Serial.println(F("Mencoba memuat data kalibrasi..."));

  if (IskakStorage.load(10, sensor1)) { // Kita gunakan index 10
    Serial.println(F("Data Kalibrasi Berhasil Dimuat:"));
    Serial.print(F(" - Offset    : ")); Serial.println(sensor1.offset);
    Serial.print(F(" - Multiplier: ")); Serial.println(sensor1.multiplier);
    Serial.print(F(" - Last Data : ")); Serial.println(sensor1.lastReadings[0]);
  } 
  else {
    Serial.println(F("Data tidak ditemukan. Melakukan kalibrasi awal..."));

    // Set data default
    sensor1.offset = 0.45f;
    sensor1.multiplier = 1.02f;
    sensor1.lastCalibrated = 1708080000; // Contoh timestamp
    
    // Isi array pembacaan
    for(int i=0; i<5; i++) {
      sensor1.lastReadings[i] = 20.0f + i;
    }

    // Simpan data
    if (IskakStorage.save(10, sensor1)) {
      Serial.println(F("Data kalibrasi baru berhasil disimpan ke memori aman."));
    }
  }
}

void loop() {
  // Biarkan kosong
}
