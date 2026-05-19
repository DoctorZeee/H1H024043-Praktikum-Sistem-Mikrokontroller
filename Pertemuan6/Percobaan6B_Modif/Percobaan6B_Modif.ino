#include <Arduino.h>

// ====================== PIN SETUP ======================
const int LED1 = 13;   // LED pertama
const int LED2 = 12;   // LED kedua

// ====================== STATE LED ======================
bool led1State = false;
bool led2State = false;

// ====================== TIMING ======================
unsigned long previousMillisLED1 = 0;
unsigned long previousMillisLED2 = 0;

const unsigned long intervalLED1 = 1000; // 1 detik
const unsigned long intervalLED2 = 500;  // 500 ms

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // ====================== LED 1 (1 detik) ======================
  if (currentMillis - previousMillisLED1 >= intervalLED1) {
    previousMillisLED1 = currentMillis;      // update waktu terakhir
    led1State = !led1State;                  // toggle LED
    digitalWrite(LED1, led1State);
  }

  // ====================== LED 2 (500 ms) ======================
  if (currentMillis - previousMillisLED2 >= intervalLED2) {
    previousMillisLED2 = currentMillis;      // update waktu terakhir
    led2State = !led2State;                  // toggle LED
    digitalWrite(LED2, led2State);
  }
}