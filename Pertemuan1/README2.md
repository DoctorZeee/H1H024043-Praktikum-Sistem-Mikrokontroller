# Modul I – Percabangan dan Perulangan
## Percobaan 2A: Perulangan (for) – LED Running pada Arduino

**Mata Kuliah:** Sistem Tertanam dan Mikrokontroler  
**Program Studi:** Jurusan Informatika, Fakultas Teknik  
**Universitas:** Universitas Jenderal Soedirman  

---

## Tujuan

1. Memahami konsep perulangan `for` dalam pemrograman Arduino.
2. Mengimplementasikan perulangan untuk mengontrol beberapa LED secara berurutan (LED running).
3. Membuat program LED yang menyala bergantian antara kelompok kiri dan kelompok kanan.
4. Menghubungkan konsep perulangan dengan perilaku nyata pada sistem perangkat keras.

---

## Teori Singkat

### Perulangan `for` pada Arduino

Perulangan `for` digunakan untuk mengeksekusi blok kode secara berulang sebanyak jumlah yang telah ditentukan. Struktur umumnya adalah:

```cpp
for (inisialisasi; syarat; penambahan/pengurangan) {
    // aksi yang diulang
}
```

- **Inisialisasi** : menentukan nilai awal variabel penghitung.
- **Syarat**       : selama syarat ini bernilai `true`, perulangan terus berjalan.
- **Penambahan / Pengurangan** : memperbarui nilai variabel penghitung di setiap iterasi.

Pada percobaan LED running, perulangan `for` dimanfaatkan untuk mengaktifkan pin-pin digital secara bergantian, sehingga LED menyala satu per satu membentuk efek "berjalan".

---

## Jawaban Pertanyaan Praktikum

---

### Pertanyaan 1 – Gambar Rangkaian Schematic 5 LED Running

> Catatan: Modul ini menggunakan **6 LED** (pin 2–7) sesuai Tabel 1.1, namun pertanyaan menyebut "5 LED running". Jawaban berikut menggambarkan skematik **6 LED** sesuai konfigurasi aktual percobaan agar selaras dengan program dan wiring yang digunakan.

#### Diagram ASCII Rangkaian

```
Arduino Uno
 ___________
|           |
|      Pin 2|---[R 220Ω]---|>|--- GND   (LED 1 - paling kiri)
|      Pin 3|---[R 220Ω]---|>|--- GND   (LED 2)
|      Pin 4|---[R 220Ω]---|>|--- GND   (LED 3)
|      Pin 5|---[R 220Ω]---|>|--- GND   (LED 4)
|      Pin 6|---[R 220Ω]---|>|--- GND   (LED 5)
|      Pin 7|---[R 220Ω]---|>|--- GND   (LED 6 - paling kanan)
|       GND |----------------------------------------------- GND (rel negatif breadboard)
|___________|

Keterangan simbol:
  [R 220Ω]  = Resistor 220 Ohm (pembatas arus, mencegah LED terbakar)
  |>|        = LED (arah panah = arah arus: dari anoda ke katoda)
  ---        = Kabel jumper
```

#### Tabel Wiring Komponen

| No | Komponen | Pin Arduino | Keterangan           |
|----|----------|-------------|----------------------|
| 1  | LED 1    | Pin 2       | LED paling kiri      |
| 2  | LED 2    | Pin 3       |                      |
| 3  | LED 3    | Pin 4       |                      |
| 4  | LED 4    | Pin 5       |                      |
| 5  | LED 5    | Pin 6       |                      |
| 6  | LED 6    | Pin 7       | LED paling kanan     |
| 7  | GND      | GND         | Rel negatif bersama  |

#### Catatan Pemasangan Komponen

- **Resistor 220Ω** dipasang seri antara pin Arduino dan kaki **anoda** (+) LED, berfungsi membatasi arus agar LED tidak rusak.
- Kaki **katoda** (–) LED dihubungkan ke jalur **GND** pada breadboard.
- Semua kaki katoda LED dihubungkan ke satu jalur GND yang sama (rail negatif breadboard), lalu rail tersebut dihubungkan ke pin GND Arduino.

---

### Pertanyaan 2 – Bagaimana Program Membuat Efek LED Berjalan dari Kiri ke Kanan?

