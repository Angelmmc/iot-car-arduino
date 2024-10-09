const byte enableA = 26; // Pin Enable A
const byte enableB = 25; // Pin Enable B
const byte input1 = 27;  // Pin IN1 (Motor A)
const byte input2 = 14;  // Pin IN2 (Motor A)
const byte input3 = 13;  // Pin IN3 (Motor B)
const byte input4 = 12;  // Pin IN4 (Motor B)
const byte EchoPin = 19;
const byte TriggerPin = 18;

enum Accion{ADELANTE, ATRAS, GIRO_IZQUIERDA, GIRO_DERECHA, GIRO_ESQ_IZQUIERDA, GIRO_ESQ_DERECHA, PARAR, GIRO_CONTRARIO};

const int configuraciones[8][6] = {
    // input1, input2, input3, input4, velocidadA, velocidadB
    {HIGH, LOW, HIGH, LOW, 250, 250}, // ADELANTE
    {LOW, HIGH, LOW, HIGH, 250, 250}, // ATRAS
    {LOW, LOW, HIGH, LOW, 10, 200},   // GIRO_IZQUIERDA
    {HIGH, LOW, LOW, LOW, 200, 10},   // GIRO_DERECHA
    {HIGH, LOW, HIGH, LOW, 50, 255},  // GIRO_ESQ_IZQUIERDA
    {HIGH, LOW, HIGH, LOW, 255, 50},  // GIRO_ESQ_DERECHA
    {LOW, LOW, LOW, LOW, 0, 0},       // PARAR
    {HIGH, LOW, LOW, HIGH, 200, 200}  // GIRO_CONTRARIO (nuevo)
};

// Función genérica para controlar los motores
void controlMotores(Accion accion)
{
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

void setup()
{
    Serial.begin(9600);
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
            controlMotores(GIRO_DERECHA);
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
            controlMotores(PARAR); // Detener el robot si hay un obstáculo
            return; // Salir de la función
        }
        controlMotores(accion); // Continuar la acción si no hay obstáculos
    }

    // Asegúrate de detener el robot al final de la acción
    controlMotores(PARAR);
}

void loop()
{
    accionConDeteccion(ADELANTE, 1000); // Ejecutar ADELANTE por 1 segundo

    accionConDeteccion(PARAR, 1000); // Puedes usar PARAR si deseas detener el robot

    accionConDeteccion(ATRAS, 1000); // Ejecutar ATRAS por 1 segundo

    accionConDeteccion(GIRO_IZQUIERDA, 2000); // Ejecutar GIRO_IZQUIERDA por 2 segundos

    accionConDeteccion(GIRO_DERECHA, 2000); // Ejecutar GIRO_DERECHA por 2 segundos

    accionConDeteccion(GIRO_ESQ_IZQUIERDA, 3000); // Ejecutar GIRO_ESQ_IZQUIERDA por 3 segundos

    accionConDeteccion(GIRO_ESQ_DERECHA, 3000); // Ejecutar GIRO_ESQ_DERECHA por 3 segundos

    accionConDeteccion(PARAR, 1500); // Detener el robot
}