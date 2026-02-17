/*
 * examples/08_SystemBlackbox/08_SystemBlackbox.ino
 * Menyimpan log kejadian terakhir (crash/error code) untuk debugging.
 
 */

#include <IskakINO_Storage.h>
IskakINO_Storage IskakStorage;

// Struktur untuk menyimpan status sistem terakhir
struct SystemLog {
  uint32_t lastTimestamp;
  uint16_t errorCode;
  char lastAction[20];
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inisialisasi library
  IskakStorage.begin("system_log", true);

  SystemLog lastLog;

  Serial.println(F("Mengecek catatan Blackbox terakhir..."));

  // Muat log dari index 200
  if (IskakStorage.load(200, lastLog)) {
    Serial.println(F("--- LAPORAN SEBELUM REBOOT ---"));
    Serial.print(F("Timestamp  : ")); Serial.println(lastLog.lastTimestamp);
    Serial.print(F("Error Code : ")); Serial.println(lastLog.errorCode);
    Serial.print(F("Aksi Akhir : ")); Serial.println(lastLog.lastAction);
    Serial.println(F("-------------------------------"));

    // Setelah dibaca, kita bisa membersihkan log atau menandainya sebagai 'read'
  } else {
    Serial.println(F("Tidak ada catatan error. Sistem sebelumnya mati secara normal."));
  }

  // Simulasi: Kejadian error terjadi di tengah jalan
  Serial.println(F("Simulasi: Terjadi error pada sensor..."));
  
  SystemLog currentError;
  currentError.lastTimestamp = 1708089999; // Contoh Unix Timestamp
  currentError.errorCode = 404;            // Misal: Sensor Not Found
  strncpy(currentError.lastAction, "Read_DHT22", 20);

  // Simpan ke Blackbox
  if (IskakStorage.save(200, currentError)) {
    Serial.println(F("Error tercatat di Blackbox. Siap untuk reboot."));
  }
}

void loop() {
  // Biarkan kosong
}
