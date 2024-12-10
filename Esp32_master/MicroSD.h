#ifndef MICROSD_H
#define MICROSD_H

///#include "MicroSD.h"
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include "RTC.h"
#include <stdio.h>
#define SD_CS_PIN 10 //Pin CS para la SD en la esp32

File dataFile;
// Declarar el objeto de archivo de la tarjeta SD
extern File dataFile;

void create_header()
{
	 
	 // Este pedazo de codigo es para crear una var llamada nombre
	 // en esta se guarda el valor actual para crear distintos csv cada vez
	 // que se use el dispositivo.
	 //
	Serial.println("ENtra en lafuncion ");
	DateTime tiempo_actual_rtc = rtc.now();
   char nombre[35]; 
	
	Serial.println("Se crea nombre");

   sprintf(nombre, "/%04d-%02d-%02d_%02d-hh-%02d-mm.csv", 
            tiempo_actual_rtc.year(), 
            tiempo_actual_rtc.month(), 
            tiempo_actual_rtc.day(),
            tiempo_actual_rtc.hour(), 
            tiempo_actual_rtc.minute());

   Serial.print("Nombre del archivo: ");
   Serial.println(nombre);
	 
	 //////////////////////////////////////////////////////////////////////

	 Serial.println("Tarjeta SD inicializada.");
	 
	 dataFile = SD.open(nombre, FILE_WRITE);
	 if (!dataFile) {
			 Serial.println("Error al abrir el archivo en la tarjeta SD.");
			 while (1);
	 }
	 Serial.println("Archivo de datos abierto.");

	 dataFile.println("////////Digitalizador de cuencas////////");
	 dataFile.println("/////////////////////by/////////////////");
	 dataFile.println("////////////////LeufuLab////////////////");
	 dataFile.println("////////////////////////////////////////");
	 dataFile.println("/////////Sample rate= ***///////////////");
	 dataFile.println("////////Fecha: /////////////////////////");
	 dataFile.println(nombre);
	 dataFile.println("////////////////////////////////////////");
	 dataFile.println("////////////////////////////////////////");
	 // Opcionalmente escribir encabezado
	 dataFile.println("Datos; Datos1; Datos2"); // Cambiado a coma como delimitador
	 dataFile.flush();

	 
}



//Ahora se mantiene abierta la tajeta SD
void initSD() {
  Serial.println("Inicializando tarjeta SD...");
  SPI.begin(13, 12, 11, SD_CS_PIN); // SCK=13, MISO=12, MOSI=11, CS=10
  if (!SD.begin(SD_CS_PIN)) {
	  Serial.println("Error al inicializar la tarjeta SD.");
	  while (1);
  }

  create_header();
}










#endif // MICROSD_H
