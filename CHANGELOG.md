# Changelog

Semua perubahan penting pada proyek ini akan didokumentasikan di file ini.

Format berdasarkan [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
dan proyek ini mengikuti [Semantic Versioning](https://semver.org/lang/id/).

## [1.0.1] - 2026-07-12

### Fixed
- **Kritis:** Instance global `IskakStorage` sekarang benar-benar didefinisikan di `IskakINO_Storage.cpp` (`extern IskakINO_Storage IskakStorage;` di header sebelumnya di-comment). Sebelumnya, contoh penggunaan di README (`IskakStorage.begin(...)`) tidak bisa di-compile/link tanpa mendeklarasikan instance secara manual di sketch.
- `save()` sekarang mengembalikan status tulis yang sebenarnya:
  - ESP32: memvalidasi jumlah byte yang berhasil ditulis dari `Preferences::putBytes()`.
  - ESP8266: memvalidasi `File::open()` dan jumlah byte hasil `File::write()`.
  - Sebelumnya fungsi ini selalu mengembalikan `true` meskipun penulisan gagal (storage penuh, file gagal dibuka, dsb).
- `load()` pada ESP8266 sekarang memvalidasi hasil `LittleFS.open()` sebelum memanggil `read()`, dan memeriksa jumlah byte yang benar-benar terbaca.
- Ditambahkan bounds-check alamat terhadap `EEPROM.length()` pada `save()` dan `load()` di jalur AVR untuk mencegah penulisan/pembacaan di luar batas EEPROM.

### Changed
- Tanda tangan method internal `_printDebug()` diubah dari `String` menjadi `const __FlashStringHelper*`, agar konsisten dengan seluruh pemanggilan `F(...)` di kode dan lebih hemat RAM pada board AVR.
- Contoh-contoh sketch (`examples/*.ino`) diperbarui: baris deklarasi/definisi manual `IskakINO_Storage IskakStorage;` dihapus dari tiap sketch, karena instance global kini sudah disediakan otomatis oleh library (mencegah error linker *"multiple definition of `IskakStorage`"*).

### Documentation
- Tabel "Struktur Memori" pada README dikoreksi: ukuran header sebenarnya 8 byte (bukan 6 byte), karena `uint32_t` pada `DataWrapper` menyebabkan 2 byte padding alignment sebelum CRC32.
- Ditambahkan dokumentasi method `clear()` (Factory Reset) yang sebelumnya sudah ada di kode namun belum tercantum di README.
- `keywords.txt` dibersihkan dari method yang tidak pernah ada di kode (`isValid`, `validate`, `commit`).

## [1.0.0] - 2026-04-23

### Added
- Rilis awal IskakINO_Storage.
- Hybrid storage engine otomatis sesuai platform:
  - ESP32 menggunakan `Preferences` (NVS).
  - ESP8266 menggunakan `LittleFS`.
  - AVR (Uno/Nano/Mega, dll.) menggunakan `EEPROM` langsung.
- Proteksi integritas data via CRC32 dan validasi *magic byte*.
- Smart wear leveling pada jalur AVR (melewati penulisan jika data tidak berubah).
- API generic template `save<T>()` / `load<T>()` untuk tipe data apa pun.
- Method `clear()` untuk factory reset seluruh storage.
- Continuous Integration: workflow `compile_test.yml` untuk pengujian compile otomatis.

[1.0.1]: https://github.com/iskakfatoni/IskakINO_Storage/compare/1.0.0...1.0.1
[1.0.0]: https://github.com/iskakfatoni/IskakINO_Storage/releases/tag/1.0.0
