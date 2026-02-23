#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

// Definir el tamaño de la pantalla SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Instancia del MPU6050
Adafruit_MPU6050 mpu;

// Instancia del SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin del LED
const int ledPin = 23; // Puede ser cambiado

// Parámetros de la estrategia (umbrales a ajustar según pruebas):
const float fallMaxThreshold = 700.0;     // pico máximo
const float fallMinThreshold = 20.0;      // valor mínimo muy bajo
const float fallRangeThreshold = 500.0;   // rango max-min
const float stdDevThreshold = 250.0;      // umbral de desviación típica (aprox. entre sentarse y caída)

// Umbral para considerar que está en el suelo
const float floorThreshold = 100.0; 

// Tiempo de estabilización (en milisegundos)
const unsigned long stabilizationTime = 2000; // 2 segundos

// Variables para el tiempo
unsigned long fallTime = 0;
unsigned long floorTimeStart = 0;
bool isOnFloor = false;
bool fallDetected = false;
bool stabilizationCompleted = false;

// Buffer para almacenar las últimas N lecturas de accelTotal
#define N 10
float accelBuffer[N];
int accelIndex = 0;
bool bufferFilled = false;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  if (!mpu.begin()) {
    Serial.println("Error al inicializar el MPU6050");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  Serial.println("MPU6050 inicializado con éxito.");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("Error al inicializar la pantalla OLED");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Fall Detector");
  display.display();
  delay(2000);

  for (int i = 0; i < N; i++) {
    accelBuffer[i] = 0.0;
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float accelTotal = sqrt(a.acceleration.x * a.acceleration.x +
                          a.acceleration.y * a.acceleration.y +
                          a.acceleration.z * a.acceleration.z);

  float gyroTotal = sqrt(g.gyro.x * g.gyro.x +
                         g.gyro.y * g.gyro.y +
                         g.gyro.z * g.gyro.z);

  float combinedMagnitude = (accelTotal * accelTotal) + (gyroTotal * 0.5);

  // Actualizar buffer circular
  accelBuffer[accelIndex] = accelTotal;
  accelIndex = (accelIndex + 1) % N;
  if (accelIndex == 0) {
    bufferFilled = true;
  }

  // Depuración
  Serial.print("Accel Total: "); Serial.println(accelTotal);
  Serial.print("Gyro Total: "); Serial.println(gyroTotal);
  Serial.print("Combined Magnitude: "); Serial.println(combinedMagnitude);
  Serial.println("-------------------");

  // Solo intentamos detectar caída si el buffer está lleno (N muestras completas)
  if (!fallDetected && bufferFilled) {
    // Calcular max, min
    float maxVal = accelBuffer[0];
    float minVal = accelBuffer[0];
    float sumVal = 0.0;

    for (int i = 0; i < N; i++) {
      if (accelBuffer[i] > maxVal) maxVal = accelBuffer[i];
      if (accelBuffer[i] < minVal) minVal = accelBuffer[i];
      sumVal += accelBuffer[i];
    }

    float meanVal = sumVal / N;
    float rangeVal = maxVal - minVal;

    // Calcular la desviación típica
    float sumDiffSq = 0.0;
    for (int i = 0; i < N; i++) {
      float diff = accelBuffer[i] - meanVal;
      sumDiffSq += diff * diff;
    }
    float variance = sumDiffSq / N;
    float stdDev = sqrt(variance);

    // Depuración stdDev
    Serial.print("Max: "); Serial.println(maxVal);
    Serial.print("Min: "); Serial.println(minVal);
    Serial.print("Range: "); Serial.println(rangeVal);
    Serial.print("Mean: "); Serial.println(meanVal);
    Serial.print("StdDev: "); Serial.println(stdDev);
    Serial.println("===================");

    // Condiciones de caída:
    // - maxVal > fallMaxThreshold
    // - minVal < fallMinThreshold
    // - rangeVal > fallRangeThreshold
    // - stdDev > stdDevThreshold
    if (maxVal > fallMaxThreshold && minVal < fallMinThreshold &&
        rangeVal > fallRangeThreshold && stdDev > stdDevThreshold) {

      digitalWrite(ledPin, HIGH);
      fallTime = millis();
      fallDetected = true;
      stabilizationCompleted = false;

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("ALERTA!");
      display.setTextSize(1);
      display.println("Caida detectada");
      display.display();
    }
  }

  // Verificar si ha pasado el tiempo de estabilización
  if (fallDetected && !stabilizationCompleted && (millis() - fallTime > stabilizationTime)) {
    stabilizationCompleted = true;
  }

  // Verificar si la persona está en el suelo
  if (stabilizationCompleted) {
    if (accelTotal < floorThreshold) {
      if (!isOnFloor) {
        isOnFloor = true;
        floorTimeStart = millis();
      }

      unsigned long timeOnFloor = (millis() - floorTimeStart) / 1000;
      Serial.print("Tiempo en el suelo: "); Serial.print(timeOnFloor); Serial.println(" segundos");

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("ALERTA!");
      display.setTextSize(1);
      display.setCursor(0, 20);
      display.print("Tiempo en suelo:");
      display.setCursor(0, 40);
      display.print(timeOnFloor);
      display.print(" segundos");
      display.display();

      digitalWrite(ledPin, HIGH);
    } else {
      // Se ha levantado
      isOnFloor = false;
      fallDetected = false;
      stabilizationCompleted = false;

      digitalWrite(ledPin, LOW);

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Sistema activo");
      display.display();
    }
  }

  delay(50);
}