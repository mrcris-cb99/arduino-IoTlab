# Laboratorio IoT con Dashboard de Monitoreo Remoto

Sistema embebido de monitoreo ambiental y detección de presencia, desarrollado con **Arduino UNO R4 WiFi**. El proyecto utiliza sensores para recopilar datos del entorno, evalúa las condiciones mediante una **Máquina de Estados Finitos (FSM)** y muestra los resultados localmente y en Arduino Cloud.

## Características principales

- **Monitoreo ambiental:** lectura de temperatura, humedad relativa, calidad del aire e iluminación.
- **Detección de presencia:** utilización del sensor PIR HC-SR501 para detectar movimiento.
- **Evaluación de estados:** clasificación del entorno en `CRITICO`, `ADVERTENCIA` y `NORMAL`.
- **Indicadores locales:** pantalla OLED SSD1306, tres LED de estado y un buzzer para alertas.
- **Monitoreo remoto:** sincronización de cinco propiedades con Arduino Cloud.
- **Código modular:** separación de la lógica del sistema, las propiedades de la nube y las credenciales de conexión.

> **Nota:** El sensor MQ-135 proporciona lecturas analógicas orientativas. Este prototipo es académico y no sustituye equipos certificados de detección de gases ni sistemas de seguridad profesionales.

## Hardware utilizado

| Componente | Pin de Arduino | Función |
|---|---|---|
| DHT22 | `D4` | Medición de temperatura y humedad |
| PIR HC-SR501 | `D3` | Detección de movimiento |
| LDR con divisor de tensión | `A0` | Medición relativa de iluminación |
| MQ-135 | `A1` | Lectura analógica de calidad del aire |
| OLED SSD1306 | `SDA` / `SCL` | Visualización de datos mediante I2C |
| LED rojo | `D13` | Indicador de estado crítico |
| LED amarillo | `D12` | Indicador de advertencia |
| LED azul | `D8` | Indicador de estado normal |
| Buzzer | `D2` | Alerta acústica |

**Componentes adicionales:** Arduino UNO R4 WiFi, protoboard, cables Dupont, tres resistencias de 220 Ω para los LED y una resistencia de 10 kΩ para el divisor de tensión de la LDR. El DHT22 utiliza una resistencia pull-up de 10 kΩ según el montaje documentado.


## Lógica de funcionamiento

El sistema evalúa las lecturas de los sensores y asigna un estado global según las condiciones implementadas en el código.

| Estado | Condición implementada | Indicador y respuesta |
|---|---|---|
| `CRITICO` | Temperatura superior a 35 °C, lectura MQ-135 superior a 300 ADC o iluminación inferior al 20 % | LED rojo y activación del buzzer |
| `ADVERTENCIA` | Sin condición crítica y temperatura superior a 28 °C, lectura MQ-135 superior a 100 ADC o presencia detectada | LED amarillo y buzzer silenciado |
| `NORMAL` | No se cumple ninguna condición anterior | LED azul y buzzer apagado |

La evaluación sigue el orden de prioridad **CRITICO → ADVERTENCIA → NORMAL**.

El comportamiento del LED rojo en el código documentado es de encendido fijo. El buzzer recibe una orden de tono de 500 ms cuando se cumple una condición crítica.

> **Importante:** los umbrales anteriores describen la lógica del código. La matriz de diseño del informe presenta algunas diferencias, especialmente en el umbral de calidad del aire y en la condición de oscuridad con presencia. Si actualizas el firmware, mantén esta tabla sincronizada con la implementación definitiva.

## Variables sincronizadas con Arduino Cloud

El proyecto sincroniza cinco propiedades con la nube:

| Propiedad | Tipo | Descripción |
|---|---|---|
| `overallStatus` | String | Estado general del sistema |
| `temperature` | Float | Temperatura medida por el DHT22 |
| `airQuality` | Integer | Lectura analógica del MQ-135 |
| `normalizedValueLDR` | Integer | Valor de iluminación normalizado de 0 a 100 |
| `presence` | Boolean | Estado de detección del sensor PIR |

La humedad relativa se mantiene como variable local y se muestra en la pantalla OLED.

