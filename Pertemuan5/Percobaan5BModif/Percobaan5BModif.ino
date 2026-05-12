/*
 * DHT Sensor + Simulasi Task (Non-blocking dengan millis())
 *
 * Disesuaikan untuk Wokwi — menggantikan FreeRTOS Queue dengan
 * pendekatan millis()-based agar kompatibel dengan simulator.
 *
 * Perilaku identik dengan versi FreeRTOS:
 *   - "Task" read_data  : membaca sensor tiap 2000 ms
 *   - "Task" display    : menampilkan data ke Serial Monitor
 *                         hanya jika ada data baru (flag newData)
 *
 * Wiring (sesuai diagram.json):
 *   DHT SDA (DATA) -> Arduino pin 2
 *   DHT VCC        -> 5V
 *   DHT GND        -> GND
 */

#include <DHT.h>

// ── Konfigurasi sensor ──────────────────────────────────────────
#define DHTPIN   2        // Sesuai koneksi di diagram.json
#define DHTTYPE  DHT22    // Sesuai atribut "type":"DHT22" di diagram.json

DHT dht(DHTPIN, DHTTYPE);

// ── Struktur data (sama seperti versi FreeRTOS) ─────────────────
struct readings {
  float temp;   // Suhu dalam derajat Celsius
  float h;      // Kelembaban relatif dalam persen
  bool  valid;  // Flag: true jika pembacaan berhasil
};

// ── "Queue" sederhana: satu slot data + flag ketersediaan ────────
readings queueSlot;        // Slot penyimpanan data (setara queue size=1)
bool     newData = false;  // Flag pengganti xQueueOverwrite/Receive

// ── Timing untuk simulasi task ───────────────────────────────────
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000;  // ms — sama dengan vTaskDelay(2000)

// ───────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);   // Baud rate sesuai serialMonitor di diagram.json
  dht.begin();

  Serial.println(F("=== DHT Monitor dimulai ==="));
  Serial.println(F("Menunggu pembacaan pertama..."));
  Serial.println(F("--------------------"));
}

// ───────────────────────────────────────────────────────────────
// Simulasi Task 1 — read_data
// Dipanggil dari loop(), aktif setiap READ_INTERVAL ms
// Setara: xQueueOverwrite(my_queue, &data) + vTaskDelay(2000)
// ───────────────────────────────────────────────────────────────
void task_read_data() {
  unsigned long now = millis();
  if (now - lastReadTime < READ_INTERVAL) return;  // Belum waktunya
  lastReadTime = now;

  readings data;
  data.h    = dht.readHumidity();
  data.temp = dht.readTemperature();  // Celsius
  data.valid = !(isnan(data.h) || isnan(data.temp));

  // Timpa slot (setara xQueueOverwrite — tidak peduli apakah sudah dibaca)
  queueSlot = data;
  newData   = true;
}

// ───────────────────────────────────────────────────────────────
// Simulasi Task 2 — display
// Dipanggil dari loop(), hanya aktif saat ada data baru
// Setara: xQueueReceive(my_queue, &data, portMAX_DELAY)
// ───────────────────────────────────────────────────────────────
void task_display() {
  if (!newData) return;   // Tidak ada data baru, lewati
  newData = false;        // Konsumsi data (setara setelah Receive berhasil)

  readings data = queueSlot;

  if (data.valid) {
    Serial.print(F("Suhu      : "));
    Serial.print(data.temp, 1);
    Serial.println(F(" °C"));

    Serial.print(F("Kelembaban: "));
    Serial.print(data.h, 1);
    Serial.println(F(" %"));

    // Hitung Heat Index (indeks panas)
    float hi = dht.computeHeatIndex(data.temp, data.h, false);
    Serial.print(F("Heat Index: "));
    Serial.print(hi, 1);
    Serial.println(F(" °C"));

    Serial.println(F("--------------------"));
  } else {
    Serial.println(F("ERROR: Gagal membaca sensor DHT!"));
    Serial.println(F("Periksa kabel dan koneksi sensor."));
    Serial.println(F("--------------------"));
  }
}

// ───────────────────────────────────────────────────────────────
void loop() {
  task_read_data();   // "Task 1" — cek & baca sensor
  task_display();     // "Task 2" — tampilkan jika ada data baru
}