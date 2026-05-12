Nama: Dimas Rafif Zaidan  
NIM: H1H024043  
Shift Awal: B  
Shift Akhir: B  

---

# Praktikum Modul 5 — Real-Time Operating System: Multitasking dan Komunikasi Task

---

# Percobaan 5A — Multitasking dengan FreeRTOS

## Foto Rangkaian

### Foto Rangkaian Fisik
<img width="2160" height="2880" alt="IMG_4239" src="https://github.com/user-attachments/assets/ff284902-e7cc-4abf-9063-45c27f83d544" />

---

## 5.5.4 Pertanyaan Praktikum

### 1. Apakah ketiga task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Ketiga task tersebut berjalan secara **bergantian**, bukan benar-benar bersamaan dalam satu waktu. Namun karena perpindahan antar task terjadi sangat cepat, seolah-olah ketiganya berjalan bersamaan (*concurrent*). Mekanisme ini diatur oleh **FreeRTOS Scheduler** yang menggunakan metode *preemptive scheduling* berbasis prioritas. Karena ketiga task (`TaskBlink1`, `TaskBlink2`, dan `Taskprint`) memiliki prioritas yang sama (priority = 1), scheduler akan membagi waktu CPU secara *round-robin* di antara ketiganya. Ketika suatu task memanggil `vTaskDelay()`, task tersebut akan masuk ke kondisi *blocked* dan CPU akan langsung diberikan ke task lain yang siap berjalan. Dengan demikian, selama satu task menunggu delay, task lainnya dapat menggunakan CPU sehingga sistem terasa *multitasking*.

---

### 2. Bagaimana cara menambahkan task keempat? Jelaskan langkahnya!

Untuk menambahkan task keempat, langkah pertama adalah **mendeklarasikan prototipe fungsi** task baru di bagian atas program, misalnya `void TaskKeempat(void *pvParameters);`. Selanjutnya, di dalam fungsi `setup()`, ditambahkan pemanggilan `xTaskCreate()` dengan parameter yang sesuai, meliputi nama fungsi task, nama string task, ukuran stack, parameter, prioritas, dan handle. Kemudian, **fungsi task keempat didefinisikan** di bagian bawah program dengan struktur `while(1)` yang berisi logika yang diinginkan dan pemanggilan `vTaskDelay()` agar task tidak memborosi CPU. Pastikan ukuran stack yang dialokasikan cukup untuk kebutuhan task tersebut, dan perhatikan prioritas task agar tidak mengganggu task lain yang lebih kritis.

---

### 3. Modifikasilah program dengan menambah sensor (misalnya potensiometer), lalu gunakan nilainya untuk mengontrol kecepatan LED! Bagaimana hasilnya? Jelaskan program pada file README.md.

## Foto Rangkaian Modifikasi

### Foto Rangkaian Fisik
<img width="1200" height="1600" alt="Potensio" src="https://github.com/user-attachments/assets/1af99999-220b-4234-b2a6-e9b3fbd4c5a4" />


---