Efek LED berjalan dari kiri ke kanan dibuat menggunakan perulangan `for` dengan variabel yang **bertambah** dari nilai kecil ke nilai besar (pin 2 → pin 7).

**Potongan kode yang bertanggung jawab:**

```cpp
for (int ledPin = 2; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);   // nyalakan LED saat ini
    delay(timer);                 // tahan beberapa saat
    digitalWrite(ledPin, LOW);    // matikan LED saat ini
}
```

**Alur eksekusi (kiri → kanan):**

```
Iterasi 1 → ledPin = 2 → LED 1 (kiri) NYALA → tunggu → MATI
Iterasi 2 → ledPin = 3 → LED 2          NYALA → tunggu → MATI
Iterasi 3 → ledPin = 4 → LED 3          NYALA → tunggu → MATI
Iterasi 4 → ledPin = 5 → LED 4          NYALA → tunggu → MATI
Iterasi 5 → ledPin = 6 → LED 5          NYALA → tunggu → MATI
Iterasi 6 → ledPin = 7 → LED 6 (kanan) NYALA → tunggu → MATI
             ledPin = 8 → syarat (< 8) GAGAL → perulangan berhenti
```

Karena hanya satu LED yang menyala pada satu waktu dan berlangsung secara sangat cepat berurutan, mata manusia menangkapnya sebagai efek "cahaya berjalan" dari kiri ke kanan.

---

### Pertanyaan 3 – Bagaimana Program Membuat LED Kembali dari Kanan ke Kiri?

Efek balik dari kanan ke kiri dibuat dengan perulangan `for` kedua, namun dengan variabel yang **berkurang** dari nilai besar ke nilai kecil (pin 7 → pin 2). Ini dicapai dengan menggunakan operator dekremen `ledPin--`.

**Potongan kode yang bertanggung jawab:**

```cpp
for (int ledPin = 7; ledPin >= 2; ledPin--) {
    digitalWrite(ledPin, HIGH);   // nyalakan LED saat ini
    delay(timer);                 // tahan beberapa saat
    digitalWrite(ledPin, LOW);    // matikan LED saat ini
}
```

**Alur eksekusi (kanan → kiri):**

```
Iterasi 1 → ledPin = 7 → LED 6 (kanan) NYALA → tunggu → MATI
Iterasi 2 → ledPin = 6 → LED 5          NYALA → tunggu → MATI
Iterasi 3 → ledPin = 5 → LED 4          NYALA → tunggu → MATI
Iterasi 4 → ledPin = 4 → LED 3          NYALA → tunggu → MATI
Iterasi 5 → ledPin = 3 → LED 2          NYALA → tunggu → MATI
Iterasi 6 → ledPin = 2 → LED 1 (kiri)  NYALA → tunggu → MATI
             ledPin = 1 → syarat (>= 2) GAGAL → perulangan berhenti
```

Setelah perulangan kanan-ke-kiri selesai, fungsi `loop()` kembali ke awal dan perulangan kiri-ke-kanan dimulai lagi, menciptakan efek **bolak-balik** yang terus menerus.

---

## Program Modifikasi: 3 LED Kanan dan 3 LED Kiri Bergantian

### Deskripsi Program

Program ini membagi 6 LED menjadi dua kelompok:
- **Kelompok KIRI**  : LED 1, LED 2, LED 3 → Pin 2, 3, 4
- **Kelompok KANAN** : LED 4, LED 5, LED 6 → Pin 5, 6, 7

Kedua kelompok menyala secara **bergantian**: saat kelompok kiri menyala, kelompok kanan mati, dan sebaliknya. Pola ini berjalan terus-menerus tanpa henti selama Arduino aktif.

### Logika Perulangan yang Digunakan

Program menggunakan dua buah perulangan `for` di dalam `loop()`:
1. **Perulangan pertama** : menyalakan dan mematikan semua LED di kelompok **kiri** (pin 2–4) secara bersamaan, lalu menunggu.
2. **Perulangan kedua**   : menyalakan dan mematikan semua LED di kelompok **kanan** (pin 5–7) secara bersamaan, lalu menunggu.

Dengan menggunakan `for` untuk menyalakan semua pin dalam satu kelompok sebelum berpindah kelompok, efek "tiga LED kiri menyala sekaligus → tiga LED kanan menyala sekaligus" dapat dicapai secara efisien tanpa harus menulis `digitalWrite` sebanyak enam baris terpisah.

