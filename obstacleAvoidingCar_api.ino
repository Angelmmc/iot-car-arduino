#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const byte enableA = 26; // Pin Enable A
const byte enableB = 25; // Pin Enable B
const byte input1 = 27;  // Pin IN1 (Motor A)
const byte input2 = 14;  // Pin IN2 (Motor A)
const byte input3 = 13;  // Pin IN3 (Motor B)
const byte input4 = 12;  // Pin IN4 (Motor B)
const byte EchoPin = 19;
const byte TriggerPin = 18;
const byte ledTrasero = 34;
const byte ledIzquierda = 22;
const byte ledDerecha = 15;
const byte ledDelantero = 4;

const char* ssid = "ChillyWilly";      // Reemplaza con el nombre de tu red Wi-Fi
const char* password = "hola12345";

enum Accion{ADELANTE, ATRAS, GIRO_IZQUIERDA, GIRO_DERECHA, GIRO_90_IZQUIERDA, GIRO_90_DERECHA, PARAR, GIRO_360_DERECHA, GIRO_360_IZQUIERDA};

const int configuraciones[9][6] = {
    // input1, input2, input3, input4, velocidadA, velocidadB
    {HIGH, LOW, HIGH, LOW, 250, 250}, // ADELANTE
    {LOW, HIGH, LOW, HIGH, 250, 250}, // ATRAS
    {HIGH, LOW, HIGH, LOW, 10, 200},  // GIRO_IZQUIERDA
    {HIGH, LOW, HIGH, LOW, 200, 10},  // GIRO_DERECHA
    {LOW, HIGH, HIGH, LOW, 50, 255},  // GIRO_90_IZQUIERDA
    {HIGH, LOW, LOW, HIGH, 255, 50},  // GIRO_90_DERECHA
    {LOW, LOW, LOW, LOW, 0, 0},       // PARAR
    {HIGH, LOW, LOW, HIGH, 150, 150},  // GIRO_360_DERECHA 
    {LOW, HIGH, HIGH, LOW, 150, 150}  // GIRO_360_IZQUIERDA
};

// Función genérica para controlar los motores
void controlMotores(Accion accion){
    digitalWrite(input1, configuraciones[accion][0]);
    digitalWrite(input2, configuraciones[accion][1]);
    digitalWrite(input3, configuraciones[accion][2]);
    digitalWrite(input4, configuraciones[accion][3]);
    analogWrite(enableA, configuraciones[accion][4]);
    analogWrite(enableB, configuraciones[accion][5]);
    }

int ping(int TriggerPin, int EchoPin) {
unsigned short int duration;
float distanceCm;

digitalWrite(TriggerPin, LOW); //para generar un pulso limpio ponemos a LOW 4us
delayMicroseconds(4);
digitalWrite(TriggerPin, HIGH); //generamos Trigger (disparo) de 10us
delayMicroseconds(10);
digitalWrite(TriggerPin, LOW);

duration = pulseIn(EchoPin, HIGH); //medimos el tiempo entre pulsos, en microsegundos

distanceCm = float(duration)/58.2; //convertimos a distancia, en cm
return distanceCm;
}

void accionConDeteccion(Accion accion, unsigned long tiempo)
{
    // Comprobar si hay un obstáculo a menos de 30 cm
    float cm = ping(TriggerPin, EchoPin);
    if (cm < 30.0)
    {
        Serial.println("Obstáculo detectado. Girando a la derecha...");
        while (cm < 30.0)
        {
            // Continuar girando a la derecha mientras haya un obstáculo
            controlMotores(GIRO_360_DERECHA);
            delay(500); // Ajustar este delay según sea necesario para mantener el giro suave
            cm = ping(TriggerPin, EchoPin);
        }
    }

    // Ejecutar la acción deseada por el tiempo especificado
    Serial.println("Camino despejado. Ejecutando la acción...");
    unsigned long inicio = millis(); // Guardar el tiempo de inicio

    while (millis() - inicio < tiempo) // Ejecutar la acción durante el tiempo especificado
    {
        // Verificar la distancia mientras se ejecuta la acción
        cm = ping(TriggerPin, EchoPin);
        if (cm < 30.0)
        {
            Serial.println("Obstáculo detectado. Deteniendo...");
            controlMotores(GIRO_360_DERECHA); // Detener el robot si hay un obstáculo
            return; // Salir de la función
        }
        controlMotores(accion); // Continuar la acción si no hay obstáculos
    }

}

