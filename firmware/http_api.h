/*
 * http_api.h
 * API HTTP mínima para controlar el dimmer desde la app.
 *   GET /set?power=0..100  -> fija la potencia
 *   GET /status            -> devuelve la potencia actual
 */
#ifndef HTTP_API_H
#define HTTP_API_H

// Abre el servidor en el puerto configurado.
void httpIniciar();

// Atiende, si lo hay, un cliente. Se llama desde loop().
void httpAtender();

#endif
