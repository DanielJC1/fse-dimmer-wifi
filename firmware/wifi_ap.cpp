/*
 * wifi_ap.cpp
 * El Arduino crea su propia red (SSID y contraseña en config.h) y queda con
 * la IP fija 192.168.4.1. No depende de ningún router externo.
 */
#include "wifi_ap.h"
#include "WiFiS3.h"

CodigoError apIniciar() {
  if (WiFi.status() == WL_NO_MODULE) {
    LOG_PRINTLN("Modulo WiFi no detectado");
    return ERR_WIFI_MODULO;
  }
  if (WiFi.beginAP(AP_SSID, AP_PASS) != WL_AP_LISTENING) {
    LOG_PRINTLN("No se pudo crear el AP");
    return ERR_AP;
  }
  delay(1000);   // la red tarda un instante en estabilizarse
  LOG_PRINT("Red lista: '");
  LOG_PRINT(AP_SSID);
  LOG_PRINT("'  ->  http://");
  LOG_PRINTLN(WiFi.localIP());   // 192.168.4.1
  return ERR_NINGUNO;
}
