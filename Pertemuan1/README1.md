# Modul I – Percabangan dan Perulangan
## Percobaan 1A: Percabangan (if-else) pada Arduino

**Mata Kuliah:** Sistem Tertanam dan Mikrokontroler  
**Program Studi:** Jurusan Informatika, Fakultas Teknik  
**Universitas:** Universitas Jenderal Soedirman  

---

## Deskripsi Singkat

Percobaan ini bertujuan untuk memahami dan mengimplementasikan konsep percabangan `if-else` dalam pemrograman mikrokontroler Arduino. Sebagai media visualisasi, digunakan sebuah LED yang berkedip dengan kecepatan berubah-ubah berdasarkan nilai variabel `timeDelay`. Program menunjukkan bagaimana percabangan digunakan untuk mengontrol alur eksekusi secara dinamis.

---

## 1. Jawaban Pertanyaan Teori

### Pertanyaan 1: Pada kondisi apa program masuk ke blok `if`?

Program masuk ke blok `if` ketika nilai variabel `timeDelay` **kurang dari atau sama dengan 100** (yaitu `timeDelay <= 100`).

Kondisi ini tercapai setelah program berulang kali mengurangi nilai `timeDelay` sebesar 100 ms di setiap siklus kedip. Dimulai dari 1000 ms, nilai `timeDelay` akan turun menjadi 900, 800, 700, ..., hingga akhirnya mencapai 100 ms. Pada titik itulah kondisi `timeDelay <= 100` menjadi **benar (true)**, dan program masuk ke blok `if` untuk memberikan jeda 3 detik lalu mereset nilai `timeDelay` kembali ke 1000 ms.

**Singkatnya:** Program masuk blok `if` saat LED sudah berkedip sangat cepat (delay = 100 ms) dan sudah waktunya untuk direset ke kondisi awal.

---

### Pertanyaan 2: Pada kondisi apa program masuk ke blok `else`?

Program masuk ke blok `else` ketika nilai `timeDelay` **lebih dari 100** (yaitu kondisi `timeDelay <= 100` bernilai **salah / false**).

Artinya, selama LED masih berkedip dengan delay di atas 100 ms (misalnya 1000 ms, 900 ms, 800 ms, dan seterusnya), program akan selalu masuk ke blok `else` dan mengurangi nilai `timeDelay` sebesar 100 ms. Ini yang menyebabkan kecepatan kedip LED semakin lama semakin cepat di setiap siklus.

**Singkatnya:** Program masuk blok `else` saat LED masih dalam proses percepatan, dan perlu dikurangi delay-nya agar berkedip lebih cepat.

---

### Pertanyaan 3: Apa fungsi dari perintah `delay(timeDelay)`?

Perintah `delay(timeDelay)` berfungsi untuk **menghentikan sementara eksekusi program** selama waktu tertentu yang ditentukan oleh nilai variabel `timeDelay` dalam satuan **milidetik (ms)**.

Fungsi ini digunakan dua kali dalam setiap siklus:
1. Setelah LED dinyalakan (`HIGH`), agar LED tetap menyala selama `timeDelay` ms.
2. Setelah LED dimatikan (`LOW`), agar LED tetap mati selama `timeDelay` ms.

Karena nilai `timeDelay` berubah setiap siklus (semakin kecil), maka durasi LED menyala dan mati pun semakin pendek — sehingga **LED terlihat berkedip semakin cepat** dari waktu ke waktu.

---

## 2. Modifikasi Program: Alur Cepat → Sedang → Mati

### Deskripsi Perubahan

Program asli memiliki alur: **mati → lambat → cepat → reset (mati)**  
Program yang dimodifikasi memiliki alur: **cepat → sedang → mati** (tanpa langsung reset ke kondisi semula)

**Logika perubahan delay:**
- Program dimulai dengan delay kecil (100 ms) sehingga LED berkedip **cepat**.
- Setiap siklus, nilai `timeDelay` **ditambah** sebesar 100 ms → LED berkedip lebih lambat (**sedang**).
- Ketika `timeDelay` mencapai nilai ambang tertentu (1100 ms), LED dimatikan total selama beberapa detik sebagai fase **mati**, lalu `timeDelay` direset kembali ke nilai awal (100 ms) untuk memulai siklus berikutnya.