void setup()
{
    Serial.begin(115200);
    // Configurar pines como salida
    pinMode(enableA, OUTPUT);
    pinMode(enableB, OUTPUT);
    pinMode(input1, OUTPUT);
    pinMode(input2, OUTPUT);
    pinMode(input3, OUTPUT);
    pinMode(input4, OUTPUT);

    // Inicializar los motores apagados
    digitalWrite(enableA, LOW);
    digitalWrite(enableB, LOW);
    digitalWrite(input1, LOW);
    digitalWrite(input2, LOW);
    digitalWrite(input3, LOW);
    digitalWrite(input4, LOW);

    // Inicializar los pines de HC-SR04
    pinMode(TriggerPin, OUTPUT);
    pinMode(EchoPin, INPUT);

    pinMode(ledDelantero, OUTPUT);
    pinMode(ledDerecha, OUTPUT);
    pinMode(ledIzquierda, OUTPUT);
    pinMode(ledTrasero, OUTPUT);

    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

    Serial.println("");
    Serial.println("Conectado a Wi-Fi");
  }
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL de la API que devuelve un JSON
    String url = "http://18.212.146.225:5000//iot/last";  // Reemplaza con la URL de tu API

    http.begin(url);
    int httpResponseCode = http.GET();  // Hacer la solicitud GET

    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtener la respuesta como string
      Serial.println("Respuesta del servidor: ");
      Serial.println(response);

      // Analizar el JSON
      StaticJsonDocument<512> doc;  // Crear el documento JSON

      DeserializationError error = deserializeJson(doc, response);  // Parsear el JSON
      if (error) {
        Serial.print("Error al analizar JSON: ");
        Serial.println(error.c_str());
        return;
      }

      int status = doc["status"];          

      Serial.print("Status: ");
      Serial.println(status);

      // Emparejar el status con la acción correspondiente
      switch (status) {
        case 1:
          accionConDeteccion(GIRO_90_IZQUIERDA, 1000); // Ejecutar ADELANTE por 1 segundo
          break;
        case 2:
          accionConDeteccion(ADELANTE, 1000); // Detener el robot
          break;
        case 3:
          accionConDeteccion(GIRO_90_DERECHA, 1000); // Ejecutar ATRAS por 1 segundo
          break;
        case 4:
          accionConDeteccion(GIRO_IZQUIERDA, 1000); // Ejecutar GIRO_IZQUIERDA por 2 segundos
          break;
        case 5:
          accionConDeteccion(PARAR, 1000); // Ejecutar GIRO_DERECHA por 2 segundos
          break;
        case 6:
          accionConDeteccion(GIRO_DERECHA, 1000); // Ejecutar GIRO_ESQ_IZQUIERDA por 3 segundos
          break;
        case 7:
          accionConDeteccion(GIRO_360_IZQUIERDA, 1000); // Ejecutar GIRO_ESQ_DERECHA por 3 segundos
          break;
        case 8:
          accionConDeteccion(ATRAS, 1000); // Ejecutar GIRO_ESQ_DERECHA por 3 segundos
          break;
        case 9:
          accionConDeteccion(GIRO_360_DERECHA, 1000); // Ejecutar GIRO_ESQ_DERECHA por 3 segundos
          break;

        default:
          Serial.println("Status desconocido");
          accionConDeteccion(PARAR, 1500); // Detener el robot por defecto
          break;
      }

    } else {
      Serial.print("Error en la solicitud GET, Código: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Cerrar la conexión
  } else {
    Serial.println("Error al conectar a Wi-Fi");
  }
}