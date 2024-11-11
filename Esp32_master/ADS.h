#ifndef ADS_H
#define ADS_H

#include <ADS1115_WE.h>

#define I2C_ADDRESS 0x48

// Declarar el objeto ADC
extern ADS1115_WE adc;

// Función para inicializar el ADS1115
void initADS();

#endif // ADS_H
