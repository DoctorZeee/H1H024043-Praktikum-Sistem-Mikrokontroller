# Modul I – Percabangan dan Perulangan
## Percobaan 2A: Perulangan (for) – LED Running

**Mata Kuliah:** Sistem Tertanam dan Mikrokontroler  
**Universitas:** Universitas Jenderal Soedirman  

---

## Deskripsi Singkat
Percobaan ini membahas penggunaan perulangan `for` untuk mengontrol beberapa LED pada Arduino. Dengan memanfaatkan perulangan, LED dapat dinyalakan secara berurutan maupun berkelompok tanpa penulisan kode yang berulang.

---

## Rangkaian
LED dihubungkan ke pin 2–7 Arduino melalui resistor 220Ω, kemudian ke GND. Semua LED menggunakan jalur ground yang sama.

---

## LED Running (Kiri → Kanan dan Kanan → Kiri)

Efek LED berjalan dibuat dengan dua perulangan: satu maju (increment) dan satu mundur (decrement).

```cpp
int timer = 100;

void setup() {
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }
}

void loop() {
  // kiri → kanan
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
  }

  // kanan → kiri
  for (int ledPin = 7; ledPin >= 2; ledPin--) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
  }
}