```cpp
/*
 * Modul 5A — Multitasking FreeRTOS dengan Kontrol Kecepatan LED via Potensiometer
 *
 * Deskripsi:
 *   Program ini menjalankan 4 task secara concurrent menggunakan FreeRTOS.
 *   Kecepatan kedip LED dikontrol secara dinamis oleh nilai potensiometer
 *   yang dibaca melalui task tersendiri dengan prioritas lebih tinggi.
 *   Mutex digunakan untuk melindungi akses ke variabel global blinkSpeed
 *   agar tidak terjadi race condition antar task.
 *
 * Wiring:
 *   Potensiometer  -> A0
 *   LED 1          -> Pin 8
 *   LED 2          -> Pin 7
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// ── Definisi Pin ───────────────────────────────────────────────
#define POT_PIN   A0   // Pin analog untuk potensiometer
#define LED1_PIN   8   // Pin digital untuk LED 1
#define LED2_PIN   7   // Pin digital untuk LED 2

// ── Handle Mutex dan Variabel Global ──────────────────────────
SemaphoreHandle_t xMutex;             // Mutex untuk proteksi blinkSpeed
volatile int blinkSpeed = 200;        // Nilai default delay LED dalam ms

// ── Prototipe Fungsi Task ──────────────────────────────────────
void TaskReadPot(void *pvParameters); // Task pembaca potensiometer
void TaskBlink1(void *pvParameters);  // Task kedip LED 1
void TaskBlink2(void *pvParameters);  // Task kedip LED 2
void Taskprint(void *pvParameters);   // Task cetak counter ke Serial

// ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);

  // Buat mutex sebelum task dibuat agar tersedia saat task pertama kali berjalan
  xMutex = xSemaphoreCreateMutex();

  // Buat semua task; TaskReadPot diberi prioritas 2 agar pembacaan sensor
  // selalu diperbarui sebelum task LED menggunakannya
  xTaskCreate(TaskReadPot, "ReadPot", 128, NULL, 2, NULL);
  xTaskCreate(TaskBlink1,  "task1",   128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2,  "task2",   128, NULL, 1, NULL);
  xTaskCreate(Taskprint,   "task3",   128, NULL, 1, NULL);

  // Serahkan kontrol ke FreeRTOS Scheduler; fungsi ini tidak pernah return
  vTaskStartScheduler();
}

// loop() dibiarkan kosong karena FreeRTOS Scheduler yang mengelola eksekusi
void loop() {}

// ──────────────────────────────────────────────────────────────
// Task 1 — TaskReadPot
// Membaca nilai potensiometer setiap 100 ms dan memetakan hasilnya
// ke rentang delay 50–1000 ms, lalu menyimpannya ke blinkSpeed.
// Prioritas lebih tinggi (2) memastikan nilai selalu diperbarui tepat waktu.
// ──────────────────────────────────────────────────────────────
void TaskReadPot(void *pvParameters) {
  while (1) {
    int potValue = analogRead(POT_PIN);                   // Baca ADC 0–1023
    int newSpeed = map(potValue, 0, 1023, 50, 1000);      // Petakan ke 50–1000 ms

    // Ambil mutex sebelum menulis ke variabel global
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      blinkSpeed = newSpeed;   // Perbarui kecepatan kedip
      xSemaphoreGive(xMutex);  // Lepas mutex agar task lain bisa mengakses
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Tunggu 100 ms sebelum baca ulang
  }
}

// ──────────────────────────────────────────────────────────────
// Task 2 — TaskBlink1
// Mengedipkan LED 1 dengan delay yang diambil dari blinkSpeed.
// Kecepatan kedip berubah sesuai posisi potensiometer.
// ──────────────────────────────────────────────────────────────
void TaskBlink1(void *pvParameters) {
  pinMode(LED1_PIN, OUTPUT);
  while (1) {
    int speed;

    // Baca blinkSpeed secara aman menggunakan mutex
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      speed = blinkSpeed;
      xSemaphoreGive(xMutex);
    }

    Serial.println("Task1 - LED1 Blink");
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(speed / portTICK_PERIOD_MS); // ON selama 'speed' ms
    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(speed / portTICK_PERIOD_MS); // OFF selama 'speed' ms
  }
}

// ──────────────────────────────────────────────────────────────
// Task 3 — TaskBlink2
// Mengedipkan LED 2 dengan delay 1.5x lebih lambat dari LED 1,
// sehingga kedua LED terlihat berbeda ritmenya.
// ──────────────────────────────────────────────────────────────
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
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS); // ON 1.5x lebih lambat
    digitalWrite(LED2_PIN, LOW);
    vTaskDelay((speed * 1.5) / portTICK_PERIOD_MS); // OFF 1.5x lebih lambat
  }
}

// ──────────────────────────────────────────────────────────────
// Task 4 — Taskprint
// Mencetak nilai counter dan kecepatan kedip saat ini ke Serial Monitor
// setiap 500 ms sebagai informasi debug dan monitoring sistem.
// ──────────────────────────────────────────────────────────────
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

    vTaskDelay(500 / portTICK_PERIOD_MS); // Cetak setiap 500 ms
  }
}
```

### Penjelasan Program