Dengan demikian, urutannya adalah: **cepat (100ms) → sedang (200–1000ms) → mati (delay panjang) → ulangi**.

---

## 3. Source Code Arduino (Modifikasi)

```cpp
// ============================================================
// Modul I – Percobaan 1A: Percabangan (Modifikasi)
// Alur: Cepat → Sedang → Mati
// Universitas Jenderal Soedirman – Jurusan Informatika
// ============================================================

// Mendefinisikan pin LED yang digunakan (pin digital 6)
const int ledPin = 6;

// Mendefinisikan nilai awal timeDelay = 100 ms (kondisi CEPAT)
int timeDelay = 100;

// -------------------------------------------------------
// setup() dijalankan sekali saat Arduino pertama dinyalakan
// -------------------------------------------------------
void setup() {
  // Mengatur pin LED sebagai OUTPUT agar bisa mengendalikan LED
  pinMode(ledPin, OUTPUT);
}

// -------------------------------------------------------
// loop() dijalankan terus-menerus selama Arduino aktif
// -------------------------------------------------------
void loop() {

  // Menyalakan LED dengan memberikan sinyal HIGH ke pin ledPin
  digitalWrite(ledPin, HIGH);

  // Menunggu selama timeDelay milidetik → LED tetap menyala
  delay(timeDelay);

  // Mematikan LED dengan memberikan sinyal LOW ke pin ledPin
  digitalWrite(ledPin, LOW);

  // Menunggu selama timeDelay milidetik → LED tetap mati
  delay(timeDelay);

  // -------------------------------------------------------
  // BLOK PERCABANGAN: menentukan apa yang terjadi selanjutnya
  // -------------------------------------------------------

  // Kondisi IF: jika timeDelay sudah melebihi atau sama dengan 1100 ms
  // Artinya: LED sudah berkedip sangat lambat → masuk fase MATI
  if (timeDelay >= 1100) {

    // Matikan LED secara permanen selama fase "mati"
    digitalWrite(ledPin, LOW);

    // Tahan selama 3 detik (3000 ms) sebagai jeda fase mati
    delay(3000);

    // Reset timeDelay kembali ke kondisi CEPAT (100 ms)
    // agar siklus cepat → sedang → mati dapat dimulai ulang
    timeDelay = 100;

  // Kondisi ELSE: jika timeDelay masih di bawah 1100 ms
  // Artinya: LED masih dalam proses perlambatan (cepat → sedang)
  } else {

    // Tambah timeDelay sebesar 100 ms → LED berkedip lebih lambat
    // Setiap satu siklus kedip, delay bertambah 100 ms
    timeDelay += 100;

  }
}
```

---

## 4. Penjelasan Kode Per Bagian

### Bagian 1 – Deklarasi Variabel

```cpp
const int ledPin = 6;
int timeDelay = 100;
```

| Baris | Penjelasan |
|-------|-----------|
| `const int ledPin = 6;` | Mendeklarasikan konstanta `ledPin` bernilai 6. Ini berarti LED terhubung ke **pin digital 6** pada Arduino. Kata `const` berarti nilainya tidak akan berubah sepanjang program. |
| `int timeDelay = 100;` | Mendeklarasikan variabel `timeDelay` dengan nilai awal **100 ms**. Nilai ini akan terus bertambah di setiap siklus untuk memperlambat kedipan LED. Dimulai dari 100 ms agar efek pertama yang terlihat adalah LED berkedip **cepat**. |

---

### Bagian 2 – Fungsi `setup()`

```cpp
void setup() {
  pinMode(ledPin, OUTPUT);
}
```

| Baris | Penjelasan |
|-------|-----------|
| `void setup()` | Fungsi ini hanya dijalankan **satu kali** saat Arduino dinyalakan atau di-reset. |
| `pinMode(ledPin, OUTPUT);` | Mengatur pin 6 sebagai **OUTPUT**, sehingga Arduino dapat mengirimkan sinyal listrik untuk menyalakan atau mematikan LED. |

---

### Bagian 3 – Kontrol LED dalam `loop()`

```cpp
digitalWrite(ledPin, HIGH);
delay(timeDelay);
digitalWrite(ledPin, LOW);
delay(timeDelay);
```

