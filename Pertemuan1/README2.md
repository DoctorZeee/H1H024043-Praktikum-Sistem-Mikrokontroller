# Modul I – Percabangan dan Perulangan
## Percobaan 2A: Perulangan (for) – LED Running

**Mata Kuliah:** Sistem Tertanam dan Mikrokontroler  
**Universitas:** Universitas Jenderal Soedirman  

---

## Deskripsi Singkat
Percobaan ini membahas penggunaan perulangan `for` untuk mengontrol beberapa LED pada Arduino. Dengan perulangan, LED dapat menyala berurutan maupun bolak-balik tanpa penulisan kode berulang.

---

## Rangkaian
LED dihubungkan ke pin **3–7 Arduino** melalui resistor 220Ω, lalu ke GND. Semua LED berbagi jalur ground yang sama.

### Skema Sederhana
Pin 3–7 → Resistor 220Ω → LED → GND

---

## LED Running (Kiri → Kanan dan Kanan → Kiri)

Efek LED berjalan dibuat menggunakan dua perulangan:
- Increment (`++`) → kiri ke kanan  
- Decrement (`--`) → kanan ke kiri  

```cpp
int timer = 100;

void setup() {
  // inisialisasi pin 3–7 sebagai output
  for (int ledPin = 3; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }
}

void loop() {
  // kiri → kanan
  for (int ledPin = 3; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
  }

  // kanan → kiri
  for (int ledPin = 7; ledPin >= 3; ledPin--) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
  }
}