| Bagian | Penjelasan |
|--------|------------|
| `#include <Arduino_FreeRTOS.h>` | Menyertakan library FreeRTOS untuk Arduino |
| `#include <semphr.h>` | Menyertakan header untuk fungsi semaphore dan mutex |
| `xMutex = xSemaphoreCreateMutex()` | Membuat mutex untuk melindungi akses ke `blinkSpeed` |
| `TaskReadPot` (prioritas 2) | Membaca ADC potensiometer dan memperbarui `blinkSpeed` setiap 100 ms |
| `map(potValue, 0, 1023, 50, 1000)` | Memetakan nilai ADC 10-bit ke rentang delay 50–1000 ms |
| `xSemaphoreTake / xSemaphoreGive` | Mengunci dan melepas mutex agar akses variabel global aman |
| `TaskBlink1` | Mengedipkan LED 1 sesuai nilai `blinkSpeed` terkini |
| `TaskBlink2` | Mengedipkan LED 2 dengan delay 1.5x dari `blinkSpeed` |
| `Taskprint` | Mencetak counter dan `blinkSpeed` ke Serial Monitor tiap 500 ms |
| `vTaskDelay()` | Melepas CPU ke task lain selama periode tunggu |

### Tabel Pemetaan Potensiometer

| Posisi Potensiometer | Nilai ADC | Delay LED 1 | Delay LED 2 |
|----------------------|-----------|-------------|-------------|
| Minimum (kiri penuh) | 0         | 50 ms       | 75 ms       |
| Tengah               | 512       | ~525 ms     | ~787 ms     |
| Maksimum (kanan penuh) | 1023    | 1000 ms     | 1500 ms     |

### Hasil

Ketika potensiometer diputar ke kiri (nilai ADC mendekati 0), kedua LED berkedip sangat cepat dengan delay mendekati 50 ms. Saat diputar ke kanan (nilai ADC mendekati 1023), kedip melambat hingga 1000 ms. LED 2 selalu berkedip 1.5x lebih lambat dari LED 1 sehingga pola kedipnya terlihat berbeda. Serial Monitor mencetak counter yang terus bertambah beserta nilai kecepatan saat ini setiap 500 ms, memudahkan pemantauan kondisi sistem secara real-time.

---
---

# Percobaan 5B — Komunikasi Task dengan Queue

### Demo Serial Monitor
<img width="2160" height="2880" alt="IMG_6288" src="https://github.com/user-attachments/assets/31fb948a-2281-41e0-806e-f64a5e9ad1ce" />


---

## 5.6.4 Pertanyaan Praktikum

### 1. Apakah kedua task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Kedua task (`read_data` dan `display`) berjalan secara **bergantian**, dikendalikan oleh FreeRTOS Scheduler. Mekanisme komunikasinya menggunakan **Queue** (`xQueueCreate`), yaitu sebuah buffer FIFO yang menjadi perantara antar task. Task `read_data` bertugas mengisi data ke dalam queue menggunakan `xQueueSend()`, lalu memanggil `vTaskDelay()` sehingga ia masuk ke kondisi *blocked*. Pada saat itulah CPU beralih ke task `display` yang menunggu data dari queue menggunakan `xQueueReceive()` dengan parameter `portMAX_DELAY`, artinya ia akan terus menunggu (*blocked*) sampai ada data tersedia. Ketika data berhasil diterima, task `display` memproses dan mencetak data ke Serial Monitor, lalu kembali menunggu. Pola ini membuat kedua task saling bersinkronisasi melalui queue secara efisien.

---

### 2. Apakah program ini berpotensi mengalami race condition? Jelaskan!

Program ini **tidak berpotensi mengalami race condition** secara signifikan karena komunikasi antar task sudah menggunakan mekanisme Queue yang disediakan oleh FreeRTOS. Queue bersifat *thread-safe*, artinya FreeRTOS menjamin bahwa operasi `xQueueSend()` dan `xQueueReceive()` bersifat *atomic* dan tidak dapat diinterupsi di tengah jalan oleh task lain. Dengan demikian, tidak akan terjadi kondisi di mana kedua task mengakses data yang sama secara bersamaan tanpa sinkronisasi. Race condition biasanya terjadi ketika dua task mengakses variabel global secara langsung tanpa mekanisme proteksi seperti mutex atau semaphore. Karena program ini menggunakan queue sebagai satu-satunya media berbagi data, risiko race condition dapat dihindari.

