/*
 * wifi_ap.h
 * Creación de la red WiFi propia (modo Access Point).
 */
#ifndef WIFI_AP_H
#define WIFI_AP_H

#include "config.h"

// Verifica el módulo WiFi y levanta el punto de acceso.
// Devuelve ERR_NINGUNO si todo salió bien.
CodigoError apIniciar();

#endif
