/*
 * examples/03_FactoryReset/03_FactoryReset.ino
 * Demonstrasi menghapus seluruh data (Factory Reset) 
 * dan memvalidasi status data setelah dihapus.
 */

#include <IskakINO_Storage.h>

struct DeviceConfig {
  char owner[20];
  int version;
};

void setup() {
  Serial.begin(115200);
  IskakStorage.begin("reset_demo", true);

  // 1. Simpan data awal
  DeviceConfig cfg = {"iskakfatoni", 1};
  IskakStorage.save(0, cfg);
  Serial.println(F("Data awal tersimpan."));

  // 2. Simulasi Factory Reset
  Serial.println(F("Melakukan Factory Reset (Clear Storage)..."));
  IskakStorage.clear(); 

  // 3. Coba baca kembali setelah dihapus
  DeviceConfig testLoad;
  if (IskakStorage.load(0, testLoad)) {
    Serial.println(F("Error: Data masih ada!"));
  } else {
    Serial.println(F("Sukses: Data berhasil dihapus (Integritas Invalid)."));
  }
}

void loop() {}
