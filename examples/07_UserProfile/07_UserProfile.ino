/*
 * examples/07_UserProfile/07_UserProfile.ino
 * Menyimpan profil pengguna dan kredensial akses secara aman.
 */

#include <IskakINO_Storage.h>
IskakINO_Storage IskakStorage;

// Struktur data profil
struct UserProfile {
  char username[16];
  char pin[8];
  uint8_t accessLevel; // 1: Admin, 2: User, 3: Guest
  bool isActive;
};

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inisialisasi library (Namespace: "security", Debug: ON)
  IskakStorage.begin("security", true);

  UserProfile user;

  Serial.println(F("Memeriksa database pengguna..."));

  // Mencoba memuat data profil dari index 100
  if (IskakStorage.load(100, user)) {
    Serial.println(F("Profil Pengguna Ditemukan:"));
    Serial.print(F(" - Nama  : ")); Serial.println(user.username);
    Serial.print(F(" - Level : ")); 
    if(user.accessLevel == 1) Serial.println(F("Administrator"));
    else Serial.println(F("Standard User"));
  } 
  else {
    Serial.println(F("Profil belum ada. Membuat profil Administrator default..."));

    // Mengisi data profil baru
    strncpy(user.username, "iskakfatoni", 16);
    strncpy(user.pin, "1234", 8);
    user.accessLevel = 1;
    user.isActive = true;

    // Simpan ke memori
    if (IskakStorage.save(100, user)) {
      Serial.println(F("Profil Administrator berhasil diamankan."));
    }
  }
}

void loop() {
  // Sistem keamanan berjalan...
}