| Baris | Penjelasan |
|-------|-----------|
| `digitalWrite(ledPin, HIGH);` | Mengirim tegangan 5V ke pin 6 → LED **menyala**. |
| `delay(timeDelay);` | Program berhenti sementara selama `timeDelay` milidetik → LED tetap **menyala** selama durasi ini. |
| `digitalWrite(ledPin, LOW);` | Menghentikan tegangan ke pin 6 → LED **padam**. |
| `delay(timeDelay);` | Program berhenti sementara selama `timeDelay` milidetik → LED tetap **padam** selama durasi ini. |

Karena `timeDelay` semakin besar setiap siklus, durasi menyala dan mati pun semakin panjang, sehingga LED terlihat berkedip semakin lambat.

---

### Bagian 4 – Blok Percabangan `if-else`

```cpp
if (timeDelay >= 1100) {
    digitalWrite(ledPin, LOW);
    delay(3000);
    timeDelay = 100;
} else {
    timeDelay += 100;
}
```

| Baris | Penjelasan |
|-------|-----------|
| `if (timeDelay >= 1100)` | Kondisi pengecekan: apakah `timeDelay` sudah mencapai atau melewati 1100 ms? Jika iya, berarti LED sudah berkedip sangat lambat dan saatnya masuk fase **mati**. |
| `digitalWrite(ledPin, LOW);` | Memastikan LED dalam kondisi **mati** saat fase jeda. |
| `delay(3000);` | Memberikan jeda **3 detik** sebagai representasi fase "mati total" sebelum siklus dimulai ulang. |
| `timeDelay = 100;` | Mereset `timeDelay` ke nilai awal **100 ms** agar siklus berikutnya dimulai dari kondisi **cepat** lagi. |
| `else { timeDelay += 100; }` | Jika kondisi `if` belum terpenuhi (delay masih di bawah 1100 ms), maka tambahkan 100 ms ke `timeDelay` untuk memperlambat kedipan secara bertahap. |

---

## 5. Logika Perubahan Delay (Cepat → Sedang → Mati)

Berikut ilustrasi perjalanan nilai `timeDelay` dari awal hingga reset:

```
Siklus 1  → timeDelay = 100 ms  → LED berkedip SANGAT CEPAT
Siklus 2  → timeDelay = 200 ms  → LED berkedip CEPAT
Siklus 3  → timeDelay = 300 ms  → LED berkedip AGAK CEPAT
Siklus 4  → timeDelay = 400 ms  → LED berkedip SEDANG
Siklus 5  → timeDelay = 500 ms  → LED berkedip SEDANG
...
Siklus 10 → timeDelay = 1000 ms → LED berkedip LAMBAT
Siklus 11 → timeDelay = 1100 ms → ✅ Masuk blok IF
             → LED MATI selama 3 detik
             → timeDelay RESET ke 100 ms
             → Siklus dimulai ulang dari kondisi CEPAT
```

Transisi ini terjadi secara otomatis dan terus berulang selama Arduino aktif, menciptakan efek visual yang dinamis dari **cepat → sedang → mati → cepat → sedang → mati → ...**

---

## 6. Kesimpulan

Dari percobaan dan modifikasi program ini, dapat disimpulkan beberapa hal:

1. **Blok `if`** digunakan untuk menangani kondisi tertentu yang bersifat *khusus* — dalam hal ini, saat LED sudah mencapai kecepatan berkedip paling lambat dan perlu masuk ke fase mati.

2. **Blok `else`** menangani semua kondisi lainnya — yaitu selama proses transisi kecepatan masih berlangsung (cepat menuju lambat).

3. **Fungsi `delay(timeDelay)`** adalah kunci pengatur kecepatan kedip LED. Semakin kecil nilainya, semakin cepat LED berkedip; semakin besar nilainya, semakin lambat.

4. Dengan memodifikasi arah perubahan `timeDelay` (dari pengurangan menjadi penambahan) dan menyesuaikan nilai ambang kondisi `if`, alur perilaku LED dapat diubah sepenuhnya — dari *lambat ke cepat* menjadi *cepat ke lambat hingga mati*.

5. Percabangan `if-else` merupakan fondasi penting dalam pemrograman mikrokontroler untuk membuat sistem yang mampu **merespons kondisi secara dinamis** tanpa campur tangan pengguna.

---

*Laporan ini dibuat sebagai bagian dari Tugas Pendahuluan dan Pertanyaan Praktikum Modul I – Sistem Tertanam dan Mikrokontroler, Universitas Jenderal Soedirman.*