---

### 3. Modifikasilah program dengan menggunakan sensor DHT sesungguhnya sehingga informasi yang ditampilkan dinamis. Bagaimana hasilnya? Jelaskan program pada file README.md.

## Foto Rangkaian Modifikasi

### Foto Rangkaian
<img width="1920" height="1080" alt="Screenshot From 2026-05-12 23-52-10" src="https://github.com/user-attachments/assets/3a9b577c-be15-4bdd-9c80-b67c8f3548a4" />


---

```cpp
/*
 * Modul 5B — Komunikasi Task FreeRTOS dengan Sensor DHT22
 *
 * Deskripsi:
 *   Program ini menerapkan komunikasi antar task menggunakan Queue FreeRTOS.
 *   Task read_data membaca data dari sensor DHT22 secara periodik dan
 *   mengirimkannya ke queue. Task display menerima data dari queue dan
 *   menampilkannya ke Serial Monitor termasuk perhitungan Heat Index.
 *   Pendekatan queue memastikan tidak ada race condition karena FreeRTOS
 *   menjamin thread-safety pada operasi xQueueSend dan xQueueReceive.
 *
 * Wiring:
 *   DHT22 DATA -> Pin 2
 *   DHT22 VCC  -> 5V
 *   DHT22 GND  -> GND
 */

#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <DHT.h>

// ── Konfigurasi Sensor DHT ─────────────────────────────────────
#define DHTPIN   2        // Pin data sensor DHT22
#define DHTTYPE  DHT22    // Tipe sensor: DHT22 (AM2302)

DHT dht(DHTPIN, DHTTYPE); // Inisialisasi objek sensor DHT

// ── Struktur Data untuk Queue ──────────────────────────────────
// Menyimpan satu set pembacaan sensor: suhu, kelembaban, dan status validitas
struct readings {
  float temp;   // Suhu dalam derajat Celsius
  float h;      // Kelembaban relatif dalam persen
  bool  valid;  // true jika pembacaan berhasil, false jika sensor error
};

// ── Handle Queue ───────────────────────────────────────────────
QueueHandle_t my_queue; // Queue dengan kapasitas 1 slot (size = 1)

// ── Prototipe Fungsi Task ──────────────────────────────────────
void read_data(void *pvParameters); // Task pembaca sensor DHT22
void display(void *pvParameters);   // Task penampil data ke Serial Monitor

// ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  dht.begin(); // Inisialisasi sensor DHT22

  // Buat queue dengan kapasitas 1 elemen bertipe struct readings
  // Kapasitas 1 cukup karena task display selalu mengonsumsi sebelum data baru datang
  my_queue = xQueueCreate(1, sizeof(struct readings));

  // Buat task dengan prioritas sama; scheduler akan mengatur eksekusi round-robin
  xTaskCreate(read_data, "read sensors", 128, NULL, 1, NULL);
  xTaskCreate(display,   "display",      128, NULL, 1, NULL);

  // Serahkan kontrol ke FreeRTOS Scheduler
  vTaskStartScheduler();
}

void loop() {} // Dikosongkan karena FreeRTOS yang mengelola eksekusi

// ──────────────────────────────────────────────────────────────
// Task 1 — read_data
// Membaca suhu dan kelembaban dari sensor DHT22 setiap 2000 ms,
// mengemas hasilnya ke dalam struct readings, lalu mengirim ke queue.
// Flag valid digunakan untuk memberi tahu task display apakah data dapat dipercaya.
// ──────────────────────────────────────────────────────────────
void read_data(void *pvParameters) {
  struct readings data;

  for (;;) {
    // Baca data dari sensor DHT22
    data.h    = dht.readHumidity();    // Kelembaban relatif (%)
    data.temp = dht.readTemperature(); // Suhu dalam Celsius

    // Validasi: isnan() mendeteksi kegagalan pembacaan sensor
    data.valid = !(isnan(data.h) || isnan(data.temp));

    // Kirim data ke queue; portMAX_DELAY artinya tunggu selamanya jika queue penuh
    // Dalam kasus ini queue berkapasitas 1 sehingga harus menunggu display mengonsumsi dulu
    xQueueSend(my_queue, &data, portMAX_DELAY);

    vTaskDelay(2000 / portTICK_PERIOD_MS); // Baca sensor setiap 2 detik
  }
}

// ──────────────────────────────────────────────────────────────
// Task 2 — display
// Menunggu data dari queue menggunakan portMAX_DELAY (blocked sampai ada data).
// Setelah data diterima, menampilkan suhu, kelembaban, dan Heat Index ke Serial Monitor.
// Jika pembacaan tidak valid, menampilkan pesan error.
// ──────────────────────────────────────────────────────────────
void display(void *pvParameters) {
  struct readings data;

  for (;;) {
    // Tunggu data dari queue; task ini blocked sampai read_data mengirim data
    if (xQueueReceive(my_queue, &data, portMAX_DELAY) == pdPASS) {

      if (data.valid) {
        // Tampilkan suhu
        Serial.print(F("Suhu      : "));
        Serial.print(data.temp, 1);   // 1 angka desimal
        Serial.println(F(" C"));

        // Tampilkan kelembaban
        Serial.print(F("Kelembaban: "));
        Serial.print(data.h, 1);
        Serial.println(F(" %"));

        // Hitung dan tampilkan Heat Index (indeks kenyamanan termal)
        // Parameter false = hasil dalam Celsius
        float hi = dht.computeHeatIndex(data.temp, data.h, false);
        Serial.print(F("Heat Index: "));
        Serial.print(hi, 1);
        Serial.println(F(" C"));

        Serial.println(F("--------------------")); // Pemisah tiap set data
      } else {
        // Tampilkan pesan error jika sensor gagal dibaca
        Serial.println(F("ERROR: Gagal membaca sensor DHT!"));
        Serial.println(F("Periksa kabel dan koneksi sensor."));
        Serial.println(F("--------------------"));
      }
    }
  }
}
```

