/*
 * src/IskakINO_Storage.h
 * * Library Penyimpanan Hybrid (EEPROM, LittleFS, Preferences)
 * Dirancang khusus untuk portabilitas antara AVR, ESP8266, dan ESP32.
 * * Author: iskakfatoni
 * Date: 2026-02-16
 * Version: 1.0.0
 */

#ifndef ISKAKINO_STORAGE_H
#define ISKAKINO_STORAGE_H

#include <Arduino.h>

/* * 1. SELEKSI ENGINE OTOMATIS
 * Memilih library sistem penyimpanan terbaik berdasarkan board yang digunakan.
 */
#if defined(ESP32)
  #include <Preferences.h>  // Engine: Key-Value NVS (Paling Aman)
#elif defined(ESP8266)
  #include <LittleFS.h>     // Engine: File System (Lebih baik dari EEPROM emulasi)
#else
  #include <EEPROM.h>       // Engine: Standard EEPROM (Untuk Uno, Nano, Mega)
#endif

// Identitas library untuk memvalidasi data yang tersimpan
#define ISKAK_STORAGE_MAGIC 0x49     // Karakter 'I' sebagai penanda
#define ISKAK_STORAGE_VERSION 0x03   // Versi skema data

class IskakINO_Storage {
  private:
    bool _debug;             // Status mode debug (Serial)
    const char* _namespace;  // Nama ruang penyimpanan (khusus ESP32/Preferences)

    #if defined(ESP32)
      Preferences _prefs;
    #endif

    /* * STRUKTUR DATA WRAPPER
     * Header kecil yang ditempelkan di depan data user untuk keamanan.
     * Total overhead: 6 Byte (1 Magic + 1 Ver + 4 CRC)
     */
    struct DataWrapper {
      uint8_t magic;    // Byte validasi
      uint8_t version;  // Versi library
      uint32_t crc;     // Kode pengecekan integritas data
    };

    /* * FUNGSI KALKULASI CRC32
     * Menghitung nilai unik berdasarkan isi data untuk mendeteksi korupsi data.
     */
    uint32_t _calculateCRC32(const uint8_t *data, size_t length) {
      uint32_t crc = 0xFFFFFFFF;
      for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
          if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
          else crc >>= 1;
        }
      }
      return ~crc;
    }

    void _printDebug(String msg) {
      if (_debug) {
        Serial.print(F("[IskakStorage] "));
        Serial.println(msg);
      }
    }

  public:
    /* * BEGIN: Inisialisasi Engine
     * @param name: Nama namespace (untuk ESP32) atau prefix file (ESP8266)
     * @param debugMode: Aktifkan laporan ke Serial Monitor
     */
    void begin(const char* name = "iskak_store", bool debugMode = false) {
      _namespace = name;
      _debug = debugMode;

      #if defined(ESP32)
        _prefs.begin(_namespace, false);
        _printDebug(F("Engine ESP32: Preferences Active"));
      #elif defined(ESP8266)
        if (!LittleFS.begin()) {
          _printDebug(F("LittleFS Mount Failed!"));
        } else {
          _printDebug(F("Engine ESP8266: LittleFS Active"));
        }
      #else
        _printDebug(F("Engine AVR: Standard EEPROM Active"));
      #endif
    }

    /* * SAVE: Menyimpan data tipe apa pun (int, float, struct)
     * @param address: Indeks lokasi (untuk AVR/LittleFS) atau Key (ESP32)
     * @param value: Variabel yang ingin disimpan
     */
    template <typename T>
    bool save(int address, const T& value) {
      // 1. Siapkan Header
      DataWrapper header;
      header.magic = ISKAK_STORAGE_MAGIC;
      header.version = ISKAK_STORAGE_VERSION;
      header.crc = _calculateCRC32((uint8_t*)&value, sizeof(T));

      // 2. Gabungkan Header + Data ke dalam satu Buffer
      uint8_t buffer[sizeof(DataWrapper) + sizeof(T)];
      memcpy(buffer, &header, sizeof(DataWrapper));
      memcpy(buffer + sizeof(DataWrapper), &value, sizeof(T));

      // 3. Simpan sesuai platform
      #if defined(ESP32)
        char key[15]; sprintf(key, "a%d", address);
        _prefs.putBytes(key, buffer, sizeof(buffer));
      #elif defined(ESP8266)
        char path[20]; sprintf(path, "/s%d.bin", address);
        File f = LittleFS.open(path, "w");
        if (f) { f.write(buffer, sizeof(buffer)); f.close(); }
      #else
        // Fitur Wear Leveling manual untuk AVR: Cek dulu sebelum tulis
        uint8_t current[sizeof(buffer)];
        for(size_t i=0; i<sizeof(buffer); i++) current[i] = EEPROM.read(address + i);
        if(memcmp(current, buffer, sizeof(buffer)) != 0) {
          for(size_t i=0; i<sizeof(buffer); i++) EEPROM.write(address + i, buffer[i]);
          _printDebug(F("AVR: EEPROM Written (Data changed)"));
        } else {
          _printDebug(F("AVR: Write Skipped (Data identical)"));
        }
      #endif
      
      _printDebug("Save Success at addr: " + String(address));
      return true;
    }

    /* * LOAD: Membaca data dan memvalidasi integritasnya
     * @return: true jika data valid, false jika magic byte salah atau CRC korup
     */
    template <typename T>
    bool load(int address, T& value) {
      DataWrapper header;
      uint8_t buffer[sizeof(DataWrapper) + sizeof(T)];

      // 1. Ambil data mentah dari platform
      #if defined(ESP32)
        char key[15]; sprintf(key, "a%d", address);
        if (_prefs.getBytes(key, buffer, sizeof(buffer)) != sizeof(buffer)) return false;
      #elif defined(ESP8266)
        char path[20]; sprintf(path, "/s%d.bin", address);
        if (!LittleFS.exists(path)) return false;
        File f = LittleFS.open(path, "r");
        f.read(buffer, sizeof(buffer)); f.close();
      #else
        for(size_t i=0; i<sizeof(buffer); i++) buffer[i] = EEPROM.read(address + i);
      #endif

      // 2. Pisahkan Header dan cek Magic Byte
      memcpy(&header, buffer, sizeof(DataWrapper));
      if (header.magic != ISKAK_STORAGE_MAGIC) {
        _printDebug(F("Load Failed: Magic Byte Mismatch"));
        return false;
      }

      // 3. Ekstrak data user dan hitung ulang CRC
      memcpy(&value, buffer + sizeof(DataWrapper), sizeof(T));
      uint32_t calculatedCrc = _calculateCRC32((uint8_t*)&value, sizeof(T));
      
      if (calculatedCrc == header.crc) {
        _printDebug(F("Load Success: CRC Match"));
        return true;
      } else {
        _printDebug(F("Load Failed: CRC Mismatch (Data Corrupt)"));
        return false;
      }
    }
};

// Instance eksternal agar bisa langsung dipakai: IskakStorage.begin()
extern IskakINO_Storage IskakStorage;

#endif
