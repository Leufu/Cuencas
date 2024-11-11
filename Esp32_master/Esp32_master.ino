#include <Arduino.h>
#include <Wire.h>
#include "ADS.h"
#include "MicroSD.h"
#include "Tasks.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando setup...");
  Wire.begin();

  initSD();
  initADS();
  initTasks();

  Serial.println("Setup completado.");
}

void loop() {
  // Bucle vacío ya que las tareas son manejadas por FreeRTOS al ejecutar initTasks()
}
