#include <Wire.h>
#include <ADS1115_WE.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

#define I2C_ADDRESS 0x48

// Definiciones para la tarjeta SD
const int chipSelect = 10;  // Pin CS para la tarjeta SD en la ESP32 
const char* archivo = "/test.txt";
File dataFile;

// Crear un objeto para el ADS1115
ADS1115_WE adc(I2C_ADDRESS);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Configuración de pines SPI para la tarjeta SD
  SPI.begin(13, 12, 11, chipSelect);  // SCK=13, MISO=12, MOSI=11, CS=10

  // Inicializar la tarjeta SD
  if (!SD.begin(chipSelect)) {  
    Serial.println("Fallo al montar la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD montada correctamente.");

  // Inicializar el ADS1115
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
  
  // Escribir una cabecera inicial en la tarjeta SD si el archivo no existe
  if (!SD.exists(archivo)) {
    dataFile = SD.open(archivo, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Lectura de datos desde el ADS1115:");
      dataFile.println("ADC Value | Voltage (V)");
      dataFile.close();
      Serial.println("Cabecera escrita correctamente.");
    } else {
      Serial.println("Error al abrir el archivo para escritura.");
    }
  }
}

void loop() {
  // Leer datos desde el ADS1115
  float voltaje = readChannel(ADS1115_COMP_0_GND);  // Leer el canal AIN0
  int16_t rawValue = readChannelRaw(ADS1115_COMP_0_GND);

  // Mostrar los datos en el monitor serie
  Serial.print("ADC0 Voltage: "); 
  Serial.println(voltaje);

  // Guardar los datos en la tarjeta SD en modo append
  dataFile = SD.open(archivo, FILE_APPEND);
  if (dataFile) {
    dataFile.print("ADC0 Voltaje: ");
    dataFile.println(voltaje); dataFile.print("  "); dataFile.print("ADC0: ");dataFile.println(rawValue);
    dataFile.close();
    Serial.println("Datos guardados en la tarjeta SD.");
  } else {
    Serial.println("Error al abrir el archivo para escritura.");
  }

  delay(1000);  // Espera 1 segundo antes de la siguiente lectura
}

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
