#ifndef ADS_H
#define ADS_H


//#include "ADS.h"
#include <Wire.h>
#include <Arduino.h>

#include <ADS1015_WE.h>

// Dirección I2C de los ADS1015 

#define I2C_ADDRESS_1  0x48 
//#define I2C_ADDRESS_2  0x49



#define INTERRUPT_PIN 4 // Pin conectado al ALERT del ADS1115
//#define INTERRUPT_PIN_2 5 // Pin conectado al ALERT del ADS1115
volatile bool convReady=false;

TaskHandle_t adcTaskHandle = NULL; // Handle para el task que leerá el ADC

// ISR para el pin ALERT/READY
void IRAM_ATTR onDataReadyISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Notifica al task que los datos están listos
  if (adcTaskHandle != NULL) {
    vTaskNotifyGiveFromISR(adcTaskHandle, &xHigherPriorityTaskWoken);
  }

  // Cambia el contexto si una tarea de mayor prioridad fue desbloqueada
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


// Definir los objetos ADC
ADS1015_WE adc_1(I2C_ADDRESS_1);
//ADS1115_WE adc_2(I2C_ADDRESS_2);

// Configuración de pines
//pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Pin ALERT

// Inicializar los ADCs
void initADS() {
  Serial.println("Inicializando ADCs...");

  // Inicializar ADC 1
  if (!adc_1.init()) {
    Serial.println("ADS1015 No 1 not connected!");
    while (1); // Detiene la ejecución si no se detecta el dispositivo
  }
  adc_1.setVoltageRange_mV(ADS1015_RANGE_4096);
  adc_1.setConvRate(ADS1015_3300_SPS);
  //adc_1.setMeasureMode(ADS1015_CONTINUOUS);
	// Configuración para el uso del pin alrt
	//
	pinMode(INTERRUPT_PIN,INPUT_PULLUP);
	adc_1.setCompareChannels(ADS1015_COMP_0_GND);
	adc_1.setAlertPinMode(ADS1015_ASSERT_AFTER_1);
	adc_1.setAlertPinToConversionReady();
	attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), onDataReadyISR, FALLING);
   //adc_1.startSingleMeasurement();
   adc_1.setMeasureMode(ADS1115_CONTINUOUS); 

  // Inicializar ADC 2
  /*
  if (!adc_2.init()) {
    Serial.println("ADS1115 No 2 not connected!");
    while (1); // Detiene la ejecución si no se detecta el dispositivo
  }
  adc_2.setVoltageRange_mV(ADS1015_RANGE_4096);
  adc_2.setConvRate(ADS1015_1600_SPS);
  adc_2.setMeasureMode(ADS1015_CONTINUOUS);
  */
  Serial.println("ADCs inicializados correctamente.");
}

// Leer el valor crudo de un canal específico

int16_t readRawChannel(ADS1015_WE &adc, ADS1015_MUX channel) {
  //adc.setConvRate(ADS1015_3300_SPS);
	adc.setCompareChannels(channel);
  //adc.startSingleMeasurement();
 
  return adc.getRawResult();
}

// Leer el valor crudo de un canal específico
/*
int16_t readRawChannel_11(ADS1115_WE &adc, ADS1115_MUX channel) {
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
 
  return adc.getRawResult();
}
*/

#endif // ADS_H
