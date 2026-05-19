Nama: Dimas Rafif Zaidan  
NIM: H1H024043  
Shift Awal: B  
Shift Akhir: B  

---

# Praktikum Modul 6 — Interrupt dan Timer

---

# Percobaan 6A — External Interrupt

## Foto Rangkaian

### Foto Rangkaian Fisik
<!-- Tambahkan foto rangkaian di sini -->
<img width="2160" height="2880" alt="IMG_4371" src="https://github.com/user-attachments/assets/cbae63f3-428b-4b6f-bca3-7047b8728dc1" />

### Video Demo
<img width="480" height="854" alt="IMG_4373" src="https://github.com/user-attachments/assets/b65aae90-81fc-49cb-b9ca-cd27407aa3cd" />

---

## 6.5.4 Pertanyaan Praktikum

### 1. Jelaskan proses bagaimana tombol dapat mengubah kondisi LED menggunakan interrupt!

Ketika tombol ditekan, tegangan pada pin 2 berubah dari HIGH ke LOW (FALLING edge) karena pin dikonfigurasi dengan `INPUT_PULLUP`. Perubahan sinyal ini memicu interrupt sehingga CPU menghentikan sementara program utama dan langsung menjalankan ISR `tombolInterrupt()`. Di dalam ISR, variabel `ledState` di-toggle (dibalik nilainya). Setelah ISR selesai, program utama dilanjutkan kembali dan `digitalWrite(13, ledState)` di `loop()` memperbarui kondisi LED sesuai nilai `ledState` yang baru.

---

### 2. Apa fungsi `attachInterrupt()` pada program tersebut?

`attachInterrupt()` berfungsi mendaftarkan (menghubungkan) sebuah fungsi ISR ke pin interrupt tertentu beserta mode pemicunya. Dengan pemanggilan:

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, FALLING);
```

Arduino diperintahkan untuk menjalankan fungsi `tombolInterrupt()` secara otomatis setiap kali pin 2 mengalami transisi dari HIGH ke LOW. Tanpa `attachInterrupt()`, Arduino tidak akan tahu fungsi mana yang harus dijalankan ketika interrupt terjadi.

---

### 3. Mengapa pada ISR tidak disarankan menggunakan `delay()` dan `Serial.print()`?

ISR harus selesai secepat mungkin karena selama ISR berjalan, program utama dan interrupt lain tertahan. Dua alasan utama:

- **`delay()`** menggunakan timer internal yang tidak dapat berjalan di dalam ISR, sehingga menyebabkan program hang atau perilaku tidak terduga.
- **`Serial.print()`** menggunakan buffer dan mekanisme interrupt tersendiri (UART interrupt). Memanggilnya dari dalam ISR dapat menyebabkan deadlock karena interrupt UART tidak dapat berjalan saat ISR sedang aktif.

---

### 4. Apa fungsi keyword `volatile` pada variabel `ledState`?

Keyword `volatile` memberitahu compiler bahwa nilai variabel `ledState` dapat berubah kapan saja di luar alur normal program (yaitu di dalam ISR), sehingga compiler tidak boleh melakukan optimasi seperti menyimpan nilai variabel di register CPU. Tanpa `volatile`, compiler mungkin mengabaikan perubahan yang terjadi di ISR dan `loop()` akan terus membaca nilai lama dari cache, menyebabkan LED tidak merespons tombol.

---

### 5. Modifikasi: Mode Interrupt RISING, CHANGE, dan LOW

**Penjelasan perbedaan mode interrupt:**

| Mode | Pemicu | Perilaku LED |
|------|--------|-------------|
| `FALLING` | HIGH → LOW (tombol ditekan) | Toggle saat tombol ditekan |
| `RISING` | LOW → HIGH (tombol dilepas) | Toggle saat tombol dilepas |
| `CHANGE` | HIGH → LOW atau LOW → HIGH | Toggle dua kali per satu klik (tekan & lepas) |
| `LOW` | Selama pin bernilai LOW | Toggle terus-menerus selama tombol ditekan (bahaya: ISR dipanggil berulang kali) |

**Source code — Mode RISING:**

```cpp
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle LED saat tombol dilepas
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  // Interrupt dipicu saat sinyal naik dari LOW ke HIGH (tombol dilepas)
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);
}

