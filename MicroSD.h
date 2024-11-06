#ifndef MICROSD_H
#define MICROSD_H

#include <SPI.h>
#include <SD.h>
#include <FS.h>

const int chipSelect = 10;  // Pin CS para la tarjeta SD en la ESP32 
const char* archivo = "/test.txt";
File dataFile;

void initSD() {
  SPI.begin(13, 12, 11, chipSelect);  // SCK=13, MISO=12, MOSI=11, CS=10
  
  if (!SD.begin(chipSelect)) {  
    Serial.println("Fallo al montar la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD montada correctamente.");
}

void writeHeader() {
  if (!SD.exists(archivo)) {
    dataFile = SD.open(archivo, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Lectura de datos desde el ADS1115:");
      dataFile.println("ADC Value ");
      dataFile.close();
      Serial.println("Cabecera escrita correctamente.");
    } else {
      Serial.println("Error al abrir el archivo para escritura.");
    }
  }
}

void saveData(int16_t rawValue) {
  dataFile = SD.open(archivo, FILE_APPEND);
  if (dataFile) {
    dataFile.print("ADS0 : ");
    dataFile.println(rawValue); 
    dataFile.close();
    Serial.println("Datos guardados en la tarjeta SD.");
  } else {
    Serial.println("Error al abrir el archivo para escritura.");
  }
}

#endif
