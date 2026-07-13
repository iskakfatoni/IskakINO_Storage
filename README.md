# IskakINO_Storage 💾
**Advanced Hybrid Storage Engine for Arduino, ESP8266, and ESP32.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: AVR/ESP](https://img.shields.io/badge/Platform-AVR%20%7C%20ESP8266%20%7C%20ESP32-blue.svg)](#)
[![Compile Test](https://github.com/iskakfatoni/IskakINO_Storage/actions/workflows/compile_test.yml/badge.svg)](https://github.com/iskakfatoni/IskakINO_Storage/actions/workflows/compile_test.yml)

IskakINO_Storage adalah library penyimpanan cerdas yang secara otomatis memilih "Engine" terbaik berdasarkan board yang Anda gunakan. Library ini menggabungkan kemudahan penggunaan dengan keamanan data tingkat tinggi (CRC32) dan perlindungan umur memori (Wear Leveling).

## 🚀 Fitur Utama

- **Hybrid Engine**: 
  - **ESP32**: Menggunakan `Preferences` (NVS) - Sangat aman & berbasis Key-Value.
  - **ESP8266**: Menggunakan `LittleFS` - Sistem file modern untuk menjaga umur Flash.
  - **AVR (Uno/Nano)**: Menggunakan `EEPROM` - Ringan dan efisien.
- **Integritas Data (CRC32)**: Setiap data yang disimpan diproteksi dengan Checksum 4-byte. Jika data korup (akibat kegagalan hardware atau power-loss), library akan mendeteksinya.
- **Smart Wear Leveling**: Menghindari penulisan ulang ke memori jika data yang ingin disimpan identik dengan data lama.
- **Magic Byte Validation**: Memastikan data yang dibaca adalah data valid milik library ini.
- **Generic Template**: Mendukung penyimpanan tipe data apa pun (`struct`, `int`, `float`, `array`) tanpa perlu konversi manual.
- **Factory Reset (`clear()`)**: Menghapus seluruh data tersimpan sesuai platform — `Preferences.clear()` di ESP32, hapus semua file di ESP8266 (LittleFS), atau reset seluruh EEPROM ke `0xFF` di AVR.
- **Status Tulis Terverifikasi**: `save()` mengembalikan `true` hanya jika data benar-benar berhasil ditulis ke storage (bukan sekadar diasumsikan berhasil).
- **Bounds-Check EEPROM (AVR)**: `save()`/`load()` memvalidasi `address` terhadap `EEPROM.length()` sebelum menulis/membaca, mencegah overflow ke luar batas EEPROM.

## 🛠️ Struktur Memori (Internal)

Library ini membungkus data Anda dalam paket keamanan berikut:

| Offset | Ukuran | Deskripsi |
| :--- | :--- | :--- |
| 0 | 1 Byte | **Magic Byte** (Validasi Identitas) |
| 1 | 1 Byte | **Version** (Kontrol Skema Data) |
| 2-5 | 2 Byte | *Padding* (menyesuaikan alignment 4-byte sebelum CRC32) |
| 6-9 | 4 Byte | **CRC32 Checksum** (Integritas Data) |
| 10+ | Variabel | **User Data** (Data Anda Sebenarnya) |

> ⚠️ **Catatan:** Ukuran header sebenarnya adalah `sizeof(DataWrapper)` (8 byte pada kebanyakan platform, termasuk 2 byte padding compiler untuk alignment `uint32_t`), bukan 6 byte. Selalu gunakan `sizeof(DataWrapper)` internal library — jangan hardcode offset 6 jika Anda mem-parsing data mentah secara manual dari EEPROM/flash.

## 💻 Cara Penggunaan

### 1. Inisialisasi
Panggil di bagian `setup()`.
```cpp
#include <IskakINO_Storage.h>

void setup() {
  Serial.begin(115200);
  // name: namespace untuk ESP32 / prefix file untuk ESP8266
  // debug: true untuk melihat laporan di Serial Monitor
  IskakStorage.begin("my_project", true);
}
```
### 2. Menyimpan Data (Save)
Anda bisa menyimpan tipe data apa pun, mulai dari variabel sederhana hingga `struct` yang kompleks. Library akan secara otomatis menghitung CRC32 sebelum menyimpan, dan mengembalikan `true`/`false` sesuai keberhasilan penulisan aktual ke storage.

```cpp
// Contoh menggunakan struct untuk konfigurasi
struct MyConfig {
  char ssid[32];
  char password[32];
  int port;
  float calibration;
};

MyConfig config = {"Rumah_Pintar", "rahasia123", 8080, 1.25};

// Menyimpan ke alamat/index 0
if (IskakStorage.save(0, config)) {
  Serial.println("Data berhasil ditulis ke storage.");
} else {
  Serial.println("Gagal menulis data (storage penuh / alamat di luar batas / IO error).");
}
```
### 3. Membaca Data (Load)
Fungsi `load` tidak hanya mengambil data, tetapi juga memverifikasi integritasnya menggunakan **CRC32**. Jika data di memori berubah atau korup, fungsi ini akan mengembalikan nilai `false`.
```cpp
MyConfig savedData;

// Mencoba memuat data dari alamat 0
if (IskakStorage.load(0, savedData)) {
  Serial.println("Berhasil: Integritas data valid (CRC OK)");
  Serial.print("SSID: "); Serial.println(savedData.ssid);
} else {
  Serial.println("Gagal: Data korup, Magic Byte salah, alamat di luar batas, atau belum pernah disimpan");
  
  // Opsi: Muat nilai default jika load gagal
  // config = defaultSettings;
}
```

### 4. Menghapus Semua Data (Factory Reset)
Gunakan `clear()` untuk mereset seluruh storage ke kondisi kosong — berguna untuk fitur "Factory Reset" pada perangkat Anda.
```cpp
// Menghapus SELURUH data tersimpan (bukan hanya satu slot)
IskakStorage.clear();
```
> ⚠️ **Perhatian:** `clear()` bersifat global — menghapus **semua** data yang tersimpan lewat library ini di storage yang bersangkutan (semua key Preferences di ESP32, semua file di LittleFS ESP8266, atau seluruh isi EEPROM di AVR), bukan hanya satu `address` tertentu.

## 📂 Instalasi Manual
Jika Anda tidak menggunakan Library Manager, Anda dapat menginstal library ini secara manual:
1. Klik tombol **Code** di bagian atas repositori ini, lalu pilih **Download ZIP**.
2. Buka Arduino IDE Anda.
3. Pergi ke menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.
4. Pilih file ZIP yang baru saja Anda download.
5. Library siap digunakan! Anda dapat menemukannya di bawah menu **File** -> **Examples** -> **IskakINO_Storage**.

## 🤝 Kontribusi
Kontribusi adalah apa yang membuat komunitas open source menjadi tempat yang luar biasa untuk belajar, menginspirasi, dan berkreasi. Setiap kontribusi yang Anda berikan sangat **sangat dihargai**.
1. Fork Proyek ini.
2. Buat Feature Branch Anda (`git checkout -b feature/FiturLuarBiasa`).
3. Commit Perubahan Anda (`git commit -m 'Menambahkan Fitur Luar Biasa'`).
4. Push ke Branch tersebut (`git push origin feature/FiturLuarBiasa`).
5. Buka Pull Request.
---

**Dibuat oleh [iskakfatoni](https://github.com/iskakfatoni)** *Memberikan solusi penyimpanan yang aman dan cerdas untuk ekosistem Arduino.*
