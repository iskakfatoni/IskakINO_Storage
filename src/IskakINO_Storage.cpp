/*
 * src/IskakINO_Storage.cpp
 * Implementasi instance global untuk IskakINO_Storage.
 * Digunakan untuk mengalokasikan memori objek IskakStorage agar dapat diakses 
 * di seluruh file project tanpa konflik definisi.
 *
 * Fixed: instance IskakStorage sebelumnya tidak pernah didefinisikan,
 * sehingga contoh penggunaan di README (IskakStorage.begin(...)) gagal
 * di-link. File ini sekarang benar-benar mendefinisikan objeknya, sesuai
 * dengan deklarasi `extern IskakINO_Storage IskakStorage;` di header.
 *
 * Created for: iskakfatoni (2026-02-16)
 */

#include "IskakINO_Storage.h"

IskakINO_Storage IskakStorage;
