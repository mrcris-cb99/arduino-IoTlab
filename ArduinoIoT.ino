// Librerías necesarias para el funcionamiento del sistema
#include <Arduino.h>           // Funciones básicas de Arduino
#include <Wire.h>              // Comunicación I2C
#include <DHT.h>                // Lectura del sensor de temperatura y humedad
#include <Adafruit_GFX.h>       // Funciones gráficas para la pantalla
#include <Adafruit_SSD1306.h>   // Control de la pantalla OLED
#include "thingProperties.h"    // Propiedades para Arduino IoT Cloud
#include "arduino_secrets.h"    // Credenciales de conexión

// Pines donde están conectados los sensores y actuadores
#define PIR 3          // Sensor de movimiento
#define LDR A0         // Sensor de luz
#define DHTPIN 4       // Pin del sensor DHT22
#define DHTTYPE DHT22  // Tipo de sensor de temperatura y humedad
#define MQ135 A1       // Sensor de calidad del aire
#define RED_LED 13     // LED indicador de estado crítico
#define YELLOW_LED 12  // LED indicador de advertencia
#define BLUE_LED 8     // LED indicador de estado normal
#define BUZZER 2       // Zumbador para alertas sonoras

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128    // Ancho de la pantalla en píxeles
#define SCREEN_HEIGHT 64    // Alto de la pantalla en píxeles
#define OLED_RESET -1       // No se utiliza un pin de reinicio externo
#define SCREEN_ADDRESS 0x3C // Dirección I2C de la pantalla

// Creación del objeto para controlar la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables para almacenar las lecturas de los sensores
bool presence;                       // Indica si se detecta movimiento
int valueLDR, normalizedValueLDR;    // Lectura original y porcentaje de luz
float temperature, humidity;         // Temperatura y humedad
int airQuality;                      // Lectura del sensor de aire
String overallStatus;                // Estado general del sistema

// Estados posibles del sistema
String systemStatus[] = {"CRITICO", "ADVERTENCIA", "NORMAL"};

// Creación del objeto para controlar el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);


// Configuración inicial del sistema
void setup() {

  // Configura los LED como salidas
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Configura el sensor de movimiento como entrada
  pinMode(PIR, INPUT);

  // Configura el buzzer como salida
  pinMode(BUZZER, OUTPUT);

  // Inicia la comunicación serial a 9600 baudios
  Serial.begin(9600);

  // Inicializa el sensor de temperatura y humedad
  dht.begin();

  // Espera un segundo para permitir la inicialización
  delay(1000);

  // Inicializa la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Pantalla no encontrada");
  }

  // Muestra un mensaje inicial en la pantalla
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Iniciando..."));
  display.display();

  // Inicializa las propiedades de Arduino IoT Cloud
  initProperties();

  // Inicia la conexión con Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Configura el nivel de mensajes de depuración
  setDebugMessageLevel(2);

  // Muestra información de depuración de la conexión
  ArduinoCloud.printDebugInfo();
}


// Bucle principal: se ejecuta continuamente
void loop() {

  // Actualiza la comunicación con Arduino IoT Cloud
  ArduinoCloud.update();

  // Lee los sensores y actualiza sus variables
  readPIR();
  readLDR();
  readDHT();
  readMQ();

  // Determina el estado general y controla los actuadores
  checkStatus();

  // Actualiza la información mostrada en la pantalla OLED
  updateOLED();
}


// Lee el sensor de movimiento PIR
void readPIR() {

  // Guarda el estado del sensor: HIGH o LOW
  presence = digitalRead(PIR);

  // Muestra el resultado en el monitor serial
  Serial.print(F("Movimiento: "));
  Serial.println(presence ? F("DETECTADO") : F("CLEAR"));
}


// Lee el sensor de luz LDR
void readLDR() {

  // Obtiene la lectura analógica del sensor
  valueLDR = analogRead(LDR);

  // Convierte la lectura a una escala de 0 a 100
  normalizedValueLDR = map(valueLDR, 0, 1023, 0, 100);

  // Muestra el porcentaje calculado
  Serial.print(F("Luz: "));
  Serial.print(normalizedValueLDR);
  Serial.println(F("%"));
}


// Lee la temperatura y la humedad del sensor DHT22
void readDHT() {

  // Obtiene la temperatura en grados Celsius
  temperature = dht.readTemperature();

  // Obtiene la humedad relativa en porcentaje
  humidity = dht.readHumidity();

  // Comprueba si alguna lectura ha fallado
  if (isnan(temperature) || isnan(humidity)) {

    // Informa del error en el monitor serial
    Serial.println(F("Error al leer el sensor DHT22."));

    // Asigna valores predeterminados ante el error
    temperature = 0.0;
    humidity = 0.0;
  }

  // Muestra la temperatura
  Serial.print(F("Temperatura: "));
  Serial.print(temperature);
  Serial.print("°");
  Serial.println("C");

  // Muestra la humedad
  Serial.print(F("Humedad: "));
  Serial.print(humidity);
  Serial.println(F("%"));
}


// Lee el sensor de calidad del aire MQ135
void readMQ() {

  // Obtiene la lectura analógica del sensor
  airQuality = analogRead(MQ135);

  // Muestra la lectura ADC en el monitor serial
  Serial.print(F("Calidad de Aire (ADC): "));
  Serial.println(airQuality);
}


// Evalúa las lecturas y determina el estado del sistema
void checkStatus() {

  // Comprueba si existe alguna condición crítica
  if (temperature > 35.0 || airQuality > 300 || normalizedValueLDR < 20) {

    // Establece el estado crítico
    overallStatus = systemStatus[0];

    // Enciende el LED rojo y apaga los demás
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    // Activa el buzzer durante 500 milisegundos
    tone(BUZZER, 3000, 500);
  }

  // Comprueba si existe alguna condición de advertencia
  else if ((temperature > 28.0 && temperature <= 35.0) || (airQuality > 150 && airQuality <= 300) || presence == true) {

    // Establece el estado de advertencia
    overallStatus = systemStatus[1];

    // Enciende el LED amarillo
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);

    // Desactiva el sonido de alerta
    noTone(BUZZER);
  }

  // Si no hay condiciones críticas ni advertencias
  else {

    // Establece el estado normal
    overallStatus = systemStatus[2];

    // Enciende el LED azul y apaga los demás
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);

    // Mantiene el buzzer apagado
    noTone(BUZZER);
  }

  // Muestra el estado general en el monitor serial
  Serial.print(F("ESTADO GENERAL: "));
  Serial.println(overallStatus);
}

void updateOLED() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 2);

  // Encabezado
  display.print(F("ESTADO: ")); 
  display.println(overallStatus);
  display.println(F("---------------------"));

  // Despliegle de variables
  display.print(F("T:")); display.print(temperature, 1); display.print((char)247); display.print(F("C"));
  display.print(F(" | H:")); display.print(humidity, 1); display.println(F("%"));

  display.print(F("Luz:")); display.print(normalizedValueLDR); display.print(F("%"));
  display.print(F(" | M: ")); display.println(presence ? F("S") : F("N"));

  display.print(F("Aire (ADC):")); display.println(airQuality);
  

  display.display(); // Envío de búfer a la pantalla física
}