#ifndef MICROSD_H
#define MICROSD_H

#include <SPI.h>
#include <SD.h>
#include <FS.h>

const int chipSelect = 10;  // Pin CS para la tarjeta SD en la ESP32 
const char* archivo = "/test.txt";
File dataFile;

//Definición del handle de la tarea
Taskhandle_t sdTaskHandle =NULL;

void initSD() {
  SPI.begin(13, 12, 11, chipSelect);  // SCK=13, MISO=12, MOSI=11, CS=10
  
  if (!SD.begin(chipSelect)) {  
    Serial.println("Fallo al montar la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD montada correctamente.");
}

// Función para verificar y crear el archivo si no existe
void createFileIfNotExist() {
  if (!SD.exists(archivo)) {
    Serial.println("El archivo no existe. Creando un nuevo archivo...");
    dataFile = SD.open(archivo, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Lectura de datos desde el ADS1115:");
      dataFile.println("ADC Value ");
      dataFile.close();
      Serial.println("Archivo creado y cabecera escrita correctamente.");
    } else {
      Serial.println("Error al crear el archivo.");
    }
  } else {
    Serial.println("El archivo ya existe.");
  }
}
// Función para guardar datos en la SD
void saveData(int16_t rawValue) {
  dataFile = SD.open(archivo, FILE_APPEND);
  if (dataFile) {
    dataFile.print("ADS1 : ");
    dataFile.println(rawValue); 
    dataFile.close();
    Serial.println("Datos guardados en la tarjeta SD.");
  } else {
    Serial.println("Error al abrir el archivo para escritura.");
  }
}

// Función de la tarea FreeRTOS para escribir datos en la SD
void sdTask(void *parameter) {
  // Recupera el valor pasado a la tarea
  int16_t rawValue = *((int16_t*)parameter);

  // Asegúrate de que el archivo exista
  createFileIfNotExist();

  // Guardar los datos en la SD
  saveData(rawValue);

  // Termina la tarea
  vTaskDelete(NULL);
}

// Función para crear la tarea de escritura en la SD
void createSDTask(int16_t rawValue, UBaseType_t priority) {
  // Crear un valor temporal que será pasado a la tarea
  int16_t *taskParam = (int16_t*)pvPortMalloc(sizeof(int16_t));
  *taskParam = rawValue;  // Asignar el valor que se va a pasar

  xTaskCreate(
    sdTask,             // Función que ejecutará la tarea
    "SDWriteTask",      // Nombre de la tarea (para depuración)
    2048*2,             // Tamaño de la pila de la tarea
    (void*)taskParam,   // Parámetro para pasar a la tarea
    20,                 // Prioridad de la tarea
    &sdTaskHandle       // Handle de la tarea
  );
}

#endif


