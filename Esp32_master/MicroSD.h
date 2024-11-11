#ifndef MICROSD_H
#define MICROSD_H

#include <SD.h>

// Declarar el objeto de archivo de la tarjeta SD
extern File dataFile;

// Función para inicializar la tarjeta SD
void initSD();

#endif // MICROSD_H
