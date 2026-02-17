/*
 * src/IskakINO_Storage.h
 * Hybrid & Advanced Storage Engine (EEPROM, LittleFS, Preferences)
 * Updated: Added clear() method for Factory Reset
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

    void _printDebug(String msg) {
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

    // --- FUNGSI BARU: CLEAR (FACTORY RESET) ---
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
      header.crc = _calculateCRC32((uint8_t*)&value, sizeof(T));

      uint8_t buffer[sizeof(DataWrapper) + sizeof(T)];
      memcpy(buffer, &header, sizeof(DataWrapper));
      memcpy(buffer + sizeof(DataWrapper), &value, sizeof(T));

      #if defined(ESP32)
        char key[15]; sprintf(key, "a%d", address);
        _prefs.putBytes(key, buffer, sizeof(buffer));
      #elif defined(ESP8266)
        char path[20]; sprintf(path, "/s%d.bin", address);
        File f = LittleFS.open(path, "w");
        if (f) { f.write(buffer, sizeof(buffer)); f.close(); }
      #else
        uint8_t current[sizeof(buffer)];
        bool changed = false;
        for(size_t i=0; i<sizeof(buffer); i++) {
          current[i] = EEPROM.read(address + i);
          if(current[i] != buffer[i]) changed = true;
        }
        if(changed) {
          for(size_t i=0; i<sizeof(buffer); i++) EEPROM.write(address + i, buffer[i]);
          _printDebug(F("AVR: EEPROM Updated."));
        } else {
          _printDebug(F("AVR: Skipped (No Change)."));
        }
      #endif
      return true;
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
        f.read(buffer, sizeof(buffer)); f.close();
      #else
        for(size_t i=0; i<sizeof(buffer); i++) buffer[i] = EEPROM.read(address + i);
      #endif

      memcpy(&header, buffer, sizeof(DataWrapper));
      if (header.magic != ISKAK_STORAGE_MAGIC) return false;

      memcpy(&value, buffer + sizeof(DataWrapper), sizeof(T));
      uint32_t currentCrc = _calculateCRC32((uint8_t*)&value, sizeof(T));
      
      return (currentCrc == header.crc);
    }
};

//extern IskakINO_Storage IskakStorage;
#endif
