
#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>


//#include "Tasks.h"
#include "ADS.h"       // Funciones de ADS para leer el sensor
#include "MicroSD.h"    // Funciones de microSD para guardar datos
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



#define STACK_SIZE 1024 * 4  // Tamaño de la pila para las tareas
#define READ_DELAY 1000      // 1 segundo entre lecturas
#define WRITE_DELAY 1500     // 1.5 segundos entre escrituras en la SD

void initTasks(); // Función para inicializar las tareas


int16_t globalRawValue = 0;   // Variable global para compartir el valor leído

// Prototipos de las tareas
void vTaskADS(void *pvParameters);
void vTaskSD(void *pvParameters);

void initTasks() {
  // Crear la tarea de lectura del ADS1115
  xTaskCreatePinnedToCore(
    vTaskADS,         // Función de la tarea
    "TaskADS",        // Nombre de la tarea
    STACK_SIZE,       // Tamaño de la pila
    NULL,             // Parámetro de entrada
    1,                // Prioridad de la tarea
    NULL,             // Manejador de la tarea
    0                 // CPU en la que se ejecutará
  );

  // Crear la tarea de escritura en la microSD
  xTaskCreatePinnedToCore(
    vTaskSD,          // Función de la tarea
    "TaskSD",         // Nombre de la tarea
    STACK_SIZE,       // Tamaño de la pila
    NULL,             // Parámetro de entrada
    1,                // Prioridad de la tarea
    NULL,             // Manejador de la tarea
    1                 // CPU en la que se ejecutará
  );
}

// Función de tarea FreeRTOS para leer el valor del ADS1115
void vTaskADS(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Leer el canal 0 del ADS1115 en modo continuo
    globalRawValue = readChannelRaw(ADS1115_COMP_0_GND);

    // Mostrar el valor en el monitor serie para verificar
    Serial.print("Valor del ADS: ");
    Serial.println(globalRawValue);

    vTaskDelay(pdMS_TO_TICKS(READ_DELAY));  // Esperar el siguiente ciclo de lectura
  }
}

// Función de la tarea FreeRTOS para guardar el valor en la microSD
void vTaskSD(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Guardar el valor actual en la microSD
    saveData(globalRawValue);

    // Mensaje de confirmación en el monitor serie
    Serial.println("Datos guardados en la tarjeta SD.");

    vTaskDelay(pdMS_TO_TICKS(WRITE_DELAY));  // Esperar el siguiente ciclo de escritura
  }
}

#endif
