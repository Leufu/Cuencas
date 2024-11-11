#include "ADS.h"
#include <Wire.h>
#include <Arduino.h>

// Definir el objeto ADC
ADS1115_WE adc(I2C_ADDRESS);

void initADS() {
  Serial.println("Inicializando ADS1115...");
  if (!adc.init()) {
    Serial.println("Error al inicializar ADS1115.");
    while (1);
  }
  Serial.println("ADS1115 inicializado.");

  adc.setVoltageRange_mV(ADS1115_RANGE_6144);    // Rango de voltaje +/- 6.144V
  adc.setCompareChannels(ADS1115_COMP_0_GND);    // Leer del canal 0
  adc.setConvRate(ADS1115_860_SPS);              // Configurar tasa de conversión a 860 SPS
  adc.setMeasureMode(ADS1115_CONTINUOUS);        // Modo continuo
}
