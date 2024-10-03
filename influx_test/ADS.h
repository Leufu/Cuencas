/*
 *Utilidad del ADS1115 
 * 
 *
 */

#define SDA0_Pin 8   // select ESP32  I2C pins
#define SCL0_Pin 9

//////////////////////////////////ADS1115 config/////////////////
#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48
ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);
//////////////////////////////////////////


float readChannel(ADS1115_MUX channel) {
  adc.setCompareChannels(channel);
  float voltage = adc.getResult_V();  // Leer el voltaje en voltios
  return voltage;
}
int16_t readChannelRaw(ADS1115_MUX channel) {
  adc.setCompareChannels(channel);
  int16_t rawValue = adc.getRawResult(); // Leer el valor bruto del ADC
  return rawValue;
}