Para visualizar las lecturas, crea un **Thing** en Arduino Cloud, vincula la placa y configura las cinco propiedades con los nombres y tipos compatibles con el firmware. El dashboard debe configurarse con los widgets que se deseen utilizar.

## Estructura del repositorio

```text
laboratorio-iot-monitoreo/
├── ArduinoIoT.ino
├── thingProperties.h
├── arduino_secrets.h.example
├── .gitignore
├── README.md
└── LICENSE
```

- `ArduinoIoT.ino`: programa principal, lectura de sensores, evaluación de estados y actualización de la pantalla OLED.
- `thingProperties.h`: declaraciones y vinculación de las propiedades con Arduino Cloud.
- `arduino_secrets.h.example`: plantilla de ejemplo para las credenciales, sin contraseñas reales.
- `.gitignore`: exclusión de archivos privados y archivos temporales.
- `README.md`: documentación del proyecto.
- `LICENSE`: licencia de distribución, si corresponde.


## Requisitos previos

Antes de cargar el proyecto, necesitarás:

- Arduino IDE 2.x.
- Paquete de tarjetas Arduino UNO R4 Boards.
- Una placa Arduino UNO R4 WiFi.
- Conexión Wi-Fi compatible.
- Una cuenta y un Thing configurado en Arduino Cloud.
- Las siguientes bibliotecas:
  - ArduinoIoTCloud
  - Arduino_ConnectionHandler
  - DHT sensor library (Adafruit)
  - Adafruit SSD1306
  - Adafruit GFX Library

Instala las bibliotecas desde el Library Manager de Arduino IDE cuando sea necesario.

## Instalación y configuración

### 1. Clonar el repositorio

```bash
git clone https://github.com/mrcris-cb99/arduino-IoTlab.git
```

### 2. Configurar las credenciales Wi-Fi

Crea localmente un archivo llamado `arduino_secrets.h` a partir de la plantilla `arduino_secrets.h.example`.

El archivo local debe definir las macros que espera el firmware:

```cpp
#define SECRET_SSID "Nombre_de_tu_WiFi"
#define SECRET_OPTIONAL_PASS "Contraseña_de_tu_WiFi"
```

### 3. Configurar Arduino Cloud

1. Accede a Arduino Cloud y crea un Thing.
2. Asocia tu placa Arduino UNO R4 WiFi.
3. Configura las cinco propiedades indicadas en la sección anterior.
4. Verifica que los nombres, tipos y permisos coincidan con los utilizados por el firmware.
5. Genera o actualiza `thingProperties.h` según la configuración de tu dispositivo.
6. Configura el dashboard con los widgets correspondientes a las propiedades que quieras visualizar.

### 4. Compilar y cargar el programa

1. Abre `ArduinoIoT.ino` en Arduino IDE.
2. Comprueba que los archivos de configuración y las bibliotecas estén disponibles.
3. Selecciona la placa Arduino UNO R4 WiFi y el puerto correspondiente.
4. Compila el proyecto y corrige cualquier error que aparezca.
5. Carga el programa en la placa.
6. Abre el monitor serie para comprobar las lecturas y los mensajes de diagnóstico.
7. Verifica la visualización en la OLED y la sincronización con Arduino Cloud.

## Calibración y pruebas

Antes de evaluar el funcionamiento, ten en cuenta las recomendaciones documentadas:

- **MQ-135:** necesita un periodo de precalentamiento adecuado; el informe contempla 24 horas antes de las pruebas de calibración.
- **DHT22:** respeta un intervalo de lectura de al menos dos segundos.
- **LDR:** comprueba que el porcentaje calculado corresponda razonablemente con las condiciones de iluminación.
- **PIR:** ajusta la sensibilidad y el tiempo de retención según el montaje.


## Posibles mejoras futuras

- Incorporar notificaciones remotas ante eventos críticos.
- Registrar y consultar el historial de las lecturas.
- Mejorar la calibración y el filtrado de los sensores.
- Unificar la matriz de estados del informe con las condiciones definitivas del firmware.
- Implementar mecanismos de recuperación ante errores de sensores o pérdida de conexión.

## Licencia

Este proyecto fue desarrollado con fines académicos. Libre uso bajo la licencia MIT.
