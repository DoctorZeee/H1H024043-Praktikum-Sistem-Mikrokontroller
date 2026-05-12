#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define POT_PIN  A0
#define LED1_PIN  8
#define LED2_PIN  7

SemaphoreHandle_t xMutex;
volatile int blinkSpeed = 200; // nilai default delay (ms)

void TaskReadPot(void *pvParameters);
void TaskBlink1(void *pvParameters);
void TaskBlink2(void *pvParameters);
void Taskprint(void *pvParameters);

void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);

  xMutex = xSemaphoreCreateMutex();

  xTaskCreate(TaskReadPot, "ReadPot", 128, NULL, 2, NULL); // prioritas lebih tinggi
  xTaskCreate(TaskBlink1,  "task1",   128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2,  "task2",   128, NULL, 1, NULL);
  xTaskCreate(Taskprint,   "task3",   128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {}

// Task membaca potensiometer dan update blinkSpeed
void TaskReadPot(void *pvParameters) {
  while (1) {
    int potValue = analogRead(POT_PIN);          // 0 - 1023
    int newSpeed = map(potValue, 0, 1023, 50, 1000); // mapping ke 50ms - 1000ms

    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      blinkSpeed = newSpeed;
      xSemaphoreGive(xMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task LED 1 - kecepatan dipengaruhi potensiometer
void TaskBlink1(void *pvParameters) {
  pinMode(LED1_PIN, OUTPUT);
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    Serial.println("Task1 - LED1 Blink");
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(speed / portTICK_PERIOD_MS);
    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(speed / portTICK_PERIOD_MS);
  }
}

// Task LED 2 - kecepatan 1.5x lebih lambat dari LED1
void TaskBlink2(void *pvParameters) {
  pinMode(LED2_PIN, OUTPUT);
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    Serial.println("Task2 - LED2 Blink");
    digitalWrite(LED2_PIN, HIGH);
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS);
    digitalWrite(LED2_PIN, LOW);
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS);
  }
}

// Task print counter + tampilkan blinkSpeed saat ini
void Taskprint(void *pvParameters) {
  int counter = 0;
  while (1) {
    int speed;
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    counter++;
    Serial.print("Counter: ");
    Serial.print(counter);
    Serial.print(" | Blink Speed: ");
    Serial.print(speed);
    Serial.println(" ms");

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}