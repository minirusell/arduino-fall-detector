# arduino-fall-detector
# Fall Detector (Arduino + MPU6050)

Proyecto académico en Arduino para detectar posibles caídas usando un sensor inercial (acelerómetro/giroscopio), con lógica basada en umbrales sobre el movimiento.

El objetivo es identificar eventos compatibles con una caída y diferenciarlos de movimientos normales (por ejemplo, sentarse o caminar), aplicando un procesamiento sencillo en tiempo real.

---

## Qué hace el proyecto

- Lee datos del sensor inercial (MPU6050)
- Calcula magnitudes de aceleración / movimiento
- Evalúa condiciones de detección mediante umbrales
- Lanza una alerta cuando detecta un patrón compatible con caída

> En algunas versiones del proyecto también se incluye una pantalla OLED para mostrar datos o estados del sistema.

---

## Objetivo del proyecto

Este proyecto está enfocado a practicar:

- Lectura de sensores en Arduino
- Adquisición de datos en tiempo real
- Diseño de lógica de detección por umbrales
- Filtrado / validación básica de eventos
- Programación de sistemas embebidos

---

## Componentes usados

- Arduino (placa compatible)
- MPU6050 (acelerómetro + giroscopio)
- (Opcional) Pantalla OLED SSD1306
- Cableado I2C

---

## Conceptos trabajados

- Sensores inerciales (IMU)
- Aceleración y movimiento angular
- Detección de eventos
- Umbrales y ventanas temporales
- Programación en Arduino/C++

---

## Archivo principal

- `Fall_detector_2.ino`

---

## Cómo ejecutar

1. Abrir el archivo `Fall_detector_2.ino` en el **Arduino IDE**
2. Instalar las librerías necesarias (si no están instaladas), por ejemplo:
   - `Adafruit MPU6050`
   - `Adafruit Sensor`
   - `Adafruit SSD1306` (si se usa pantalla)
3. Seleccionar la placa y el puerto
4. Subir el programa al Arduino
5. Monitorizar la salida por puerto serie (si aplica)

---

## Autor

Proyecto realizado por **Marcos** como práctica universitaria.