---

## Source Code Arduino

```cpp
// ============================================================
// Modul I – Percobaan 2A: Perulangan (Modifikasi)
// Program: 3 LED Kiri dan 3 LED Kanan Menyala Bergantian
// Universitas Jenderal Soedirman – Jurusan Informatika
// ============================================================

// Variabel untuk mengatur durasi nyala setiap kelompok LED (dalam ms)
int timer = 500;

// -------------------------------------------------------
// setup() → dijalankan SEKALI saat Arduino pertama aktif
// -------------------------------------------------------
void setup() {

  // Gunakan perulangan for untuk menginisialisasi semua pin (2–7) sebagai OUTPUT
  // Ini lebih efisien daripada menulis pinMode() sebanyak 6 kali
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }

}

// -------------------------------------------------------
// loop() → dijalankan TERUS-MENERUS selama Arduino aktif
// -------------------------------------------------------
void loop() {

  // -------------------------------------------------------
  // TAHAP 1: Nyalakan 3 LED KIRI (pin 2, 3, 4)
  // -------------------------------------------------------

  // Perulangan for untuk menyalakan semua LED kiri sekaligus
  // ledPin dimulai dari 2, berjalan sampai ledPin < 5 (yaitu pin 2, 3, 4)
  for (int ledPin = 2; ledPin < 5; ledPin++) {
    digitalWrite(ledPin, HIGH);   // Nyalakan LED pada pin saat ini
  }

  // Tahan posisi ini (3 LED kiri menyala) selama 'timer' milidetik
  delay(timer);

  // Perulangan for untuk mematikan semua LED kiri sekaligus
  for (int ledPin = 2; ledPin < 5; ledPin++) {
    digitalWrite(ledPin, LOW);    // Matikan LED pada pin saat ini
  }

  // -------------------------------------------------------
  // TAHAP 2: Nyalakan 3 LED KANAN (pin 5, 6, 7)
  // -------------------------------------------------------

  // Perulangan for untuk menyalakan semua LED kanan sekaligus
  // ledPin dimulai dari 5, berjalan sampai ledPin < 8 (yaitu pin 5, 6, 7)
  for (int ledPin = 5; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);   // Nyalakan LED pada pin saat ini
  }

  // Tahan posisi ini (3 LED kanan menyala) selama 'timer' milidetik
  delay(timer);

  // Perulangan for untuk mematikan semua LED kanan sekaligus
  for (int ledPin = 5; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, LOW);    // Matikan LED pada pin saat ini
  }

  // Setelah tahap 2 selesai, loop() kembali ke awal secara otomatis
  // → Pola kiri-kanan akan terus berulang tanpa henti

}
```

---

## Penjelasan Kode Per Bagian

### Bagian 1 – Deklarasi Variabel Global

```cpp
int timer = 500;
```

| Baris | Penjelasan |
|-------|-----------|
| `int timer = 500;` | Mendefinisikan variabel `timer` bernilai **500 ms** (0,5 detik). Nilai ini menentukan berapa lama setiap kelompok LED bertahan dalam kondisi menyala sebelum berpindah ke kelompok berikutnya. Nilai dapat diubah sesuai kebutuhan: semakin kecil = semakin cepat pergantian. |

---

### Bagian 2 – Fungsi `setup()`

```cpp
void setup() {
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }
}
```

| Baris | Penjelasan |
|-------|-----------|
| `void setup()` | Fungsi inisialisasi yang hanya berjalan **satu kali** ketika Arduino dinyalakan atau di-reset. |
| `for (int ledPin = 2; ledPin < 8; ledPin++)` | Perulangan yang mengiterasi nilai `ledPin` dari 2 hingga 7 (selama `ledPin < 8`). Setiap iterasi mewakili satu pin LED. |
| `pinMode(ledPin, OUTPUT);` | Mengonfigurasi pin saat ini sebagai **OUTPUT** sehingga Arduino dapat mengirimkan sinyal untuk menyalakan/mematikan LED. Tanpa ini, pin tidak akan berfungsi sebagaimana mestinya. |

---

### Bagian 3 – Tahap 1: Nyalakan 3 LED Kiri

