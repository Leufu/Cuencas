
//ADS1115
#include "ADS.h"

//Influx  datos //
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
  
  // WiFi AP SSID
  #define WIFI_SSID "Wifi_14000"
  // WiFi password
  #define WIFI_PASSWORD "wifi14000"
  
  #define INFLUXDB_URL "http://146.83.216.177:8086"
  #define INFLUXDB_TOKEN "0u_MrR7F1lRIK44g-LL44zifmUloCe9jkUtnl-9ylirqZZcC-y3jxkrrEITSvs7xRBrU02DuOvTOsNNJPsW0jQ=="
  #define INFLUXDB_ORG "ed6884a7fc5b9d6e"
  #define INFLUXDB_BUCKET "cuencas"
  
  // Time zone info
  #define TZ_INFO "UTC-4"
  
  // Declare InfluxDB client instance with preconfigured InfluxCloud certificate
  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
  
  // Declare Data point
  Point sensor("wifi_status");
///////////////////////////////////////////////////////////////////////////////////////////////7
void setup() {
    Serial.begin(115200);
    while(!Serial);
    Wire.begin(SDA0_Pin,SCL0_Pin);
    
    // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  
    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
    }
    Serial.println();

    //////////////////////// Inicializar el ADS1115//////////////////////////
    
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
    
    /////////////////////////////////////////////////////////////////////////////////
    
    // Accurate time is necessary for certificate validation and writing in batches
    // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
    // Syncing progress and the time will be printed to Serial.
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  
    // Check server connection
    if (client.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }
    // Add tags to the data point
    sensor.addTag("device", DEVICE);
    sensor.addTag("SSID", WiFi.SSID());
    
  }
void loop()
{
  int16_t rawValue = readChannelRaw(ADS1115_COMP_0_GND);

  // Mostrar los datos en el monitor serie
  //Serial.print("ADC0 Voltage: "); 
  Serial.println(rawValue);
    

}


void influx()
{
 // Clear fields for reusing the point. Tags will remain the same as set above.
  sensor.clearFields();

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Waiting 1 second");
  delay(1000); 

}
