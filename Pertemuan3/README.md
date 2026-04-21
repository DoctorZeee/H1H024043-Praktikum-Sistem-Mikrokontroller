# README — Percobaan 3A: Komunikasi Serial (UART)
### Modul 3: Protokol Komunikasi | Sistem Tertanam dan Mikrokontroler
**Universitas Jenderal Soedirman — Jurusan Informatika, Fakultas Teknik**

---

## Daftar Isi
1. [Tujuan Percobaan](#1-tujuan-percobaan)
2. [Alur Data: Keyboard → LED (Step-by-Step)](#2-alur-data-keyboard--led-step-by-step)
3. [Penjelasan Serial.available()](#3-penjelasan-serialavailable)
4. [Kode Program Modifikasi (dengan Blink Input '2')](#4-kode-program-modifikasi-dengan-blink-input-2)
5. [Perbandingan delay() vs millis()](#5-perbandingan-delay-vs-millis)
6. [Referensi](#6-referensi)

---

## 1. Tujuan Percobaan

Percobaan ini bertujuan untuk:
- Memahami cara kerja komunikasi serial UART pada Arduino Uno
- Mengendalikan LED melalui perintah dari Serial Monitor (keyboard komputer)
- Memahami perbedaan pendekatan `delay()` dan `millis()` dalam sistem kontrol real-time

---

## 2. Alur Data: Keyboard → LED (Step-by-Step)

Proses dari penekanan tombol keyboard di Serial Monitor hingga LED menyala atau mati melibatkan beberapa lapisan komunikasi. Berikut adalah penjelasan alurnya secara lengkap:

```
┌─────────────────────────────────────────────────────────────────────┐
│                        ALUR DATA UART                               │
│                                                                     │
│  [KEYBOARD]                                                         │
│      │  Pengguna mengetik '1', '0', atau '2'                        │
│      ▼                                                              │
│  [SERIAL MONITOR (Arduino IDE)]                                     │
│      │  Karakter dikemas menjadi frame UART:                        │
│      │  [START BIT][DATA 8 BIT][STOP BIT] @ 9600 bps               │
│      ▼                                                              │
│  [USB → PORT COM / SERIAL]                                          │
│      │  Data dikirim melalui kabel USB ke chip CH340/ATmega16U2     │
│      ▼                                                              │
│  [PIN RX ARDUINO UNO]                                               │
│      │  Chip UART hardware Arduino menerima bit per bit             │
│      │  dan merakitnya menjadi 1 byte karakter                      │
│      ▼                                                              │
│  [BUFFER SERIAL (64 byte)]                                          │
│      │  Karakter disimpan sementara di buffer RX                    │
│      │  Serial.available() > 0 → ada data di buffer                 │
│      ▼                                                              │
│  [PROGRAM ARDUINO (loop())]                                         │
│      │  Serial.read() mengambil 1 karakter dari buffer              │
│      │  if (data == '1') → digitalWrite(HIGH)                       │
│      │  if (data == '0') → digitalWrite(LOW)                        │
│      │  if (data == '2') → mode berkedip aktif                      │
│      ▼                                                              │
│  [PIN DIGITAL 8]                                                    │
│      │  Tegangan berubah: HIGH (5V) atau LOW (0V)                   │
│      ▼                                                              │
│  [LED]                                                              │
│      Arus mengalir → LED MENYALA / MATI                             │
└─────────────────────────────────────────────────────────────────────┘
```

### Penjelasan Detail Setiap Tahap:

**Tahap 1 — Input dari Keyboard:**
Pengguna mengetikkan karakter (misalnya `'1'`) di Serial Monitor Arduino IDE dan menekan Enter. Karakter ini memiliki nilai ASCII tertentu (karakter `'1'` = ASCII 49).

**Tahap 2 — Enkapsulasi Frame UART:**
Arduino IDE mengemas karakter tersebut ke dalam format frame UART:
- **Start bit** (1 bit, logika LOW): menandai awal transmisi
- **Data bit** (8 bit): isi karakter dalam format biner (contoh: `'1'` = `0b00110001`)
- **Stop bit** (1 bit, logika HIGH): menandai akhir frame
- Kecepatan transmisi: **9600 bps** (baud rate yang disepakati kedua pihak)

**Tahap 3 — Transmisi via USB:**
Data dikirim melalui kabel USB. Chip konverter USB-to-Serial (misalnya CH340G pada Arduino clone, atau ATmega16U2 pada Arduino original) mengubah sinyal USB menjadi sinyal TTL UART dan mengirimkannya ke pin RX (pin 0) mikrokontroler ATmega328P.

**Tahap 4 — Penerimaan oleh Hardware UART Arduino:**
Modul UART hardware pada ATmega328P membaca sinyal bit per bit berdasarkan baud rate yang telah di-set dengan `Serial.begin(9600)`. Setelah 8 bit data terkumpul, karakter dirakit kembali dan disimpan di **buffer penerimaan (RX buffer)** berukuran 64 byte.

**Tahap 5 — Pemrosesan oleh Program:**
Fungsi `Serial.available()` mengecek apakah ada karakter di buffer. Jika ada, `Serial.read()` mengambil karakter tersebut, lalu program memutuskan aksi berdasarkan nilai karakter.

**Tahap 6 — Kontrol LED:**
Fungsi `digitalWrite()` mengatur tegangan pada pin digital sesuai perintah, sehingga LED menyala atau mati.

---

## 3. Penjelasan Serial.available()

### Apa itu Serial.available()?

`Serial.available()` adalah fungsi yang **mengembalikan jumlah byte yang tersedia untuk dibaca dari buffer serial**. Nilai yang dikembalikan berupa integer (0 jika tidak ada data, atau jumlah byte yang tersedia di buffer).

```arduino
// Contoh penggunaan
if (Serial.available() > 0) {
    char data = Serial.read();  // Baru membaca jika ada data
}
```

### Mengapa Serial.available() WAJIB Digunakan Sebelum Serial.read()?

| Kondisi | Dengan `Serial.available()` | Tanpa `Serial.available()` |
|---|---|---|
| **Ada data di buffer** | Baca dan proses dengan benar | Baca dan proses (kebetulan benar) |
| **Tidak ada data** | Lewati blok `if`, program berjalan normal | `Serial.read()` mengembalikan **-1** |
| **Hasil jika -1 diproses** | — | Karakter -1 tidak cocok dengan `'0'`, `'1'`, atau `'2'`, sehingga memicu blok `else` berulang kali dan menampilkan **"Perintah tidak dikenal"** secara terus-menerus |

### Dampak Jika Serial.available() Dihilangkan:

1. **Spam output:** Program akan terus-menerus mencetak `"Perintah tidak dikenal"` ke Serial Monitor karena `Serial.read()` selalu dipanggil meski buffer kosong, dan mengembalikan nilai -1 yang tidak dikenali.
2. **Pemborosan sumber daya CPU:** Mikrokontroler terus-menerus memanggil `Serial.read()` tanpa tujuan di setiap iterasi `loop()`.
3. **Perilaku program tidak terprediksi:** Pada beberapa kondisi, program mungkin bereaksi terhadap karakter -1 dengan cara yang tidak diinginkan.

---

## 4. Kode Program Modifikasi (dengan Blink Input '2')

### Deskripsi Modifikasi

Program dimodifikasi sehingga:
- Input `'1'` → LED **menyala** statis
- Input `'0'` → LED **mati**
- Input `'2'` → LED **berkedip terus-menerus** (non-blocking) hingga ada perintah baru

Pendekatan yang digunakan adalah **`millis()`** (bukan `delay()`) agar sistem tetap responsif menerima input baru saat LED sedang berkedip.

### Kode Program Lengkap

```cpp
// ============================================================
//  File    : modul3_serialmonitor_modifikasi.ino
//  Modul   : 3A - Komunikasi Serial UART (Modifikasi)
//  Deskripsi: Kontrol LED via Serial Monitor
//             '1' = ON, '0' = OFF, '2' = BLINK (non-blocking)
// ============================================================

// --- Definisi Pin ---
const int PIN_LED = 8;          // Pin digital tempat LED terhubung (sesuai Tabel 3.1)

// --- Variabel Status LED ---
bool modeBlink = false;         // Flag: true jika LED sedang dalam mode berkedip

// --- Variabel Timing untuk millis() ---
unsigned long waktuSebelumnya = 0;   // Menyimpan waktu terakhir LED berubah status
const long intervalBlink = 500;      // Interval kedip: 500ms nyala, 500ms mati
bool statusLED = false;              // Status LED saat ini: true = nyala, false = mati

// ============================================================
//  SETUP — dijalankan sekali saat Arduino dinyalakan/reset
// ============================================================
void setup() {
    Serial.begin(9600);
    // Inisialisasi komunikasi serial dengan baud rate 9600 bps.
    // Angka 9600 harus sama dengan pengaturan baud rate di Serial Monitor.

    Serial.println("=== Kontrol LED via Serial Monitor ===");
    // Tampilkan pesan sambutan ke Serial Monitor saat program dimulai.

    Serial.println("Perintah: '1' = LED ON | '0' = LED OFF | '2' = LED BLINK");
    // Tampilkan panduan penggunaan agar pengguna tahu perintah yang valid.

    pinMode(PIN_LED, OUTPUT);
    // Konfigurasikan pin 8 sebagai OUTPUT agar bisa mengirim tegangan ke LED.

    digitalWrite(PIN_LED, LOW);
    // Pastikan LED dalam keadaan mati saat program pertama kali berjalan.
}

// ============================================================
//  LOOP — dijalankan berulang-ulang secara terus-menerus
// ============================================================
void loop() {

    // ----------------------------------------------------------
    //  BAGIAN 1: Baca Input dari Serial Monitor (Keyboard)
    // ----------------------------------------------------------
    if (Serial.available() > 0) {
        // Cek apakah ada data yang menunggu di buffer penerimaan serial.
        // Serial.available() mengembalikan jumlah byte yang tersedia.
        // Blok ini hanya dieksekusi jika ada karakter yang dikirim dari keyboard.

        char data = Serial.read();
        // Ambil 1 karakter dari buffer serial dan simpan ke variabel 'data'.
        // Serial.read() mengurangi jumlah byte di buffer sebesar 1.

        // ----------------------------------------------------------
        //  Proses Perintah Berdasarkan Karakter yang Diterima
        // ----------------------------------------------------------
        if (data == '1') {
            // Jika karakter yang diterima adalah '1' (ASCII 49):
            modeBlink = false;           // Matikan mode berkedip
            statusLED = true;            // Update status LED menjadi nyala
            digitalWrite(PIN_LED, HIGH); // Nyalakan LED (berikan tegangan 5V ke pin 8)
            Serial.println(">> LED ON");  // Konfirmasi ke Serial Monitor

        } else if (data == '0') {
            // Jika karakter yang diterima adalah '0' (ASCII 48):
            modeBlink = false;           // Matikan mode berkedip
            statusLED = false;           // Update status LED menjadi mati
            digitalWrite(PIN_LED, LOW);  // Matikan LED (berikan tegangan 0V ke pin 8)
            Serial.println(">> LED OFF"); // Konfirmasi ke Serial Monitor

        } else if (data == '2') {
            // Jika karakter yang diterima adalah '2' (ASCII 50):
            modeBlink = true;            // Aktifkan mode berkedip
            waktuSebelumnya = millis();  // Reset timer agar kedip dimulai dari awal
            Serial.println(">> LED BLINK (kirim '0' atau '1' untuk menghentikan)");
            // Konfirmasi ke Serial Monitor bahwa mode blink aktif.

        } else if (data != '\n' && data != '\r') {
            // Karakter '\n' (newline) dan '\r' (carriage return) adalah karakter
            // yang otomatis dikirim saat pengguna menekan Enter di Serial Monitor.
            // Karakter ini bukan perintah, jadi diabaikan.
            // Selain ketiganya, tampilkan pesan error.
            Serial.print(">> Perintah tidak dikenal: '");
            Serial.print(data);
            Serial.println("'  (gunakan: 0, 1, atau 2)");
        }
    }

    // ----------------------------------------------------------
    //  BAGIAN 2: Eksekusi Mode Blink (Non-Blocking dengan millis())
    // ----------------------------------------------------------
    if (modeBlink) {
        // Blok ini hanya aktif jika modeBlink == true (pengguna mengirim '2').

        unsigned long waktuSekarang = millis();
        // millis() mengembalikan jumlah milidetik sejak Arduino dinyalakan.
        // Nilai ini terus bertambah dan tidak pernah berhenti.

        if (waktuSekarang - waktuSebelumnya >= intervalBlink) {
            // Cek apakah sudah waktunya untuk mengganti status LED.
            // Selisih waktu sekarang dengan waktu terakhir perubahan >= 500ms?

            waktuSebelumnya = waktuSekarang;
            // Simpan waktu sekarang sebagai referensi untuk perhitungan berikutnya.

            statusLED = !statusLED;
            // Toggle status LED: jika true menjadi false, jika false menjadi true.
            // Operator '!' adalah operator logika NOT (negasi).

            digitalWrite(PIN_LED, statusLED ? HIGH : LOW);
            // Tulis ke pin LED sesuai status:
            // Jika statusLED == true  → HIGH (LED nyala)
            // Jika statusLED == false → LOW  (LED mati)
            // Operator ternary: (kondisi) ? nilai_jika_true : nilai_jika_false
        }
        // CATATAN PENTING: Tidak ada delay() di sini!
        // Program langsung kembali ke awal loop() dan siap menerima input baru.
        // Inilah keunggulan millis(): LED berkedip TANPA memblokir Serial.read().
    }
}
```

### Diagram Alir Program

```
START
  │
  ▼
setup():
  Serial.begin(9600)
  pinMode(8, OUTPUT)
  digitalWrite(8, LOW)
  │
  ▼
loop() ◄─────────────────────────────────────────────────┐
  │                                                       │
  ├─► Serial.available() > 0? ──── TIDAK ──────────────► │ (skip baca)
  │         │                                             │
  │        YA                                             │
  │         │                                             │
  │         ▼                                             │
  │    data = Serial.read()                               │
  │         │                                             │
  │    ┌────┴──────────────────────────────┐              │
  │    │ data == '1'? → LED ON, blink=OFF  │              │
  │    │ data == '0'? → LED OFF, blink=OFF │              │
  │    │ data == '2'? → blink=ON, reset timer│            │
  │    │ lainnya?    → Error message       │              │
  │    └────────────────────────────────────┘             │
  │                                                       │
  ├─► modeBlink == true?                                  │
  │         │                                             │
  │        YA                                             │
  │         │                                             │
  │         ▼                                             │
  │   millis() - waktuSebelumnya >= 500ms?                │
  │         │          │                                  │
  │        YA         TIDAK                               │
  │         │                                             │
  │   Toggle LED ──────────────────────────────────────── ┘
  │
  └──────────────────────────────────────────────────────► Ulangi loop()
```

---

## 5. Perbandingan delay() vs millis()

### Cara Kerja delay()

`delay(ms)` menghentikan eksekusi program selama `ms` milidetik. Selama jeda ini, **tidak ada kode lain yang dapat berjalan** — termasuk membaca input dari Serial Monitor.

```arduino
// Implementasi blink dengan delay() — TIDAK DISARANKAN
void loop() {
    if (modeBlink) {
        digitalWrite(PIN_LED, HIGH);
        delay(500);               // ← Program BEKU selama 500ms di sini
        digitalWrite(PIN_LED, LOW);
        delay(500);               // ← Program BEKU lagi selama 500ms di sini
        // Selama total 1 detik ini, Serial.read() TIDAK bisa berjalan!
        // Jika pengguna mengirim '0' saat ini, perintah DIABAIKAN atau TERTUNDA.
    }
    // Serial.read() baru bisa jalan setelah delay selesai
    if (Serial.available() > 0) { ... }
}
```

### Cara Kerja millis()

`millis()` hanya membaca waktu saat ini tanpa menghentikan apapun. Program terus berjalan, dan pengecekan waktu dilakukan di setiap iterasi `loop()`.

```arduino
// Implementasi blink dengan millis() — DISARANKAN
void loop() {
    // Cek input — SELALU berjalan di setiap iterasi
    if (Serial.available() > 0) { ... }

    // Cek waktu untuk blink — TIDAK memblokir apapun
    if (modeBlink) {
        if (millis() - waktuSebelumnya >= 500) {
            waktuSebelumnya = millis();
            statusLED = !statusLED;
            digitalWrite(PIN_LED, statusLED);
        }
    }
    // loop() langsung kembali ke awal → sangat cepat, responsif
}
```

### Tabel Perbandingan Lengkap

| Aspek | `delay()` | `millis()` |
|---|---|---|
| **Cara kerja** | Memblokir (blocking) — program berhenti | Non-blocking — program terus berjalan |
| **Responsivitas input** | ❌ Buruk — input bisa terlewat saat delay | ✅ Baik — input selalu dapat dibaca |
| **Kemudahan kode** | ✅ Mudah ditulis dan dipahami | ⚠️ Sedikit lebih kompleks |
| **Multi-tasking** | ❌ Tidak bisa menjalankan tugas lain | ✅ Bisa menjalankan banyak tugas sekaligus |
| **Cocok untuk** | Prototipe sederhana, satu tugas saja | Sistem yang butuh responsivitas tinggi |
| **Dampak ke Serial** | Buffer serial bisa penuh/terlewat | Serial selalu terbaca tepat waktu |
| **Penggunaan di sistem nyata** | Jarang — hanya untuk debugging | ✅ Standar industri untuk embedded system |

### Kesimpulan Pemilihan Metode

Untuk percobaan ini, **`millis()` adalah pilihan yang tepat** karena:

1. **Sistem harus responsif:** LED harus bisa dihentikan kedipannya kapan saja dengan mengirim `'0'` atau `'1'`. Dengan `delay()`, perintah baru akan diabaikan selama LED sedang dalam fase delay.
2. **Prinsip embedded system:** Sistem tertanam yang baik tidak boleh "membuang waktu" dengan `delay()` karena dapat menyebabkan kehilangan event atau input penting.
3. **Skalabilitas:** Jika ke depannya ditambahkan sensor atau komponen lain, pendekatan `millis()` memungkinkan semua tugas berjalan secara bersamaan (kooperatif multitasking).

---

## 6. Referensi

- [1] Arduino LLC, "Serial - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/communication/serial/
- [2] Arduino LLC, "millis() - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/time/millis/
- [3] Arduino LLC, "Blink Without Delay," https://www.arduino.cc/en/Tutorial/BuiltInExamples/BlinkWithoutDelay
- [4] Arduino LLC, "Serial.available() - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/communication/serial/available/
- [5] Last Minute Engineers, "UART Communication Protocol - How It Works," https://lastminuteengineers.com/uart-communication-protocol-arduino/

---

# README — Percobaan 3B: Inter-Integrated Circuit (I2C)
### Modul 3: Protokol Komunikasi | Sistem Tertanam dan Mikrokontroler
**Universitas Jenderal Soedirman — Jurusan Informatika, Fakultas Teknik**

---

## Daftar Isi
1. [Tujuan Percobaan](#1-tujuan-percobaan)
2. [Cara Kerja Komunikasi I2C antara Arduino dan LCD](#2-cara-kerja-komunikasi-i2c-antara-arduino-dan-lcd)
3. [Fungsi Pin Potensiometer dan Efek Jika Tertukar](#3-fungsi-pin-potensiometer-dan-efek-jika-tertukar)
4. [Kode Program Modifikasi (UART + I2C)](#4-kode-program-modifikasi-uart--i2c)
5. [Tabel Hasil Pengamatan](#5-tabel-hasil-pengamatan)
6. [Referensi](#6-referensi)

---

## 1. Tujuan Percobaan

Percobaan ini bertujuan untuk:
- Memahami cara kerja protokol komunikasi I2C antara Arduino Uno dan modul LCD I2C
- Membaca nilai analog dari potensiometer menggunakan ADC (Analog-to-Digital Converter) Arduino
- Menampilkan data ADC dalam format bar visual di LCD I2C
- Menggabungkan komunikasi UART (Serial Monitor) dan I2C (LCD) dalam satu sistem

---

## 2. Cara Kerja Komunikasi I2C antara Arduino dan LCD

### Analogi: Sistem Interkom Apartemen 🏢

Bayangkan sebuah gedung apartemen besar dengan sistem interkom terpusat. Cara kerjanya persis seperti I2C:

- **Arduino** = Resepsionis (Master) yang duduk di lobi dan bisa menghubungi siapa saja
- **LCD I2C** = Penghuni apartemen (Slave) yang memiliki **nomor unit (alamat)** unik, misalnya unit **0x27**
- **Kabel SDA** = Saluran suara (data dua arah)
- **Kabel SCL** = Bel/sinyal dari resepsionis yang mengatur kapan penghuni boleh bicara

Jika ada 10 penghuni, **semua menggunakan saluran interkom yang sama**, tetapi resepsionis menelepon nomor unit tertentu terlebih dahulu sebelum bicara. Penghuni yang nomor unitnya tidak dipanggil akan mengabaikan panggilan tersebut.

### Penjelasan Teknis Protokol I2C

I2C (Inter-Integrated Circuit) menggunakan hanya **dua jalur komunikasi**:

| Jalur | Nama | Fungsi | Pin Arduino Uno |
|---|---|---|---|
| **SDA** | Serial Data | Jalur data dua arah (Master ↔ Slave) | A4 |
| **SCL** | Serial Clock | Sinyal clock dari Master untuk sinkronisasi | A5 |

### Proses Komunikasi I2C Step-by-Step

```
┌──────────────────────────────────────────────────────────────────────┐
│               ALUR KOMUNIKASI I2C: Arduino → LCD                     │
│                                                                       │
│  1. START CONDITION                                                   │
│     Arduino menarik SDA dari HIGH ke LOW sementara SCL HIGH          │
│     → Sinyal "saya akan mulai berbicara"                              │
│                                                                       │
│  2. ALAMAT SLAVE (7 bit) + BIT R/W (1 bit)                           │
│     Arduino mengirim: [0x27][W]                                       │
│     = [0100111][0] dalam biner                                        │
│     → "Saya ingin bicara dengan perangkat di alamat 0x27 (WRITE)"    │
│                                                                       │
│  3. ACK dari LCD                                                      │
│     LCD menarik SDA ke LOW selama 1 clock                            │
│     → "Saya ada, silakan kirim data"                                  │
│                                                                       │
│  4. DATA (8 bit per byte)                                             │
│     Arduino mengirim byte data (karakter, perintah cursor, dll.)     │
│     LCD menerima dan menampilkan sesuai perintah                      │
│                                                                       │
│  5. STOP CONDITION                                                    │
│     Arduino menarik SDA dari LOW ke HIGH sementara SCL HIGH          │
│     → Sinyal "saya selesai berbicara"                                 │
└──────────────────────────────────────────────────────────────────────┘
```

### Diagram Sinyal I2C

```
SCL:  ‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾
SDA:  ‾‾| ADDR[6:0] |R/W|ACK|   DATA[7:0]  |ACK
         ↑ START                                ↑ STOP
```

### Peran Library LiquidCrystal_I2C

Library `LiquidCrystal_I2C` menyembunyikan kerumitan protokol I2C di balik fungsi yang mudah digunakan:

```arduino
lcd.init();          // Kirim sequence inisialisasi ke LCD via I2C
lcd.backlight();     // Kirim perintah nyalakan backlight via I2C
lcd.setCursor(0, 0); // Kirim perintah pindah kursor ke baris 0, kolom 0
lcd.print("Teks");   // Kirim karakter satu per satu via I2C ke LCD
```

Di balik setiap pemanggilan fungsi tersebut, Arduino mengirimkan paket-paket data I2C ke modul PCF8574 (chip I/O expander yang biasanya ada di belakang modul LCD I2C), yang kemudian mengontrol pin-pin LCD secara paralel.

---

## 3. Fungsi Pin Potensiometer dan Efek Jika Tertukar

### Struktur Potensiometer

Potensiometer adalah **resistor variabel** dengan tiga terminal. Secara internal, komponen ini terdiri dari elemen resistif (panjang tetap) dan sebuah wiper (kontak geser) yang posisinya dapat diubah dengan memutar kenop.

```
        [Kaki Kiri]          [Kaki Kanan]
             │                    │
    GND (0V) │──[══════╪══════]──│ 5V (VCC)
                        │
                   [Kaki Tengah]
                   ke pin A0 (ADC)
                   Output tegangan: 0V – 5V
```

### Fungsi Masing-Masing Kaki

| Kaki | Terhubung ke | Fungsi |
|---|---|---|
| **Kaki Kiri** | GND (0V) | Titik referensi tegangan bawah |
| **Kaki Tengah (Wiper)** | Pin A0 Arduino | Output tegangan variabel (dibaca ADC) |
| **Kaki Kanan** | 5V (VCC) | Titik referensi tegangan atas |

### Cara Kerja (Prinsip Pembagi Tegangan)

Potensiometer bekerja sebagai **voltage divider** (pembagi tegangan):

- Saat kenop diputar ke **paling kiri** → Wiper berada di ujung GND → Output A0 = **0V** → ADC = **0**
- Saat kenop diputar ke **posisi tengah** → Output A0 = **2.5V** → ADC ≈ **512**
- Saat kenop diputar ke **paling kanan** → Wiper berada di ujung 5V → Output A0 = **5V** → ADC = **1023**

### Apa yang Terjadi Jika Kaki Kiri dan Kanan Tertukar?

Jika koneksi **GND dan 5V tertukar** (kaki kiri ke 5V, kaki kanan ke GND):

```
[Kondisi Normal]                 [Kondisi Tertukar]
Putar kiri  → ADC naik ↑        Putar kiri  → ADC turun ↓
Putar kanan → ADC turun ↓       Putar kanan → ADC naik ↑
```

**Efek yang terjadi:**
1. **Nilai ADC terbalik:** Saat kenop diputar ke kiri (mendekati yang sekarang terhubung ke 5V), nilai ADC justru besar, bukan kecil.
2. **Bar di LCD terbalik:** Bar akan semakin pendek saat potensiometer diputar ke kanan, dan semakin panjang saat diputar ke kiri — berlawanan dengan yang diharapkan.
3. **Tidak ada kerusakan hardware:** Potensiometer tidak akan rusak karena terbalik (tegangan tetap dalam batas aman). Ini murni masalah logika/orientasi pembacaan.
4. **Formula konversi ikut terbalik:** Persentase dan tampilan voltase akan memberikan nilai yang "terbalik" dari intuisi pengguna.

---

## 4. Kode Program Modifikasi (UART + I2C)

### Deskripsi Modifikasi

Program digabungkan sehingga data ADC ditampilkan **di dua tempat sekaligus**:
- **LCD I2C** (via I2C): Baris 1 = nilai ADC, Baris 2 = bar level visual
- **Serial Monitor** (via UART): Format lengkap `ADC: xxx Volt: x.xx V Persen: xx%`

### Kode Program Lengkap

```cpp
// ============================================================
//  File    : modul3_i2c_uart_gabungan.ino
//  Modul   : 3B - I2C + UART (Modifikasi Gabungan)
//  Deskripsi: Baca potensiometer, tampilkan di LCD I2C (I2C)
//             DAN Serial Monitor (UART) secara bersamaan
// ============================================================

// --- Library yang Dibutuhkan ---
#include <Wire.h>
// Library Wire diperlukan untuk komunikasi I2C.
// Menyediakan fungsi Wire.begin(), Wire.write(), Wire.read(), dll.
// Sudah termasuk dalam instalasi Arduino IDE default.

#include <LiquidCrystal_I2C.h>
// Library untuk mengontrol LCD karakter melalui protokol I2C.
// Harus diinstall manual via Library Manager: cari "LiquidCrystal I2C" by Frank de Brabander.

#include <Arduino.h>
// Header utama Arduino yang menyediakan fungsi-fungsi dasar seperti
// pinMode(), digitalWrite(), analogRead(), millis(), dll.

// --- Inisialisasi Objek LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Membuat objek lcd dengan konfigurasi:
//   0x27 = Alamat I2C modul LCD (coba 0x3F jika LCD tidak menyala)
//   16   = Jumlah kolom LCD (16 karakter per baris)
//   2    = Jumlah baris LCD (2 baris)
// Alamat I2C dapat ditemukan dengan menjalankan sketch I2C Scanner.

// --- Definisi Pin ---
const int pinPot = A0;
// Pin analog A0 terhubung ke kaki tengah (wiper) potensiometer.
// analogRead(A0) akan mengembalikan nilai 0–1023.

// --- Konstanta ---
const float TEGANGAN_REFERENSI = 5.0;
// Tegangan referensi Arduino Uno adalah 5V.
// Digunakan untuk konversi nilai ADC ke satuan Volt.

const int ADC_MAX = 1023;
// Nilai maksimum output ADC Arduino Uno (resolusi 10 bit = 2^10 - 1).

const int PANJANG_BAR_MAX = 16;
// Panjang maksimum bar di LCD = 16 kolom (sesuai lebar LCD 16x2).

// ============================================================
//  SETUP — dijalankan sekali saat Arduino dinyalakan/reset
// ============================================================
void setup() {
    Serial.begin(9600);
    // Inisialisasi komunikasi UART dengan baud rate 9600 bps.
    // Harus sama dengan pengaturan di Serial Monitor Arduino IDE.

    Serial.println("========================================");
    Serial.println(" Sistem Monitor Potensiometer (ADC)    ");
    Serial.println("========================================");
    Serial.println("Format: ADC: xxx  Volt: x.xx V  Persen: xx%");
    Serial.println("----------------------------------------");
    // Tampilkan header informasi ke Serial Monitor saat program dimulai.

    lcd.init();
    // Inisialisasi LCD: mengirim sequence perintah via I2C untuk menyiapkan
    // kontroler HD44780 di dalam LCD agar siap digunakan.
    // Proses ini mencakup pengaturan mode 4-bit, pembersihan layar, dll.

    lcd.backlight();
    // Nyalakan backlight (lampu latar) LCD via pin yang dikontrol oleh
    // modul I2C backpack (PCF8574). Tanpa ini, layar gelap meski menampilkan data.

    lcd.setCursor(0, 0);
    // Pindahkan kursor ke kolom 0, baris 0 (pojok kiri atas LCD).
    // Parameter: setCursor(kolom, baris) — keduanya dimulai dari 0.

    lcd.print("  Memulai...    ");
    // Tampilkan pesan loading sementara saat inisialisasi.

    delay(1000);
    // Jeda 1 detik untuk menampilkan pesan loading.
    // delay() digunakan di setup() karena di sini tidak masalah memblokir sebentar.

    lcd.clear();
    // Bersihkan seluruh tampilan LCD sebelum memulai loop utama.
}

// ============================================================
//  LOOP — dijalankan berulang-ulang secara terus-menerus
// ============================================================
void loop() {

    // ----------------------------------------------------------
    //  BAGIAN 1: Baca Nilai Analog dari Potensiometer
    // ----------------------------------------------------------
    int nilaiADC = analogRead(pinPot);
    // Baca tegangan pada pin A0 menggunakan ADC 10-bit Arduino.
    // Mengembalikan nilai integer antara 0 (0V) hingga 1023 (5V).

    // ----------------------------------------------------------
    //  BAGIAN 2: Konversi Nilai ADC ke Satuan Lain
    // ----------------------------------------------------------
    float tegangan = (nilaiADC / (float)ADC_MAX) * TEGANGAN_REFERENSI;
    // Konversi nilai ADC ke tegangan (Volt):
    //   Tegangan = (ADC / 1023) × 5.0 V
    // (float) digunakan untuk memastikan pembagian floating-point, bukan integer.
    // Contoh: ADC=512 → (512/1023)×5.0 ≈ 2.50 V

    int persen = map(nilaiADC, 0, ADC_MAX, 0, 100);
    // Konversi nilai ADC ke persentase (0%–100%).
    // Fungsi map() memetakan nilai dari rentang sumber ke rentang tujuan.
    // map(nilai, fromLow, fromHigh, toLow, toHigh)
    // Contoh: ADC=512 → map(512, 0, 1023, 0, 100) = 50%

    int panjangBar = map(nilaiADC, 0, ADC_MAX, 0, PANJANG_BAR_MAX);
    // Konversi nilai ADC ke panjang bar untuk ditampilkan di baris LCD ke-2.
    // map(nilaiADC, 0, 1023, 0, 16): ADC=0 → bar=0, ADC=1023 → bar=16 blok.

    // ----------------------------------------------------------
    //  BAGIAN 3: Tampilkan Data di Serial Monitor (UART)
    // ----------------------------------------------------------
    Serial.print("ADC: ");
    Serial.print(nilaiADC);        // Tampilkan nilai ADC mentah (0–1023)
    Serial.print("\t");            // Tab untuk rapi
    Serial.print("Volt: ");
    Serial.print(tegangan, 2);     // Tampilkan tegangan dengan 2 angka desimal
    Serial.print(" V\t");          // Satuan dan tab
    Serial.print("Persen: ");
    Serial.print(persen);          // Tampilkan persentase
    Serial.println("%");           // Satuan persen + newline (pindah baris)
    // Contoh output: "ADC: 512    Volt: 2.50 V    Persen: 50%"

    // ----------------------------------------------------------
    //  BAGIAN 4: Tampilkan Data di LCD I2C (Baris 1 — Nilai ADC)
    // ----------------------------------------------------------
    lcd.setCursor(0, 0);
    // Pindahkan kursor ke awal baris pertama (baris 0, kolom 0).

    lcd.print("ADC:");
    // Cetak label "ADC:" sebagai keterangan nilai berikutnya.

    lcd.print(nilaiADC);
    // Cetak nilai ADC saat ini. Arduino secara otomatis mengkonversi
    // integer menjadi string karakter dan mengirimkannya via I2C ke LCD.

    lcd.print("  ");
    // Cetak spasi ekstra untuk menghapus sisa karakter lama jika
    // nilai ADC berubah dari 4 digit (1023) ke 3 digit (999) atau lebih kecil.
    // Tanpa ini, digit lama bisa tertinggal di layar (ghost character).

    // Tampilkan persentase di bagian kanan baris 1
    lcd.setCursor(8, 0);
    // Pindahkan kursor ke kolom 8 pada baris 0 (bagian kanan LCD).

    lcd.print(persen);
    // Cetak nilai persentase.

    lcd.print("% ");
    // Cetak simbol persen dan spasi pembersih.

    // ----------------------------------------------------------
    //  BAGIAN 5: Tampilkan Bar Level di LCD I2C (Baris 2)
    // ----------------------------------------------------------
    lcd.setCursor(0, 1);
    // Pindahkan kursor ke awal baris kedua (baris 1, kolom 0).

    for (int i = 0; i < PANJANG_BAR_MAX; i++) {
        // Iterasi sebanyak 16 kali untuk mengisi setiap kolom baris ke-2 LCD.

        if (i < panjangBar) {
            lcd.print((char)255);
            // Cetak karakter ASCII 255 yang pada LCD HD44780 terlihat sebagai
            // blok penuh (█). Ini membentuk bar visual yang menunjukkan level.
            // Jumlah blok = nilai panjangBar yang sudah dihitung dari ADC.
        } else {
            lcd.print(" ");
            // Cetak spasi (kosong) untuk bagian bar yang belum terisi.
            // Ini PENTING untuk menghapus blok lama saat nilai ADC turun.
        }
    }

    // ----------------------------------------------------------
    //  BAGIAN 6: Delay untuk Kestabilan Pembacaan
    // ----------------------------------------------------------
    delay(200);
    // Jeda 200ms sebelum membaca ulang nilai ADC.
    // Tujuan:
    //   1. Mencegah nilai ADC berfluktuasi terlalu cepat (noise filtering)
    //   2. Memberikan waktu LCD untuk merender tampilan sebelum diperbarui lagi
    //   3. Mengurangi traffic I2C yang tidak perlu
    // Nilai 200ms cukup responsif untuk potensiometer (update 5x per detik).
}
```

### Contoh Output Serial Monitor

```
========================================
 Sistem Monitor Potensiometer (ADC)    
========================================
Format: ADC: xxx  Volt: x.xx V  Persen: xx%
----------------------------------------
ADC: 0      Volt: 0.00 V    Persen: 0%
ADC: 256    Volt: 1.25 V    Persen: 25%
ADC: 512    Volt: 2.50 V    Persen: 50%
ADC: 768    Volt: 3.75 V    Persen: 75%
ADC: 1023   Volt: 5.00 V    Persen: 100%
```

### Tampilan LCD

```
┌────────────────┐
│ADC:512  50%    │   ← Baris 1: Nilai ADC dan Persentase
│████████        │   ← Baris 2: Bar level (8 dari 16 blok terisi)
└────────────────┘
```

---

## 5. Tabel Hasil Pengamatan

Tabel berikut menunjukkan hasil pengamatan pada Serial Monitor berdasarkan nilai ADC yang didapat saat memutar potensiometer. Nilai Volt dan Persen dihitung menggunakan formula konversi yang ada dalam program.

| ADC | Volt (V) | Persen (%) | Kondisi Potensiometer |
|:---:|:---:|:---:|:---|
| 1 | 0.00 V | 0% | Diputar paling kiri (hampir GND) |
| 21 | 0.10 V | 2% | Sedikit diputar ke kanan dari minimum |
| 49 | 0.24 V | 5% | Sekitar 5% dari rentang penuh |
| 74 | 0.36 V | 7% | Sekitar 7% dari rentang penuh |
| 96 | 0.47 V | 9% | Sekitar 9% dari rentang penuh |
| 256 | 1.25 V | 25% | Seperempat putaran |
| 512 | 2.50 V | 50% | Posisi tengah (setengah putaran) |
| 768 | 3.75 V | 75% | Tiga perempat putaran |
| 1023 | 5.00 V | 100% | Diputar paling kanan (5V penuh) |

### Formula Konversi

Rumus yang digunakan dalam program untuk menghitung nilai pada tabel di atas:

```
Tegangan (V) = (Nilai ADC / 1023) × 5.0

Persen (%)   = (Nilai ADC / 1023) × 100
```

**Contoh perhitungan untuk ADC = 49:**
- Tegangan = (49 / 1023) × 5.0 = 0.0479 × 5.0 = **0.24 V** ✓
- Persen   = (49 / 1023) × 100 = **4.79% ≈ 5%** ✓

> **Catatan:** Nilai ADC pada hardware nyata mungkin sedikit berbeda dari perhitungan teoritis karena faktor noise, toleransi komponen, dan presisi ADC. Variasi ±1–3 ADC adalah normal.

---

## 6. Referensi

- [1] Arduino LLC, "Wire Library (I2C) - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/communication/wire/
- [2] Arduino LLC, "analogRead() - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
- [3] Last Minute Engineers, "Interface LCD using I2C with Arduino," https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/
- [4] Arduino LLC, "map() - Arduino Reference," https://www.arduino.cc/reference/en/language/functions/math/map/
- [5] Electronoobs, "I2C Protocol Explained - Arduino Tutorial," https://www.electronoobs.com/eng_arduino_tut4.php

---