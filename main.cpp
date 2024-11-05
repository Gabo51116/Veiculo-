#include <WiFi.h>

// Definiciones de los pines de los motores
const int adelanteM1 = 19;
const int atrasM1 = 18;
const int adelanteM2 = 17;
const int atrasM2 = 16;

const int veloM1 = 14;
const int veloM2 = 27;
const int five  = 33;  // Pin 33 que debe estar siempre activo
const int ledPin = 2;
bool ledState = LOW;

const int VELOCIDAD_INICIAL = 255;

// Credenciales del punto de acceso
const char* ssid = "ESP32_AP";
const char* password = "123456789";

// URLs del video de la cámara IP
const char* CAMERA_URL1 = "http://192.168.4.2:81/stream";
const char* CAMERA_URL2 = "http://192.168.4.3:81/stream";

WiFiServer server(80);

// Funciones para controlar los motores principales
void adelante() {
  digitalWrite(adelanteM1, HIGH);
  digitalWrite(atrasM1, LOW);
  digitalWrite(adelanteM2, HIGH);
  digitalWrite(atrasM2, LOW);
}

void atras() {
  digitalWrite(adelanteM1, LOW);
  digitalWrite(atrasM1, HIGH);
  digitalWrite(adelanteM2, LOW);
  digitalWrite(atrasM2, HIGH);
}

void izquierda() {
  digitalWrite(adelanteM1, HIGH);
  digitalWrite(atrasM1, LOW);
  digitalWrite(adelanteM2, LOW);
  digitalWrite(atrasM2, HIGH);
}

void derecha() {
  digitalWrite(adelanteM1, LOW);
  digitalWrite(atrasM1, HIGH);
  digitalWrite(adelanteM2, HIGH);
  digitalWrite(atrasM2, LOW);
}

void alto() {
  digitalWrite(adelanteM1, LOW);
  digitalWrite(atrasM1, LOW);
  digitalWrite(adelanteM2, LOW);
  digitalWrite(atrasM2, LOW);
}

void sinco() {
  digitalWrite(five, HIGH);
}

// Función para alternar el estado del LED
void toggleLed() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

void setup() {
  Serial.begin(115200);

  // Configurar los pines de los motores como salida
  pinMode(adelanteM1, OUTPUT);
  pinMode(atrasM1, OUTPUT);
  pinMode(adelanteM2, OUTPUT);
  pinMode(atrasM2, OUTPUT);
  pinMode(veloM1, OUTPUT);
  pinMode(veloM2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(five, OUTPUT);  // Configurar pin 33 como salida

  // Mantener el pin 33 activo
  digitalWrite(five, HIGH);  // Esto mantiene el pin 33 siempre en alto

  // Configurar velocidad inicial de los motores
  analogWrite(veloM1, VELOCIDAD_INICIAL);
  analogWrite(veloM2, VELOCIDAD_INICIAL);

  alto();

  // Configurar el punto de acceso WiFi
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Iniciar el servidor
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print(R"rawliteral(
              <!DOCTYPE HTML><html>
              <head>
                <title>Control de Motores y Cámara</title>
                <meta name="viewport" content="width=device-width, initial-scale=1">
                <style>
                  .video-container {
                    width: 100%;
                    max-width: 640px;
                    margin: 0 auto;
                  }
                  .video-stream {
                    width: 100%;
                    height: auto;
                  }
                </style>
                <script>
                  function sendCommand(command) {
                    var xhttp = new XMLHttpRequest();
                    xhttp.onreadystatechange = function() {
                      if (this.readyState == 4 && this.status == 200) {
                        console.log('Command sent');
                      }
                    };
                    xhttp.open("GET", command, true);
                    xhttp.send();
                  }

                  function loadStream() {
                    var img = document.getElementById('videoStream');
                    img.onerror = function() {
                      img.src = ')rawliteral");
            client.print(CAMERA_URL2);
            client.print(R"rawliteral(';
                    };
                    img.src = ')rawliteral");
            client.print(CAMERA_URL1);
            client.print(R"rawliteral(';
                  }
                </script>
              </head>
              <body onload="loadStream()">
                <h1>Control de Motores y Cámara</h1>
                <div class="video-container">
                  <img id="videoStream" class="video-stream">
                </div>
                <p><button onclick="sendCommand('/adelante')">Adelante</button></p>
                <p><button onclick="sendCommand('/atras')">Atrás</button></p>
                <p><button onclick="sendCommand('/izquierda')">Izquierda</button></p>
                <p><button onclick="sendCommand('/derecha')">Derecha</button></p>
                <p><button onclick="sendCommand('/alto')">Alto</button></p>
                <p><button onclick="sendCommand('/toggleLed')">LED</button></p>
                <p><button onclick="sendCommand('/baseIzquierda')">Base Izquierda</button></p>
                <p><button onclick="sendCommand('/baseDerecha')">Base Derecha</button></p>
                <p><button onclick="sendCommand('/detenerBase')">Detener Base</button></p>
                <p><button onclick="sendCommand('/lanzarIzquierda')">Lanzar Izquierda</button></p>
                <p><button onclick="sendCommand('/lanzarDerecha')">Lanzar Derecha</button></p>
                <p><button onclick="sendCommand('/detenerLanzador')">Detener Lanzador</button></p>
              </body>
              </html>
            )rawliteral");

            client.println();
            break;
          } else {
            if (currentLine.startsWith("GET /adelante")) {
              adelante();
            }
            if (currentLine.startsWith("GET /atras")) {
              atras();
            }
            if (currentLine.startsWith("GET /izquierda")) {
              izquierda();
            }
            if (currentLine.startsWith("GET /derecha")) {
              derecha();
            }
            if (currentLine.startsWith("GET /alto")) {
              alto();
            }
            if (currentLine.startsWith("GET /toggleLed")) {
              toggleLed();
            }

            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}