void loop() {
  digitalWrite(13, ledState);
}
```

**Source code — Mode CHANGE:**

```cpp
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle setiap ada perubahan sinyal
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  // Interrupt dipicu setiap ada perubahan nilai pin (tekan maupun lepas)
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);
}

void loop() {
  digitalWrite(13, ledState);
}
```

**Analisis:**
- **RISING**: LED berubah sesaat tombol dilepaskan. Cocok jika ingin aksi terjadi saat "release".
- **CHANGE**: LED toggle dua kali per klik (sekali saat tekan, sekali saat lepas), sehingga setelah satu klik penuh LED kembali ke kondisi semula.
- **LOW**: ISR terpanggil berulang kali selama tombol ditahan sehingga LED berfluktuasi sangat cepat — sebaiknya dihindari untuk toggling LED.

---
---

# Percobaan 6B — Timer Menggunakan millis()

## Foto Rangkaian

### Foto Rangkaian Fisik
<!-- Tambahkan foto rangkaian di sini -->
<img width="1600" height="1200" alt="594549490-f9ac671f-79a9-41ba-97b6-9fdd6027005f" src="https://github.com/user-attachments/assets/765e436d-0466-40f2-89e9-5f983d49dcf2" />

---

## 6.6.4 Pertanyaan Praktikum

### 1. Jelaskan bagaimana fungsi `millis()` bekerja pada program tersebut!

`millis()` mengembalikan jumlah milidetik sejak Arduino dinyalakan. Pada setiap iterasi `loop()`, nilai `millis()` saat ini disimpan ke `currentMillis`. Kemudian dihitung selisih antara `currentMillis` dan `previousMillis` (waktu terakhir LED berubah). Jika selisih tersebut sudah mencapai atau melebihi nilai `interval` (1000 ms), maka kondisi LED di-toggle dan `previousMillis` diperbarui menjadi waktu saat ini sebagai titik acuan baru. Pola ini menciptakan efek "alarm" non-blocking tanpa menghentikan eksekusi program.

---

### 2. Apa perbedaan utama antara `delay()` dan `millis()`?

| Aspek | `delay()` | `millis()` |
|-------|-----------|-----------|
| Cara kerja | Memblokir CPU selama durasi yang ditentukan | Hanya mengecek selisih waktu, CPU tetap bebas |
| Program lain | Tidak dapat berjalan selama delay | Dapat berjalan bersamaan |
| Multitasking | Tidak mendukung | Mendukung |
| Tipe | Blocking | Non-blocking |

---

### 3. Mengapa metode `millis()` disebut non-blocking?

`millis()` disebut non-blocking karena ia tidak menghentikan eksekusi program. Alih-alih "menunggu" seperti `delay()`, program terus berjalan dan hanya memeriksa apakah sudah cukup waktu berlalu setiap kali `loop()` dieksekusi. CPU bebas mengerjakan tugas lain (membaca sensor, mengecek tombol, dll.) di antara pengecekan tersebut, sehingga sistem tetap responsif.

---

### 4. Modifikasi: Dua LED dengan Interval Berbeda

## Foto Rangkaian Modifikasi

### Foto Rangkaian Fisik
<!-- Tambahkan foto rangkaian modifikasi di sini -->
<img width="2160" height="2880" alt="IMG_4377" src="https://github.com/user-attachments/assets/7299f9b7-621a-43ec-997a-b356417f80c4" />

### Demo Rangkaian
<img width="480" height="854" alt="IMG_4374" src="https://github.com/user-attachments/assets/d7da858b-afe1-4258-aca4-fa47c03281ed" />


```cpp
#include <Arduino.h>

// ====================== PIN SETUP ======================
const int LED1 = 13; // LED pertama
const int LED2 = 12; // LED kedua

// ====================== STATE LED ======================
bool led1State = false; // status LED 1 saat ini (OFF)
bool led2State = false; // status LED 2 saat ini (OFF)

// ====================== TIMING ======================
unsigned long previousMillisLED1 = 0;        // waktu terakhir LED 1 berubah
unsigned long previousMillisLED2 = 0;        // waktu terakhir LED 2 berubah
const unsigned long intervalLED1 = 1000;     // interval LED 1: 1 detik
const unsigned long intervalLED2 = 500;      // interval LED 2: 500 ms

void setup() {
  pinMode(LED1, OUTPUT); // konfigurasi LED 1 sebagai output
  pinMode(LED2, OUTPUT); // konfigurasi LED 2 sebagai output
}