### Penjelasan Program

| Bagian | Penjelasan |
|--------|------------|
| `#include <DHT.h>` | Menyertakan library DHT untuk membaca sensor DHT22 |
| `struct readings` | Struktur data yang menampung suhu, kelembaban, dan flag validitas pembacaan |
| `xQueueCreate(1, sizeof(struct readings))` | Membuat queue berkapasitas 1 elemen untuk komunikasi antar task |
| `dht.readHumidity()` | Membaca nilai kelembaban relatif dari sensor DHT22 |
| `dht.readTemperature()` | Membaca nilai suhu dalam satuan Celsius dari sensor DHT22 |
| `isnan()` | Mendeteksi kegagalan pembacaan sensor; sensor mengembalikan NaN jika error |
| `xQueueSend(my_queue, &data, portMAX_DELAY)` | Mengirim struct data ke queue; menunggu jika queue penuh |
| `xQueueReceive(my_queue, &data, portMAX_DELAY)` | Menerima data dari queue; task blocked sampai data tersedia |
| `dht.computeHeatIndex(temp, h, false)` | Menghitung Heat Index dalam Celsius berdasarkan suhu dan kelembaban |
| `vTaskDelay(2000 / portTICK_PERIOD_MS)` | Melepas CPU selama 2 detik sebelum membaca sensor kembali |

### Tabel Contoh Output Serial Monitor

| Data | Kondisi Normal | Kondisi Error |
|------|---------------|---------------|
| Suhu | 28.5 C | — |
| Kelembaban | 72.3 % | — |
| Heat Index | 31.2 C | — |
| Status | -------------------- | ERROR: Gagal membaca sensor DHT! |

### Hasil

Setelah sensor DHT22 terpasang dan program dijalankan, Serial Monitor menampilkan data suhu, kelembaban, dan Heat Index yang diperbarui setiap 2 detik secara dinamis. Nilai berubah sesuai kondisi lingkungan sekitar sensor. Jika kabel sensor dicabut atau terjadi gangguan koneksi, program mencetak pesan error tanpa crash karena validasi `isnan()` menangani kondisi tersebut. Mekanisme queue memastikan task `read_data` dan task `display` berjalan secara terkoordinasi tanpa konflik akses data.
