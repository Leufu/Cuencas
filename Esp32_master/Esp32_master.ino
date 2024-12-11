#include <Arduino.h>
#include <Wire.h>
#include "ADS.h"
#include "MicroSD.h"
#include "Task.h"
#include "RTC.h" // Incluir el archivo de inicialización del RTC


void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando setup...");
  Wire.begin();

  initRTC();  // se Inicializa el RTC primero para evitar memory fault al iniciar la SD  
  initSD();
  initADS();
  initTasks();
  

  Serial.println("Setup completado.");
}

void loop() {
  // Bucle vacío ya que las tareas son manejadas por FreeRTOS al ejecutar initTasks()
}
