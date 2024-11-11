#include "MicroSD.h"
#include <SPI.h>
#include <Arduino.h>

#define SD_CS_PIN 10 //Pin CS para la SD en la esp32

File dataFile;

//Ahora se mantiene abierta la tajeta SD
void initSD() {
  Serial.println("Inicializando tarjeta SD...");
  SPI.begin(13, 12, 11, SD_CS_PIN); // SCK=13, MISO=12, MOSI=11, CS=10
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD.");
    while (1);
  }
  Serial.println("Tarjeta SD inicializada.");

  dataFile = SD.open("/data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error al abrir el archivo en la tarjeta SD.");
    while (1);
  }
  Serial.println("Archivo de datos abierto.");
  // Opcionalmente escribir encabezado
  dataFile.println("Datos del ADS1115");
  dataFile.flush();
}
