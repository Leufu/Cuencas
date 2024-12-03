#ifndef ADS_H
#define ADS_H


//#include "ADS.h"
#include <Wire.h>
#include <Arduino.h>

#include <ADS1115_WE.h>

#define I2C_ADDRESS 0x48
#define INTERRUPT_PIN 18 // Pin conectado al ALERT del ADS1115

// Definir el objeto ADC
ADS1115_WE adc(I2C_ADDRESS);

// Configuración de pines
//pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Pin ALERT

void initADS() {
  Serial.println("Inicializando ADS1115...");
  if (!adc.init()) {
    Serial.println("Error al inicializar ADS1115.");
    while (1);
  }
  Serial.println("ADS1115 inicializado.");
  // Configuración de pines
  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Pin ALERT
  adc.setVoltageRange_mV(ADS1115_RANGE_6144);    // Rango de voltaje +/- 6.144V
  adc.setCompareChannels(ADS1115_COMP_0_GND);    // Leer del canal 0
  adc.setConvRate(ADS1115_860_SPS  );              // Configurar tasa de conversión a 860 SPS
  adc.setMeasureMode(ADS1115_CONTINUOUS);        // Modo continuo
  adc.setAlertModeAndLimit_V(ADS1115_WINDOW, 8.0, 2.0); // Configurar el modo de la salida ALERT como ventana
  adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1); // Configurar el pin ALERT para que active inmediatamente


}

#endif // ADS_H
