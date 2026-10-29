/*
 * config.h
 * Constantes de todo el sistema: pines, temporización, calibración del foco,
 * parámetros de red y códigos de error. Es el único archivo que se toca para
 * adaptar el proyecto a otro foco, otra red o otros pines.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------- Depuración por Serial ----------
// 1 = imprime mensajes al Monitor Serie (desarrollo).
// 0 = sin mensajes (operación final con powerbank, no hay quien los lea).
#define DEBUG_SERIAL 1
#if DEBUG_SERIAL
  #define LOG_PRINT(x)   Serial.print(x)
  #define LOG_PRINTLN(x) Serial.println(x)
#else
  #define LOG_PRINT(x)
  #define LOG_PRINTLN(x)
#endif

// ---------- Pines (módulo dimmer) ----------
constexpr uint8_t PIN_ZC   = 2;   // PZC: pulso de cruce por cero (entrada, interrupción)
constexpr uint8_t PIN_GATE = 3;   // PD : disparo del TRIAC (salida)

// ---------- Temporización ----------
constexpr uint32_t SEMICICLO_US = 8333;                       // 1 / (2 * 60 Hz)
constexpr uint32_t TICK_US      = 50;                         // resolución del temporizador
constexpr float    TIMER_FREQ_HZ = 1000000.0f / TICK_US;      // 20 kHz
constexpr int      TICKS        = SEMICICLO_US / TICK_US;     // 166 ticks por semiciclo
constexpr int      MARGEN       = 4;                          // suelta el gate 4 ticks (200 us) antes del cruce

// ---------- Calibración del foco (observada experimentalmente) ----------
constexpr int MIN_UTIL = 11;   // % a partir del cual el filamento empieza a emitir luz
constexpr int MAX_UTIL = 90;   // % a partir del cual el brillo ya no aumenta

// ---------- Red (modo Access Point) ----------
constexpr const char* AP_SSID = "DimmerEXTRA";
constexpr const char* AP_PASS = "dimmerEXTRA";   // WPA2: mínimo 8 caracteres

// ---------- Servidor HTTP ----------
constexpr uint16_t      HTTP_PORT       = 80;
constexpr unsigned long HTTP_TIMEOUT_MS = 500;   // espera máxima de la línea de petición
constexpr size_t        MAX_REQ_LEN     = 128;   // longitud máxima aceptada de la línea de petición

// ---------- Códigos de error (número de parpadeos del LED integrado) ----------
enum CodigoError : uint8_t {
  ERR_NINGUNO     = 0,
  ERR_TIMER       = 1,   // no se pudo iniciar el temporizador
  ERR_WIFI_MODULO = 2,   // módulo WiFi no detectado
  ERR_AP          = 3    // no se pudo crear el punto de acceso
};

#endif
