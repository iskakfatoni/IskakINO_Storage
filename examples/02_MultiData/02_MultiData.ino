/*
 * examples/02_MultiData/02_MultiData.ino
 * Menyimpan beberapa variabel/struct di alamat (index) yang berbeda.
 */

#include <IskakINO_Storage.h>
IskakINO_Storage IskakStorage;

void setup() {
  Serial.begin(115200);
  IskakStorage.begin("multi_data", true);

  // Data 1: Sebuah Integer di Index 0
  int userScore = 950;
  IskakStorage.save(0, userScore);

  // Data 2: Sebuah String (array of char) di Index 1
  char username[20] = "iskakfatoni";
  IskakStorage.save(1, username);

  Serial.println(F("Semua data disimpan di index berbeda."));

  // --- Ambil Kembali ---
  int loadScore;
  char loadName[20];

  if (IskakStorage.load(0, loadScore)) {
    Serial.print(F("Score di Index 0: ")); Serial.println(loadScore);
  }

  if (IskakStorage.load(1, loadName)) {
    Serial.print(F("Nama di Index 1 : ")); Serial.println(loadName);
  }
}

void loop() {}
