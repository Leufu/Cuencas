#include "ads.h"

// InfluxDB datos
#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID y contraseña
#define WIFI_SSID "Wifi_14000"
#define WIFI_PASSWORD "wifi14000"

// InfluxDB configuración
#define INFLUXDB_URL "http://146.83.216.177:8086"
#define INFLUXDB_TOKEN "0u_MrR7F1lRIK44g-LL44zifmUloCe9jkUtnl-9ylirqZZcC-y3jxkrrEITSvs7xRBrU02DuOvTOsNNJPsW0jQ=="
#define INFLUXDB_ORG "ed6884a7fc5b9d6e"
#define INFLUXDB_BUCKET "cuencas"

// Información de zona horaria
#define TZ_INFO "UTC-4"

// Declarar cliente de InfluxDB
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declarar punto de datos
Point sensor("sensor_data");

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA0_Pin, SCL0_Pin); // Inicializar I2C

  // Configurar conexión WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando a WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Inicializar el ADS1115
  if (!adc.init()) {
    Serial.println("Fallo al inicializar el ADS1115.");
    while (1);
  }
  Serial.println("ADS1115 inicializado correctamente.");
  
  adc.setVoltageRange_mV(ADS1115_RANGE_6144);  // Rango de voltaje +/- 6.144V
  adc.setConvRate(ADS1115_860_SPS);            // 860 muestras por segundo
  adc.setMeasureMode(ADS1115_CONTINUOUS);      // Modo continuo

  // Sincronizar tiempo para InfluxDB
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Verificar conexión a InfluxDB
  if (client.validateConnection()) {
    Serial.print("Conectado a InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("Error de conexión a InfluxDB: ");
    Serial.println(client.getLastErrorMessage());
  }

  // Añadir etiquetas al punto de datos
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
}

void loop() {
  //  valor del canal 0
  int16_t rawValue = readChannelRaw(ADS1115_COMP_0_GND);

  Serial.print("Valor raw del ADC: ");
  Serial.println(rawValue);

  influx(rawValue);

  delay(1000); //???
}

void influx(int16_t rawValue) {
  // Limpiar campos del sensor para reutilizar el punto
  sensor.clearFields();

  // Agregar el valor crudo al campo
  sensor.addField("raw_adc", rawValue);

  // RSSI de la red WiFi conectada
  sensor.addField("rssi", WiFi.RSSI());

  // Imprimir en consola el dato a escribir
  Serial.print("Escribiendo: ");
  Serial.println(sensor.toLineProtocol());

  // Verificar conexión WiFi
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida");
    return;
  }

  // Escribir el punto en InfluxDB
  if (!client.writePoint(sensor)) {
    Serial.print("Error al escribir en InfluxDB: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Datos enviados a InfluxDB");
}
