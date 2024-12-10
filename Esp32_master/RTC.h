#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

RTC_DS3231 rtc;

void initRTC() {
  if (!rtc.begin()) {
    Serial.println("Error: Módulo RTC no encontrado.");
    while (1); // Detener si no se encuentra el RTC
  }
  // Sincronizar con la hora actual si es necesario:
  rtc.adjust(DateTime(__DATE__,__TIME__)); //(2024, 11, 13, 17, 15, 0) // Configura la hora si el RTC no está sincronizado
  Serial.println("RTC inicializado.");
}

#endif 
