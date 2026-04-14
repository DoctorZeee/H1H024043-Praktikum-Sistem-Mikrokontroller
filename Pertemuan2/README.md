# Modul 2 – Pemrograman GPIO
**Sistem Tertanam dan Mikrokontroler**
Universitas Jenderal Soedirman – Jurusan Informatika, Fakultas Teknik

---

## Percobaan 2A: Seven Segment Display (Counter 0–F)

### Pertanyaan 1 – Schematic Rangkaian

![Gambar](https://drive.google.com/file/d/1sYYCZrqXGg10U3zE2Qfl7FCcDojR5a64/view?usp=drive_open)
>
> *Keterangan: Gambar schematic menunjukkan koneksi antara pin a–g dan dp pada seven segment ke pin digital Arduino Uno sesuai Tabel 1.4, dengan resistor 220 Ohm pada setiap segmen dan GND dihubungkan ke common cathode.*

Konfigurasi wiring berdasarkan Tabel 1.4:

| Segmen | Label | Pin Arduino |
|--------|-------|-------------|
| a      | a     | 7           |
| b      | b     | 6           |
| c      | c     | 5           |
| d      | d     | 11          |
| e      | e     | 10          |
| f      | f     | 8           |
| g      | g     | 9           |
| dp     | titik | 4           |

---

### Pertanyaan 2 – Apa yang terjadi jika nilai `num` lebih dari 15?

Jika nilai `num` melebihi 15, maka program akan mengakses indeks array `digitPattern` di luar batas yang didefinisikan (`digitPattern[16][8]`). Hal ini menyebabkan **undefined behavior** pada C/C++ (Arduino), yaitu:

- Program bisa membaca nilai memori sembarang di luar array, sehingga seven segment menampilkan pola acak atau tidak terdefinisi.
- Tidak ada pesan error karena Arduino tidak memiliki mekanisme bounds checking bawaan.
- Dalam skenario terburuk, bisa menyebabkan program crash atau perilaku tidak terprediksi.

**Solusi:** Tambahkan validasi sebelum memanggil `displayDigit()`:

```cpp
void displayDigit(int num) {
    if (num < 0 || num > 15) return; // abaikan nilai di luar rentang
    for (int i = 0; i < 8; i++) {
        digitalWrite(segmentPins[i], digitPattern[num][i]);
    }
}
```

---

### Pertanyaan 3 – Common Cathode atau Common Anode?

Program ini menggunakan konfigurasi **Common Cathode**.

**Alasan:**

Pada array `digitPattern`, nilai `1` (HIGH / 5V) diberikan untuk menyalakan segmen, dan nilai `0` (LOW / 0V) untuk mematikannya. Contohnya pada digit `0`:

```cpp
{1, 1, 1, 1, 1, 1, 0, 0}  // a=ON, b=ON, c=ON, d=ON, e=ON, f=ON, g=OFF, dp=OFF
```

Logika ini sesuai dengan seven segment **Common Cathode**, di mana:
- Kaki common dihubungkan ke GND.
- Segmen menyala ketika pin diberi logika HIGH (anoda mendapat tegangan positif).

Jika menggunakan **Common Anode**, logikanya akan terbalik — segmen menyala saat pin diberi logika LOW — sehingga seluruh isi array `digitPattern` harus diinvert (nilai `1` menjadi `0` dan sebaliknya).

---

### Pertanyaan 4 – Modifikasi Program: Counter F ke 0

Program dimodifikasi agar tampilan berjalan mundur dari `F` ke `0`.

**Perubahan pada fungsi `loop()`:** Cukup ubah iterasi dari maju (`i=0; i<16; i++`) menjadi mundur (`i=15; i>=0; i--`).

```cpp
#include <Arduino.h>

// ============================================================
// PIN MAPPING
// Mendefinisikan urutan pin Arduino yang terhubung ke segmen
// a, b, c, d, e, f, g, dp pada seven segment
// ============================================================
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};
// urutan: a  b  c  d   e   f  g  dp

// ============================================================
// DATA POLA DIGIT (Common Cathode)
// Setiap baris adalah pola nyala segmen untuk 1 karakter hex
// Urutan kolom: a  b  c  d  e  f  g  dp
// 1 = segmen menyala (HIGH), 0 = segmen mati (LOW)
// ============================================================
byte digitPattern[16][8] = {
    {1, 1, 1, 1, 1, 1, 0, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1, 0},  // 2
    {1, 1, 1, 1, 0, 0, 1, 0},  // 3
    {0, 1, 1, 0, 0, 1, 1, 0},  // 4
    {1, 0, 1, 1, 0, 1, 1, 0},  // 5
    {1, 0, 1, 1, 1, 1, 1, 0},  // 6
    {1, 1, 1, 0, 0, 0, 0, 0},  // 7
    {1, 1, 1, 1, 1, 1, 1, 0},  // 8
    {1, 1, 1, 1, 0, 1, 1, 0},  // 9
    {1, 1, 1, 0, 1, 1, 1, 0},  // A
    {0, 0, 1, 1, 1, 1, 1, 0},  // b
    {1, 0, 0, 1, 1, 1, 0, 0},  // C
    {0, 1, 1, 1, 1, 0, 1, 0},  // d
    {1, 0, 0, 1, 1, 1, 1, 0},  // E
    {1, 0, 0, 0, 1, 1, 1, 0}   // F
};

// ============================================================
// FUNGSI displayDigit
// Menerima angka (0–15) dan menulis pola segmen ke pin Arduino
// ============================================================
void displayDigit(int num) {
    // Validasi: pastikan num ada dalam rentang yang valid
    if (num < 0 || num > 15) return;

    // Loop untuk setiap segmen (a sampai dp = 8 segmen)
    for (int i = 0; i < 8; i++) {
        // Tulis nilai HIGH/LOW ke pin segmen sesuai pola digit
        digitalWrite(segmentPins[i], digitPattern[num][i]);
    }
}

// ============================================================
// SETUP
// Dijalankan sekali saat Arduino menyala
// ============================================================
void setup() {
    // Set semua pin segmen sebagai OUTPUT
    for (int i = 0; i < 8; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }
}

// ============================================================
// LOOP
// Dijalankan terus-menerus selama Arduino aktif
// ============================================================
void loop() {
    // Iterasi mundur dari F (15) ke 0 — perubahan utama dari versi awal
    for (int i = 15; i >= 0; i--) {
        displayDigit(i);   // Tampilkan digit ke-i pada seven segment
        delay(1000);       // Tahan tampilan selama 1 detik
    }
}
```

---

---

## Percobaan 2B: Kontrol Counter dengan Push Button

### Pertanyaan 1 – Schematic Rangkaian

![Gambar](https://drive.google.com/file/d/16CBq8nWE8Pv8QfFqD4t5XOpiTwV8g54A/view)
>
> *Keterangan: Gambar schematic menunjukkan koneksi seven segment ke pin Arduino (sama seperti percobaan 2A), ditambah satu push button yang terhubung antara Pin 2 Arduino dan GND. Tidak diperlukan resistor eksternal karena menggunakan INPUT_PULLUP.*

Tambahan wiring push button:

| Komponen    | Kaki        | Pin Arduino |
|-------------|-------------|-------------|
| Push Button | Satu kaki   | Pin 2       |
| Push Button | Kaki lainnya | GND        |

---

### Pertanyaan 2 – Mengapa Push Button Menggunakan `INPUT_PULLUP`?

`INPUT_PULLUP` mengaktifkan resistor pull-up internal pada Arduino (sekitar 20–50 kΩ) sehingga pin selalu terhubung ke 5V secara internal. Ketika tombol ditekan, pin terhubung ke GND dan terbaca LOW. Ketika tombol dilepas, pin ditarik ke HIGH oleh resistor internal.

**Keuntungan dibandingkan rangkaian biasa (tanpa pull-up):**

| Aspek | INPUT_PULLUP | Rangkaian biasa (tanpa resistor) |
|-------|-------------|----------------------------------|
| Komponen tambahan | Tidak perlu resistor eksternal | Perlu resistor pull-up/pull-down eksternal |
| Kondisi floating | Tidak terjadi (pin selalu terdefinisi HIGH/LOW) | Bisa terjadi — pin membaca nilai random |
| Kerumitan rangkaian | Lebih sederhana | Lebih kompleks |
| Keandalan | Lebih andal | Rentan noise dan glitch |

Tanpa pull-up, pin dalam kondisi "floating" saat tombol tidak ditekan, sehingga bisa membaca nilai HIGH atau LOW secara acak — menyebabkan counter berubah sendiri tanpa tombol ditekan.

---

### Pertanyaan 3 – Penyebab Salah Satu Segmen Tidak Menyala

**Dari sisi Hardware:**

1. **Kabel jumper tidak terhubung dengan baik** — koneksi longgar di breadboard atau pin Arduino.
2. **Resistor putus atau nilai salah** — resistor 220 Ohm harus terpasang di setiap segmen.
3. **LED segmen rusak** — salah satu segmen pada seven segment sudah mati secara fisik.
4. **Wiring terbalik** — pin segmen terhubung ke pin Arduino yang salah (tidak sesuai Tabel 1.4).
5. **Posisi seven segment di breadboard salah** — pin tidak masuk ke lubang yang terhubung dengan benar.

**Dari sisi Software:**

1. **Kesalahan mapping pin** — array `segmentPins[]` tidak sesuai urutan segmen fisik.
2. **Pola digit salah** — nilai di `digitPattern[][]` untuk segmen tertentu selalu `0` secara tidak sengaja.
3. **`pinMode()` tidak dipanggil** — pin lupa diset sebagai `OUTPUT` di `setup()`.
4. **Salah indeks array** — off-by-one error sehingga satu segmen tidak pernah ditulis.

---

### Pertanyaan 4 – Modifikasi: Dua Push Button (Increment & Decrement)

Rangkaian ditambahkan satu push button lagi untuk fungsi pengurangan (decrement).

**Tambahan wiring:**

| Komponen       | Kaki          | Pin Arduino |
|----------------|---------------|-------------|
| Push Button UP | Satu kaki     | Pin 2       |
| Push Button UP | Kaki lainnya  | GND         |
| Push Button DOWN | Satu kaki   | Pin 3       |
| Push Button DOWN | Kaki lainnya | GND        |

> 📷 **[SISIPKAN GAMBAR SCHEMATIC RANGKAIAN DENGAN 2 PUSH BUTTON DI SINI]**
>
> *Keterangan: Sama seperti percobaan 2B namun ditambahkan satu push button lagi pada Pin 3 Arduino, juga terhubung ke GND, tanpa resistor eksternal (menggunakan INPUT_PULLUP).*

> 📷 **[SISIPKAN FOTO RANGKAIAN FISIK (BREADBOARD) DENGAN 2 PUSH BUTTON DI SINI]**

**Source Code:**

```cpp
#include <Arduino.h>

// ============================================================
// PIN MAPPING – SEVEN SEGMENT
// Urutan pin sesuai segmen: a, b, c, d, e, f, g, dp
// ============================================================
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// ============================================================
// PIN PUSH BUTTON
// btnUp  : tombol untuk menambah (increment) nilai counter
// btnDown: tombol untuk mengurangi (decrement) nilai counter
// ============================================================
const int btnUp   = 2;  // Pin 2 → push button naik
const int btnDown = 3;  // Pin 3 → push button turun

// ============================================================
// DATA POLA DIGIT (Common Cathode)
// Urutan kolom: a  b  c  d  e  f  g  dp
// 1 = menyala (HIGH), 0 = mati (LOW)
// ============================================================
byte digitPattern[16][8] = {
    {1, 1, 1, 1, 1, 1, 0, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1, 0},  // 2
    {1, 1, 1, 1, 0, 0, 1, 0},  // 3
    {0, 1, 1, 0, 0, 1, 1, 0},  // 4
    {1, 0, 1, 1, 0, 1, 1, 0},  // 5
    {1, 0, 1, 1, 1, 1, 1, 0},  // 6
    {1, 1, 1, 0, 0, 0, 0, 0},  // 7
    {1, 1, 1, 1, 1, 1, 1, 0},  // 8
    {1, 1, 1, 1, 0, 1, 1, 0},  // 9
    {1, 1, 1, 0, 1, 1, 1, 0},  // A
    {0, 0, 1, 1, 1, 1, 1, 0},  // b
    {1, 0, 0, 1, 1, 1, 0, 0},  // C
    {0, 1, 1, 1, 1, 0, 1, 0},  // d
    {1, 0, 0, 1, 1, 1, 1, 0},  // E
    {1, 0, 0, 0, 1, 1, 1, 0}   // F
};

// ============================================================
// VARIABEL GLOBAL
// currentDigit : nilai hex yang sedang ditampilkan (0–15)
// lastUpState  : status tombol UP sebelumnya (untuk edge detection)
// lastDownState: status tombol DOWN sebelumnya (untuk edge detection)
// ============================================================
int  currentDigit  = 0;
bool lastUpState   = HIGH; // HIGH = tombol tidak ditekan (karena INPUT_PULLUP)
bool lastDownState = HIGH;

// ============================================================
// FUNGSI displayDigit
// Menulis pola segmen ke semua pin sesuai angka yang diberikan
// Parameter: num = nilai 0–15 yang ingin ditampilkan
// ============================================================
void displayDigit(int num) {
    // Validasi rentang nilai agar tidak akses array di luar batas
    if (num < 0 || num > 15) return;

    // Tulis nilai ke setiap pin segmen (a sampai dp)
    for (int i = 0; i < 8; i++) {
        digitalWrite(segmentPins[i], digitPattern[num][i]);
    }
}

// ============================================================
// SETUP
// Inisialisasi pin – dijalankan sekali saat Arduino menyala
// ============================================================
void setup() {
    // Set semua 8 pin segmen sebagai OUTPUT
    for (int i = 0; i < 8; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }

    // Set pin push button sebagai INPUT dengan pull-up internal
    // (pin akan HIGH saat tidak ditekan, LOW saat ditekan)
    pinMode(btnUp,   INPUT_PULLUP);
    pinMode(btnDown, INPUT_PULLUP);

    // Tampilkan nilai awal (0) saat pertama kali menyala
    displayDigit(currentDigit);
}

// ============================================================
// LOOP
// Membaca status tombol dan memperbarui counter
// Menggunakan edge detection agar counter hanya naik/turun
// sekali per tekanan, bukan terus-menerus selama ditekan
// ============================================================
void loop() {
    // Baca kondisi tombol saat ini
    // LOW  = sedang ditekan  (karena INPUT_PULLUP)
    // HIGH = tidak ditekan
    bool upState   = digitalRead(btnUp);
    bool downState = digitalRead(btnDown);

    // ======== TOMBOL UP (INCREMENT) ========
    // Deteksi tepi turun: transisi dari HIGH → LOW = tombol baru ditekan
    if (lastUpState == HIGH && upState == LOW) {
        currentDigit++;                              // Tambah nilai counter
        if (currentDigit > 15) currentDigit = 0;   // Wrap-around: setelah F kembali ke 0
        displayDigit(currentDigit);                 // Perbarui tampilan seven segment
    }

    // ======== TOMBOL DOWN (DECREMENT) ========
    // Deteksi tepi turun: transisi dari HIGH → LOW = tombol baru ditekan
    if (lastDownState == HIGH && downState == LOW) {
        currentDigit--;                              // Kurangi nilai counter
        if (currentDigit < 0) currentDigit = 15;   // Wrap-around: sebelum 0 kembali ke F
        displayDigit(currentDigit);                 // Perbarui tampilan seven segment
    }

    // Simpan status tombol saat ini untuk dibandingkan di iterasi berikutnya
    lastUpState   = upState;
    lastDownState = downState;
}
```

---

## Pertanyaan Umum (2.7)

### 1 – Hasil Praktikum

**Percobaan 2A:** Program berhasil menampilkan karakter hex 0 hingga F secara berurutan pada seven segment dengan jeda 1 detik per digit. Pola segmen sesuai dengan array `digitPattern` yang telah didefinisikan. Tampilan berjalan terus-menerus tanpa error.

**Percobaan 2B:** Push button berfungsi sebagai pemicu increment counter. Setiap kali tombol ditekan dan dilepas, nilai yang ditampilkan pada seven segment bertambah 1. Setelah mencapai F (15), nilai kembali ke 0 (wrap-around). Sistem berjalan sesuai spesifikasi.

---

### 2 – Prinsip Kerja Seven Segment Display

Seven segment display terdiri dari **7 segmen LED** (diberi label a–g) yang disusun membentuk angka delapan, ditambah 1 segmen titik desimal (dp). Dengan menyalakan kombinasi segmen tertentu, berbagai angka dan karakter dapat ditampilkan.

Contoh untuk menampilkan angka `0`: segmen a, b, c, d, e, f dinyalakan; segmen g dan dp dimatikan.

Terdapat dua jenis seven segment:
- **Common Cathode:** Kaki common dihubungkan ke GND. Segmen menyala saat pin diberi logika HIGH.
- **Common Anode:** Kaki common dihubungkan ke VCC. Segmen menyala saat pin diberi logika LOW.

Pada percobaan ini digunakan **Common Cathode**, sehingga logika `1` = menyala.

---

### 3 – Cara Kerja Sistem Counter pada Program

Sistem counter bekerja menggunakan mekanisme **edge detection** (deteksi tepi sinyal):

1. Setiap iterasi `loop()`, program membaca status pin push button menggunakan `digitalRead()`.
2. Status saat ini dibandingkan dengan status sebelumnya (`lastUpState`).
3. Jika terjadi transisi dari `HIGH` ke `LOW` (tombol baru saja ditekan), nilai `currentDigit` ditambah atau dikurangi 1.
4. Jika `currentDigit` melampaui batas (>15 atau <0), dilakukan **wrap-around** kembali ke ujung lain.
5. Fungsi `displayDigit()` dipanggil untuk memperbarui tampilan seven segment sesuai nilai `currentDigit` yang baru.
6. Status tombol disimpan ke `lastUpState`/`lastDownState` untuk dibandingkan pada iterasi berikutnya.

Penggunaan edge detection ini penting agar counter hanya berubah sekali per tekanan, bukan terus-menerus selama tombol ditahan.

---

*Dokumen ini merupakan laporan Tugas Pendahuluan dan Jawaban Pertanyaan Praktikum Modul 2 – Pemrograman GPIO.*