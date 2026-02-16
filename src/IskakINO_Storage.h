/*
 * src/IskakINO_Storage.h
 * Library for: iskakfatoni
 * Multi-platform: AVR, ESP8266, ESP32
 */

#ifndef ISKAKINO_STORAGE_H
#define ISKAKINO_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>

#define ISKAK_STORAGE_MAGIC 0x49  // 'I'
#define ISKAK_STORAGE_VERSION 0x01

class IskakINO_Storage {
  private:
    int _size;
    bool _debug;
    const int _offset = 2; 

    void _printDebug(String msg) {
      if (_debug) {
        Serial.print(F("[IskakStorage] "));
        Serial.println(msg);
      }
    }

  public:
    void begin(int size = 512, bool debugMode = false) {
      _size = size;
      _debug = debugMode;
      #if defined(ESP8266) || defined(ESP32)
        EEPROM.begin(_size);
      #endif
      _printDebug(F("Storage Initialized."));
    }

    bool isValid() {
      return (EEPROM.read(0) == ISKAK_STORAGE_MAGIC && EEPROM.read(1) == ISKAK_STORAGE_VERSION);
    }

    void validate() {
      EEPROM.write(0, ISKAK_STORAGE_MAGIC);
      EEPROM.write(1, ISKAK_STORAGE_VERSION);
      commit();
      _printDebug(F("Storage Validated with Magic Byte."));
    }

    template <typename T>
    void save(int address, const T& value) {
      int realAddr = address + _offset;
      T currentVal;
      EEPROM.get(realAddr, currentVal);

      if (memcmp(&currentVal, &value, sizeof(T)) != 0) {
        EEPROM.put(realAddr, value);
        commit();
        _printDebug("Data saved to addr: " + String(realAddr));
      } else {
        _printDebug(F("Data identical, skipping write (Wear Leveling)."));
      }
    }

    template <typename T>
    void load(int address, T& value) {
      int realAddr = address + _offset;
      EEPROM.get(realAddr, value);
    }

    void commit() {
      #if defined(ESP8266) || defined(ESP32)
        EEPROM.commit();
      #endif
    }

    void clear() {
      for (int i = 0; i < _size; i++) EEPROM.write(i, 0);
      commit();
      _printDebug(F("Storage Cleared."));
    }
};

extern IskakINO_Storage IskakStorage;

#endif
