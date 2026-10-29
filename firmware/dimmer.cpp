/*
 * dimmer.cpp
 * Control de fase de un TRIAC.
 *
 * Dos rutinas de interrupción trabajan juntas, sin intervención del loop():
 *  - onZeroCross: se dispara en cada cruce por cero (120 Hz) y reinicia la cuenta.
 *  - onTick     : se dispara cada 50 us (20 kHz), cuenta ticks y dispara el gate
 *                 cuando la cuenta alcanza 'umbral'.
 * El único dato que cruza de la tarea de fondo (loop) a las interrupciones es
 * 'umbral'. Todo el brillo es, en esencia, el valor de ese entero.
 */
#include "dimmer.h"
#include "config.h"
#include "FspTimer.h"

// 'volatile': estas variables las modifica una interrupción mientras otra
// rutina las lee; el compilador no debe guardarlas en registros ni optimizarlas.
static volatile int  contador  = 0;             // ticks transcurridos en el semiciclo actual
static volatile int  umbral    = TICKS + 10;    // tick en el que se dispara (TICKS+10 = nunca = apagado)
static volatile bool disparado = false;         // ya se disparó en este semiciclo
static int potenciaActual = 0;                  // solo la usa el loop(), no requiere volatile

static FspTimer timer;

// ISR del cruce por cero: marca el inicio de un semiciclo.
static void onZeroCross() {
  contador  = 0;
  disparado = false;
  digitalWrite(PIN_GATE, LOW);
}

// ISR del temporizador (cada TICK_US): corazón del control de fase.
static void onTick(timer_callback_args_t *arg) {
  contador++;
  if (!disparado && contador >= umbral) {
    digitalWrite(PIN_GATE, HIGH);   // dispara; el TRIAC se engancha y conduce solo
    disparado = true;
  }
  // Se suelta el gate un poco antes del siguiente cruce para no volver a
  // disparar al inicio del semiciclo nuevo.
  if (disparado && contador >= TICKS - MARGEN) {
    digitalWrite(PIN_GATE, LOW);
  }
}

bool dimmerIniciar() {
  pinMode(PIN_GATE, OUTPUT);
  digitalWrite(PIN_GATE, LOW);
  pinMode(PIN_ZC, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ZC), onZeroCross, RISING);

  uint8_t tipo = GPT_TIMER;
  int8_t idx = FspTimer::get_available_timer(tipo);
  if (idx < 0) idx = FspTimer::get_available_timer(tipo, true);
  if (idx < 0) return false;                                   // no hay temporizador libre

  if (!timer.begin(TIMER_MODE_PERIODIC, tipo, idx, TIMER_FREQ_HZ, 0.0f, onTick)) return false;
  if (!timer.setup_overflow_irq()) return false;
  if (!timer.open())  return false;
  if (!timer.start()) return false;
  return true;
}

void dimmerSetPotencia(int pct) {
  pct = constrain(pct, 0, 100);
  potenciaActual = pct;
  if (pct == 0) { umbral = TICKS + 10; return; }               // 0 %: nunca dispara
  // Remapea 1..100 % de la app al rango útil del foco (MIN_UTIL..MAX_UTIL).
  int real = map(pct, 1, 100, MIN_UTIL, MAX_UTIL);
  if (real >= 100) umbral = 0;
  else             umbral = (100 - real) * TICKS / 100;        // más potencia = disparo más temprano
}

int dimmerGetPotencia() {
  return potenciaActual;
}

void dimmerApagar() {
  dimmerSetPotencia(0);
  digitalWrite(PIN_GATE, LOW);
}