```cpp
for (int ledPin = 2; ledPin < 5; ledPin++) {
    digitalWrite(ledPin, HIGH);
}
delay(timer);
for (int ledPin = 2; ledPin < 5; ledPin++) {
    digitalWrite(ledPin, LOW);
}
```

| Baris | Penjelasan |
|-------|-----------|
| `for (int ledPin = 2; ledPin < 5; ledPin++)` | Perulangan untuk pin 2, 3, dan 4 (LED kiri). Batas atas `ledPin < 5` memastikan hanya 3 pin pertama yang diproses. |
| `digitalWrite(ledPin, HIGH);` | Mengirim sinyal HIGH (5V) ke pin saat ini → LED **menyala**. Dilakukan secara berurutan sangat cepat sehingga ketiga LED tampak menyala **bersamaan**. |
| `delay(timer);` | Menghentikan program selama 500 ms → ketiga LED kiri tetap **menyala** selama setengah detik. |
| `for (...) { digitalWrite(ledPin, LOW); }` | Perulangan kedua untuk mematikan kembali ketiga LED kiri sekaligus sebelum menyalakan kelompok kanan. |

---

### Bagian 4 – Tahap 2: Nyalakan 3 LED Kanan

```cpp
for (int ledPin = 5; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);
}
delay(timer);
for (int ledPin = 5; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, LOW);
}
```

| Baris | Penjelasan |
|-------|-----------|
| `for (int ledPin = 5; ledPin < 8; ledPin++)` | Perulangan untuk pin 5, 6, dan 7 (LED kanan). Nilai awal `ledPin = 5` dan batas `ledPin < 8` memastikan hanya 3 pin terakhir yang diproses. |
| `digitalWrite(ledPin, HIGH);` | Menyalakan LED kanan satu per satu dengan sangat cepat → tampak menyala **bersamaan**. |
| `delay(timer);` | Menghentikan program selama 500 ms → ketiga LED kanan tetap **menyala** selama setengah detik. |
| `for (...) { digitalWrite(ledPin, LOW); }` | Mematikan kembali ketiga LED kanan. Setelah ini, `loop()` kembali ke awal dan siklus dimulai ulang. |

---

### Ilustrasi Alur Pola Pergantian

```
Waktu 0.0 s → [LED1 LED2 LED3] NYALA  |  [LED4 LED5 LED6] MATI
Waktu 0.5 s → [LED1 LED2 LED3] MATI   |  [LED4 LED5 LED6] NYALA
Waktu 1.0 s → [LED1 LED2 LED3] NYALA  |  [LED4 LED5 LED6] MATI
Waktu 1.5 s → [LED1 LED2 LED3] MATI   |  [LED4 LED5 LED6] NYALA
... (pola terus berulang tanpa henti)
```

---

## Kesimpulan

Dari percobaan dan modifikasi program ini, dapat ditarik beberapa kesimpulan:

1. **Perulangan `for` kiri ke kanan** menggunakan variabel yang bertambah (`ledPin++`) dari pin bernilai kecil ke besar, sehingga LED menyala secara berurutan dari kiri ke kanan.

2. **Perulangan `for` kanan ke kiri** menggunakan variabel yang berkurang (`ledPin--`) dari pin bernilai besar ke kecil, sehingga LED menyala secara berurutan dari kanan ke kiri.

3. **Penggunaan `for` dalam `setup()`** untuk inisialisasi pin menunjukkan efisiensi penulisan kode — cukup satu blok untuk mengonfigurasi semua pin sekaligus.

4. **Pola 3 LED kiri dan 3 LED kanan bergantian** menunjukkan bahwa dengan memodifikasi batas awal dan akhir variabel `ledPin` pada `for`, kita dapat mengontrol kelompok LED secara selektif dan terstruktur.

5. Perulangan `for` adalah salah satu fondasi penting dalam pemrograman sistem tertanam karena memungkinkan pengendalian banyak perangkat (seperti LED, motor, atau sensor) secara sistematis dan efisien hanya dengan beberapa baris kode.

---

*Laporan ini dibuat sebagai bagian dari Tugas Pendahuluan dan Pertanyaan Praktikum Modul I – Sistem Tertanam dan Mikrokontroler, Universitas Jenderal Soedirman.*