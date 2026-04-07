# Modul I – Percabangan dan Perulangan
## Percobaan 1A: Percabangan (if-else)

**Mata Kuliah:** Sistem Tertanam dan Mikrokontroler  
**Universitas:** Universitas Jenderal Soedirman  

---

## Deskripsi Singkat
Percobaan ini bertujuan memahami penggunaan percabangan `if-else` pada Arduino. LED digunakan sebagai indikator untuk melihat perubahan perilaku sistem berdasarkan nilai `timeDelay`, sehingga hubungan antara program dan output fisik dapat diamati secara langsung.

---

## Jawaban Pertanyaan

### 1. Kapan masuk blok `if`?
Program masuk ke blok `if` saat `timeDelay <= 100`. Kondisi ini menandakan LED sudah berada pada kecepatan kedip maksimum (sangat cepat), sehingga program melakukan reset agar siklus dapat diulang dari awal.

### 2. Kapan masuk blok `else`?
Program masuk ke blok `else` saat `timeDelay > 100`. Pada kondisi ini, program masih dalam proses perubahan kecepatan, sehingga nilai `timeDelay` terus dikurangi untuk mempercepat kedipan LED.

### 3. Fungsi `delay(timeDelay)`
`delay(timeDelay)` berfungsi untuk memberi jeda eksekusi program dalam milidetik. Nilai ini menentukan durasi LED menyala dan mati. Semakin kecil nilainya, semakin cepat LED berkedip, sehingga perubahan nilai delay langsung mempengaruhi respon sistem.

---

## Modifikasi Program (Cepat → Sedang → Mati)

Program dimodifikasi agar pola LED berubah dari cepat ke lebih lambat, kemudian masuk fase mati sebelum mengulang kembali. Perubahan ini dilakukan dengan menambah nilai `timeDelay` secara bertahap hingga mencapai batas tertentu.

---

## Source Code

```cpp
const int ledPin = 6;
int timeDelay = 100;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(timeDelay);

  digitalWrite(ledPin, LOW);
  delay(timeDelay);

  if (timeDelay >= 1100) {
    digitalWrite(ledPin, LOW);
    delay(3000);
    timeDelay = 100;
  } else {
    timeDelay += 100;
  }
}
