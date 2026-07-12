/*
 * src/IskakINO_Storage.h
 * Hybrid & Advanced Storage Engine (EEPROM, LittleFS, Preferences)
 * Updated: Added clear() method for Factory Reset
 * Fixed:
 *   - extern instance declaration diaktifkan (sebelumnya di-comment,
 *     menyebabkan IskakStorage tidak dikenali linker)
 *   - save() sekarang mengembalikan status tulis yang sebenarnya
 *     (bukan selalu true)
 *   - load() (ESP8266) sekarang mengecek hasil open() sebelum read()
 *   - Bounds-check alamat terhadap EEPROM.length() pada jalur AVR
 */

#ifndef ISKAKINO_STORAGE_H
#define ISKAKINO_STORAGE_H

#include <Arduino.h>

#if defined(ESP32)
  #include <Preferences.h>  
#elif defined(ESP8266)
  #include <LittleFS.h>     
#else
  #include <EEPROM.h>       
#endif

#define ISKAK_STORAGE_MAGIC 0x49
#define ISKAK_STORAGE_VERSION 0x03

class IskakINO_Storage {
  private:
    bool _debug;
    const char* _namespace;

    #if defined(ESP32)
      Preferences _prefs;
    #endif

    struct DataWrapper {
      uint8_t magic;
      uint8_t version;
      uint32_t crc;
    };

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

    void _printDebug(const __FlashStringHelper* msg) {
      if (_debug) {
        Serial.print(F("[IskakStorage] "));
        Serial.println(msg);
      }
    }

  public:
    void begin(const char* name = "iskak_store", bool debugMode = false) {
      _namespace = name;
      _debug = debugMode;
      #if defined(ESP32)
        _prefs.begin(_namespace, false);
      #elif defined(ESP8266)
        LittleFS.begin();
      #endif
      _printDebug(F("Hybrid Engine Ready with CRC32 Protection."));
    }

    // --- CLEAR (FACTORY RESET) ---
    void clear() {
      _printDebug(F("Clearing all data (Factory Reset)..."));
      #if defined(ESP32)
        _prefs.clear();
      #elif defined(ESP8266)
        Dir dir = LittleFS.openDir("/");
        while (dir.next()) {
          LittleFS.remove(dir.fileName());
        }
      #else
        // Untuk AVR (EEPROM), kita set semua ke 0xFF (default EEPROM kosong)
        for (int i = 0 ; i < (int)EEPROM.length() ; i++) {
          if(EEPROM.read(i) != 0xFF) EEPROM.write(i, 0xFF);
        }
      #endif
      _printDebug(F("Storage cleared successfully."));
    }

    template <typename T>
    bool save(int address, const T& value) {
      DataWrapper header;
      header.magic = ISKAK_STORAGE_MAGIC;
      header.version = ISKAK_STORAGE_VERSION;
      header.crc = _calculateCRC32((const uint8_t*)&value, sizeof(T));

      uint8_t buffer[sizeof(DataWrapper) + sizeof(T)];
      memcpy(buffer, &header, sizeof(DataWrapper));
      memcpy(buffer + sizeof(DataWrapper), &value, sizeof(T));

      #if defined(ESP32)
        char key[15]; sprintf(key, "a%d", address);
        size_t written = _prefs.putBytes(key, buffer, sizeof(buffer));
        if (written != sizeof(buffer)) {
          _printDebug(F("ESP32: Write failed / incomplete."));
          return false;
        }
        return true;
      #elif defined(ESP8266)
        char path[20]; sprintf(path, "/s%d.bin", address);
        File f = LittleFS.open(path, "w");
        if (!f) {
          _printDebug(F("ESP8266: Failed to open file for write."));
          return false;
        }
        size_t written = f.write(buffer, sizeof(buffer));
        f.close();
        if (written != sizeof(buffer)) {
          _printDebug(F("ESP8266: Write incomplete."));
          return false;
        }
        return true;
      #else
        // Bounds-check terhadap kapasitas EEPROM
        if (address < 0 || (size_t)address + sizeof(buffer) > (size_t)EEPROM.length()) {
          _printDebug(F("AVR: Address out of EEPROM bounds."));
          return false;
        }
        bool changed = false;
        for(size_t i=0; i<sizeof(buffer); i++) {
          if(EEPROM.read(address + i) != buffer[i]) { changed = true; break; }
        }
        if(changed) {
          for(size_t i=0; i<sizeof(buffer); i++) EEPROM.write(address + i, buffer[i]);
          _printDebug(F("AVR: EEPROM Updated."));
        } else {
          _printDebug(F("AVR: Skipped (No Change)."));
        }
        return true;
      #endif
    }

    template <typename T>
    bool load(int address, T& value) {
      DataWrapper header;
      uint8_t buffer[sizeof(DataWrapper) + sizeof(T)];

      #if defined(ESP32)
        char key[15]; sprintf(key, "a%d", address);
        if (_prefs.getBytes(key, buffer, sizeof(buffer)) != sizeof(buffer)) return false;
      #elif defined(ESP8266)
        char path[20]; sprintf(path, "/s%d.bin", address);
        if (!LittleFS.exists(path)) return false;
        File f = LittleFS.open(path, "r");
        if (!f) return false;
        size_t readBytes = f.read(buffer, sizeof(buffer));
        f.close();
        if (readBytes != sizeof(buffer)) return false;
      #else
        if (address < 0 || (size_t)address + sizeof(buffer) > (size_t)EEPROM.length()) {
          _printDebug(F("AVR: Address out of EEPROM bounds."));
          return false;
        }
        for(size_t i=0; i<sizeof(buffer); i++) buffer[i] = EEPROM.read(address + i);
      #endif

      memcpy(&header, buffer, sizeof(DataWrapper));
      if (header.magic != ISKAK_STORAGE_MAGIC) return false;

      memcpy(&value, buffer + sizeof(DataWrapper), sizeof(T));
      uint32_t currentCrc = _calculateCRC32((const uint8_t*)&value, sizeof(T));
      
      return (currentCrc == header.crc);
    }
};

extern IskakINO_Storage IskakStorage;
#endif
