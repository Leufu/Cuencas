#ifndef ADS_H
#define ADS_H


//#include "ADS.h"
#include <Wire.h>
#include <Arduino.h>

#include <ADS1015_WE.h>

// Dirección I2C de los ADS1015 

#define I2C_ADDRESS_1  0x48 
#define I2C_ADDRESS_2  0x49

#define INTERRUPT_PIN 18 // Pin conectado al ALERT del ADS1115

// Definir los objetos ADC
ADS1015_WE adc_1(I2C_ADDRESS_1);
ADS1015_WE adc_2(I2C_ADDRESS_2);

// Configuración de pines
//pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Pin ALERT

// Inicializar los ADCs
void initADS() {
  Serial.println("Inicializando ADCs...");

  // Inicializar ADC 1
  if (!adc_1.init()) {
    Serial.println("ADS1115 No 1 not connected!");
    while (1); // Detiene la ejecución si no se detecta el dispositivo
  }
  adc_1.setVoltageRange_mV(ADS1015_RANGE_4096);
  adc_1.setConvRate(ADS1015_1600_SPS);
  adc_1.setMeasureMode(ADS1015_CONTINUOUS);

  // Inicializar ADC 2
  if (!adc_2.init()) {
    Serial.println("ADS1115 No 2 not connected!");
    while (1); // Detiene la ejecución si no se detecta el dispositivo
  }
  adc_2.setVoltageRange_mV(ADS1015_RANGE_4096);
  adc_2.setConvRate(ADS1015_920_SPS);
  adc_2.setMeasureMode(ADS1015_CONTINUOUS);

  Serial.println("ADCs inicializados correctamente.");
}

// Leer el valor crudo de un canal específico
int16_t readRawChannel(ADS1015_WE &adc, ADS1015_MUX channel) {
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
 
  return adc.getRawResult();
}

#endif // ADS_H
