/*
 * http_api.cpp
 * Servidor HTTP de una sola petición a la vez. Se encarga de leer, validar,
 * enrutar y responder; la lógica de potencia vive en dimmer.cpp.
 * Códigos de estado: 200 (ok), 400 (petición o parámetro inválido), 404 (ruta desconocida).
 */
#include "http_api.h"
#include "config.h"
#include "dimmer.h"
#include "WiFiS3.h"

static WiFiServer server(HTTP_PORT);

// Escribe la respuesta completa (línea de estado, cabeceras y cuerpo JSON).
static void responder(WiFiClient& cli, int codigo, const char* texto, const String& cuerpo) {
  cli.print("HTTP/1.1 ");
  cli.print(codigo);
  cli.print(" ");
  cli.println(texto);
  cli.println("Content-Type: application/json");
  cli.println("Access-Control-Allow-Origin: *");
  cli.println("Connection: close");
  cli.println();
  cli.println(cuerpo);
}

// De "GET /set?power=50 HTTP/1.1" extrae "/set?power=50". false si no es un GET bien formado.
static bool extraerRuta(const String& req, String& ruta) {
  if (!req.startsWith("GET ")) return false;
  int fin = req.indexOf(' ', 4);
  if (fin < 0) return false;
  ruta = req.substring(4, fin);
  return true;
}

// Lee el entero que sigue a "power=". Acepta de 1 a 3 dígitos y un valor de 0 a 100.
static bool extraerPotencia(const String& ruta, int& valor) {
  int i = ruta.indexOf("power=");
  if (i < 0) return false;
  i += 6;
  int digitos = 0;
  int acum = 0;
  while (i < (int)ruta.length() && isDigit(ruta[i]) && digitos < 3) {
    acum = acum * 10 + (ruta[i] - '0');
    i++;
    digitos++;
  }
  if (digitos == 0) return false;                               // no hay número
  if (i < (int)ruta.length() && isDigit(ruta[i])) return false; // más de 3 dígitos
  if (acum > 100) return false;                                 // fuera de rango
  valor = acum;
  return true;
}

void httpIniciar() {
  server.begin();
}

void httpAtender() {
  WiFiClient cli = server.available();
  if (!cli) return;

  cli.setTimeout(HTTP_TIMEOUT_MS);              // un cliente mudo no bloquea el servidor más de este tiempo
  String req = cli.readStringUntil('\r');       // "GET /set?power=50 HTTP/1.1"
  while (cli.available()) cli.read();           // descarta cabeceras restantes

  String ruta;
  int valor = 0;

  if (req.length() == 0 || req.length() > MAX_REQ_LEN || !extraerRuta(req, ruta)) {
    responder(cli, 400, "Bad Request", "{\"status\":\"error\",\"msg\":\"peticion invalida\"}");
  }
  else if (ruta.startsWith("/set")) {
    if (extraerPotencia(ruta, valor)) {
      dimmerSetPotencia(valor);
      responder(cli, 200, "OK", "{\"status\":\"ok\",\"power\":" + String(dimmerGetPotencia()) + "}");
      LOG_PRINT("Potencia -> ");
      LOG_PRINTLN(dimmerGetPotencia());
    } else {
      responder(cli, 400, "Bad Request", "{\"status\":\"error\",\"msg\":\"power debe ser un entero de 0 a 100\"}");
    }
  }
  else if (ruta.startsWith("/status")) {
    responder(cli, 200, "OK", "{\"power\":" + String(dimmerGetPotencia()) + "}");
  }
  else {
    responder(cli, 404, "Not Found", "{\"status\":\"error\",\"msg\":\"ruta no encontrada\"}");
  }

  delay(1);
  cli.stop();
}
