/*
 * Control inalámbrico de una lámpara incandescente (control de fase con TRIAC)
 * Arduino UNO R4 WiFi - Fundamentos de Sistemas Embebidos, extraordinario 2026 
 *
 * Arquitectura:
 *   dimmer.*    control de fase (interrupciones + temporizador)
 *   wifi_ap.*   red propia en modo Access Point
 *   http_api.*  API HTTP /set y /status
 *   config.h    constantes y códigos de error
 */
#include "config.h"
#include "dimmer.h"
#include "wifi_ap.h"
#include "http_api.h"

// Estado seguro + señalización: apaga la carga y parpadea el LED integrado
// tantas veces como indique el código de error, en bucle. Sirve en operación
// autónoma, cuando no hay Monitor Serie.
[[noreturn]] static void fallaFatal(CodigoError e) {
  dimmerApagar();
  LOG_PRINT("ERROR fatal, codigo ");
  LOG_PRINTLN((int)e);
  pinMode(LED_BUILTIN, OUTPUT);
  while (true) {
    for (uint8_t i = 0; i < (uint8_t)e; i++) {
      digitalWrite(LED_BUILTIN, HIGH); delay(200);
      digitalWrite(LED_BUILTIN, LOW);  delay(200);
    }
    delay(1500);
  }
}

void setup() {
#if DEBUG_SERIAL
  Serial.begin(115200);
#endif

  if (!dimmerIniciar()) fallaFatal(ERR_TIMER);

  CodigoError e = apIniciar();
  if (e != ERR_NINGUNO) fallaFatal(e);

  httpIniciar();
}

void loop() {
  httpAtender();
}
