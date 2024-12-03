#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>

//#include "Tasks.h"
#include "ADS.h"
#include "MicroSD.h"
#include "RTC.h" // Incluir el archivo de inicialización del RTC
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

//#include <RTClib.h>
#include <RTClib.h>


#define QUEUE_LENGTH 1024 * 10//Prueba de la queue entre leer y escribir
#define SAMPLE_RATE_HZ 860  

// Manejo global de la cola
QueueHandle_t dataQueue;

// Prototipos de funciones para tareas
void vTaskADS(void *pvParameters);
void vTaskSD(void *pvParameters);

//Función para calcular el tiempo de 860 muestras
void calcularTiempoCada860Muestras() {
  static unsigned long startTime = micros();  // Tiempo de inicio de la medición
  static int muestraContador = 0;  // Contador de muestras

  muestraContador++;

  // Verifica si hemos alcanzado 860 muestras
  if (muestraContador >= 860) {
    unsigned long endTime = micros();  // Tiempo actual
    float tiempoSegundos = (endTime - startTime) / 1e6;  // Convertir de microsegundos a segundos

    // Mostrar el tiempo transcurrido en segundos
    Serial.print("Tiempo para 860 muestras: ");
    Serial.print(tiempoSegundos, 5);  //5 decimales
    Serial.println(" segundos");

    // Reiniciar el contador de muestras y el tiempo de inicio
    muestraContador = 0;
    startTime = micros();
  }
}

void initTasks() {
  Serial.println("Creando cola de datos...");
  // Crear la cola
  dataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(int16_t));
  if (dataQueue == NULL) {
    Serial.println("Error al crear la cola de datos.");
    while (1);
  }
  Serial.println("Cola de datos creada.");

  // Crear tareas
  Serial.println("Creando tareas...");
  xTaskCreatePinnedToCore(vTaskADS, "TaskADS", 4096*2, NULL, 10, NULL, 0); //Core 0, 4096 tamaño del stack de la tarea
  xTaskCreatePinnedToCore(vTaskSD, "TaskSD", 4096*2, NULL, 5, NULL, 1); //Core 1
  Serial.println("Tareas creadas.");
}

// Tarea de lectura
void vTaskADS(void *pvParameters) {
  Serial.println("vTaskADS iniciada.");
  (void)pvParameters;
  const TickType_t xFrequency = pdMS_TO_TICKS(1);  // 1ms de delay (menor tiempo no se puede con el delay de software)
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    // Verifica si el pin ALERT está en alto
   // if (digitalRead(INTERRUPT_PIN) == HIGH) {
      // Leer el valor del ADC
      int16_t adcValue = adc.getRawResult();
      /*Serial.print("Valor crudo: ");
      Serial.println(adcValue);*/
    


    calcularTiempoCada860Muestras();

       // Enviar el valor a la cola
      if (xQueueSend(dataQueue, &adcValue, 0) != pdPASS) {
        Serial.println("La cola de datos está llena. Muestra perdida.");
      }
    //} else {
     // Serial.println("Pin ALERT no activado. Saltando lectura...");
   // }

    // Esperar para el siguiente ciclo
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}


// Tarea de escritura
void vTaskSD(void *pvParameters) {
  Serial.println("vTaskSD iniciada.");
  (void)pvParameters;
  const size_t BUFFER_SIZE = 512*1;  // Prueba del buffer con 512bytes
  int16_t buffer[BUFFER_SIZE];
  size_t index = 0;

  while (1) {
    // Recibir datos de la cola
    if (xQueueReceive(dataQueue, &buffer[index], portMAX_DELAY) == pdPASS) {


      index++;

      // Si el buffer está lleno, escribir en la tarjeta SD
      if (index >= BUFFER_SIZE) {
        Serial.println("Buffer lleno. Escribiendo en tarjeta SD...");
        // Escribir datos en la tarjeta SD
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
          DateTime now = rtc.now();
          // Formato: "dd/mm/yyyy,hh:mm:ss,valor"
          //dataFile.print(now.day());
          //dataFile.print("/");
          //dataFile.print(now.month());
          //dataFile.print("/");
          //dataFile.print(now.year());
          //dataFile.print(",");
          dataFile.print(now.hour());
          dataFile.print(":");
          dataFile.print(now.minute());
          dataFile.print(":");
          dataFile.print(now.second());
          dataFile.print(";");
          dataFile.println(buffer[i]);    // Valor del ADC
        }
        dataFile.flush();  // Asegurar que los datos se escriben en la tarjeta SD
        Serial.println("Datos escritos en la tarjeta SD.");

        // Reiniciar índice
        index = 0;
      }
    }
  }
}


#endif // TASKS_H
