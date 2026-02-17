/*
 * examples/06_BootCounter/06_BootCounter.ino
 * Menghitung jumlah berapa kali perangkat dinyalakan (Boot Counter).
 * Menunjukkan fitur Wear Leveling: Data hanya ditulis jika berubah.
 */

#include <IskakINO_Storage.h>
IskakINO_Storage IskakStorage;

struct DeviceStats {
  uint32_t totalBoots;
  uint32_t lastUptime;
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inisialisasi library
  IskakStorage.begin("stats_log", true);

  DeviceStats stats;

  // 1. Coba muat data statistik
  if (!IskakStorage.load(50, stats)) {
    Serial.println(F("Data baru: Memulai penghitungan dari 0."));
    stats.totalBoots = 0;
    stats.lastUptime = 0;
  }

  // 2. Tambahkan jumlah boot
  stats.totalBoots++;
  
  Serial.println(F("-----------------------------------"));
  Serial.print(F("Jumlah Boot kumulatif: ")); 
  Serial.println(stats.totalBoots);
  Serial.println(F("-----------------------------------"));

  // 3. Simpan kembali
  // Library akan mengecek: Jika totalBoots tetap sama, penulisan fisik ke Flash dibatalkan
  if (IskakStorage.save(50, stats)) {
    Serial.println(F("Statistik berhasil diperbarui di memori."));
  }

  // 4. Test Wear Leveling (Simulasi simpan data yang sama)
  Serial.println(F("Mencoba simpan data yang sama (Test Wear Leveling)..."));
  IskakStorage.save(50, stats); // Perhatikan log Debug, seharusnya muncul "Skipped" di AVR
}

void loop() {
  // Biarkan kosong
}