void loop() {
  unsigned long currentMillis = millis(); // ambil waktu saat ini

  // ====================== LED 1 (1 detik) ======================
  if (currentMillis - previousMillisLED1 >= intervalLED1) {
    previousMillisLED1 = currentMillis; // update waktu terakhir
    led1State = !led1State;             // toggle LED 1
    digitalWrite(LED1, led1State);      // tulis ke pin LED 1
  }

  // ====================== LED 2 (500 ms) ======================
  if (currentMillis - previousMillisLED2 >= intervalLED2) {
    previousMillisLED2 = currentMillis; // update waktu terakhir
    led2State = !led2State;             // toggle LED 2
    digitalWrite(LED2, led2State);      // tulis ke pin LED 2
  }
}
```

**Penjelasan Program:**

| Bagian | Penjelasan |
|--------|------------|
| `LED1 = 13` / `LED2 = 12` | LED 1 terpasang di pin 13, LED 2 di pin 12 |
| `led1State` / `led2State` | Menyimpan kondisi ON/OFF masing-masing LED |
| `previousMillisLED1` / `previousMillisLED2` | Menyimpan waktu terakhir masing-masing LED berubah kondisi |
| `intervalLED1 = 1000` | LED 1 berkedip setiap 1000 ms (1 detik) |
| `intervalLED2 = 500` | LED 2 berkedip setiap 500 ms (0,5 detik) |
| `currentMillis - previousMillisLEDx >= intervalLEDx` | Mengecek apakah waktu yang berlalu sudah memenuhi interval |
| `ledxState = !ledxState` | Men-toggle kondisi LED antara ON dan OFF |
| `digitalWrite(LEDx, ledxState)` | Menerapkan kondisi baru ke pin LED |

**Hasil:**  
LED 1 (pin 13) berkedip setiap 1 detik dan LED 2 (pin 12) berkedip dua kali lebih cepat setiap 500 ms. Keduanya berjalan bersamaan tanpa saling mengganggu karena masing-masing memiliki variabel timer yang independen dan tidak ada `delay()` yang memblokir program.

---
---

# Pertanyaan Umum (6.7)

### 1. Keuntungan interrupt dibanding polling

| Aspek | Polling | Interrupt |
|-------|---------|-----------|
| Cara kerja | CPU terus-menerus memeriksa kondisi input | CPU hanya merespons saat event benar-benar terjadi |
| Efisiensi CPU | Boros, CPU sibuk meski tidak ada event | Efisien, CPU bebas mengerjakan tugas lain |
| Waktu respons | Bergantung pada kecepatan loop | Sangat cepat, hampir instan |
| Kompleksitas | Lebih sederhana | Sedikit lebih kompleks (perlu ISR) |

---

### 2. Mengapa timer penting dalam sistem embedded dan real-time?

Timer memungkinkan sistem melakukan aksi pada interval waktu yang tepat dan terukur tanpa memblokir CPU. Dalam sistem real-time, ketepatan waktu adalah kunci — misalnya pembacaan sensor periodik, pembangkitan sinyal PWM, atau penjadwalan task. Tanpa timer, satu-satunya cara mengatur waktu adalah `delay()` yang membekukan seluruh sistem.

---

### 3. Alur kerja jika interrupt dan timer digabung

1. Program utama berjalan dan timer (`millis()`) terus berjalan di background.
2. Jika event eksternal terjadi (tombol ditekan), interrupt memicu ISR untuk menangani event tersebut secara instan.
3. Setelah ISR selesai, program utama dilanjutkan.
4. Di dalam `loop()`, timer terus diperiksa untuk menjalankan task periodik (seperti kedip LED).
5. Kedua mekanisme bekerja secara independen — interrupt merespons event tidak terduga, timer mengelola jadwal berkala.

---

### 4. Apa yang terjadi jika ISR terlalu panjang atau kompleks?

- **Program utama tertahan** lebih lama dari seharusnya, menyebabkan sistem terasa tidak responsif.
- **Interrupt lain bisa terlewat** karena interrupt baru tidak dapat diproses selama ISR berjalan (pada Arduino, interrupt dinonaktifkan secara default selama ISR).
- **`millis()` berhenti bertambah** selama ISR berjalan karena `millis()` bergantung pada Timer0 interrupt yang juga ikut tertahan.
- Pada kasus ekstrem dapat menyebabkan **watchdog timeout** atau crash sistem.
