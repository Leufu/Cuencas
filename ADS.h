/*
 *Utilidad del ADS1115 
 * 
 *
 */
#ifndef ADS_H
#define ADS_H

//#include <Arduino.h>
#include <Wire.h>
#include <ADS1115_WE.h>

#define I2C_ADDRESS 0x48
ADS1115_WE adc(I2C_ADDRESS);

void initADS() {
  if (!adc.init()) {
    Serial.println("Fallo al inicializar el ADS1115.");
    while (1);
  }
  Serial.println("ADS1115 inicializado correctamente.");
  
  // Configuración del ADS1115
  adc.setVoltageRange_mV(ADS1115_RANGE_6144);  // Rango de voltaje +/- 6.144V
  adc.setCompareChannels(ADS1115_COMP_0_GND);  // Leer desde el canal 0 con respecto a GND
  adc.setConvRate(ADS1115_860_SPS);            // 860 muestras por segundo
  adc.setMeasureMode(ADS1115_CONTINUOUS);      // Modo continuo
}

int16_t readChannelRaw(ADS1115_MUX channel) {
  adc.setCompareChannels(channel);
  return adc.getRawResult(); // Leer el valor bruto del ADC
}

#endif
