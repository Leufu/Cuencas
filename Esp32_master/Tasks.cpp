#include "Tasks.h"
#include "ADS.h"
#include "MicroSD.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define QUEUE_LENGTH 1024 * 2
#define SAMPLE_RATE_HZ 860

// Manejo global de la cola
QueueHandle_t dataQueue;

// Prototipos de funciones para tareas
void vTaskADS(void *pvParameters);
void vTaskSD(void *pvParameters);

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
    Serial.print(tiempoSegundos, 6);  // Imprime con precisión de 6 decimales
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
  xTaskCreatePinnedToCore(vTaskADS, "TaskADS", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(vTaskSD, "TaskSD", 4096, NULL, 1, NULL, 1);
  Serial.println("Tareas creadas.");
}

// Tarea de lectura
void vTaskADS(void *pvParameters) {
  Serial.println("vTaskADS iniciada.");
  (void)pvParameters;
  const TickType_t xFrequency = pdMS_TO_TICKS(1);  // Aproximadamente 1ms de delay
  TickType_t xLastWakeTime = xTaskGetTickCount();
  unsigned long lastTime = micros();  // Para medir el tiempo entre lecturas

  while (1) {
    // Leer el valor del ADC
    int16_t adcValue = adc.getRawResult();


    calcularTiempoCada860Muestras();

    // Enviar el valor a la cola
    if (xQueueSend(dataQueue, &adcValue, 0) != pdPASS) {
      // La cola está llena, se perderán datos
      Serial.println("La cola de datos está llena. Muestra perdida.");
    }

    // Esperar para el siguiente ciclo
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}


// Tarea de escritura
void vTaskSD(void *pvParameters) {
  Serial.println("vTaskSD iniciada.");
  (void)pvParameters;
  const size_t BUFFER_SIZE = 512;  // Ajustar el tamaño del buffer según sea necesario
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
          dataFile.println(buffer[i]);
        }
        dataFile.flush();  // Asegurar que los datos se escriben en la tarjeta SD
        Serial.println("Datos escritos en la tarjeta SD.");

        // Reiniciar índice
        index = 0;
      }
    }
  }
}
