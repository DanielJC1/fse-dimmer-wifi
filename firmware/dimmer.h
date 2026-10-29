/*
 * dimmer.h
 * Interfaz del módulo de control de fase. Oculta las interrupciones, el
 * temporizador y las variables compartidas; el resto del programa solo
 * pide una potencia en porcentaje.
 */
#ifndef DIMMER_H
#define DIMMER_H

// Configura pines, interrupción de cruce por cero y temporizador.
// Devuelve false si algún paso falla (el llamador decide qué hacer).
bool dimmerIniciar();

// Fija la potencia de 0 a 100 %. Valores fuera de rango se recortan.
void dimmerSetPotencia(int pct);

// Última potencia solicitada (0 a 100 %).
int dimmerGetPotencia();

// Deja la carga apagada (estado seguro ante una falla).
void dimmerApagar();

#endif
