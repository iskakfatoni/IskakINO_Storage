/*
 * examples/04_WiFiConfig/04_WiFiConfig.ino
 * Menyimpan dan memuat kredensial WiFi dengan aman.
 */

#include <IskakINO_Storage.h>

// Struktur untuk menyimpan data WiFi
struct WiFiConfig {
  char ssid[32];
  char pass[32];
  bool useStaticIP;
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inisialisasi library
  IskakStorage.begin("network_cfg", true);

  WiFiConfig myWiFi;

  // 1. Mencoba memuat data yang tersimpan
  Serial.println(F("Mengecek konfigurasi WiFi di memori..."));
  
  if (IskakStorage.load(0, myWiFi)) {
    Serial.println(F("Konfigurasi ditemukan!"));
    Serial.print(F("Menghubungkan ke SSID: ")); Serial.println(myWiFi.ssid);
    // Di sini Anda bisa memanggil WiFi.begin(myWiFi.ssid, myWiFi.pass);
  } 
  else {
    Serial.println(F("Konfigurasi tidak ditemukan atau korup."));
    Serial.println(F("Menyimpan konfigurasi baru sebagai default..."));

    // 2. Jika data tidak ada, buat data baru (Simulasi input dari User)
    strncpy(myWiFi.ssid, "WiFi_Rumah_Iskak", 32);
    strncpy(myWiFi.pass, "passwordSuperRahasia", 32);
    myWiFi.useStaticIP = false;

    // Simpan ke memori
    if (IskakStorage.save(0, myWiFi)) {
      Serial.println(F("Data WiFi baru berhasil diamankan dengan CRC32."));
    }
  }
}

void loop() {
  // Sistem berjalan...
}
