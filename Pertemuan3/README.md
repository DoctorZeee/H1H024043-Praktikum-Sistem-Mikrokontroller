# README — Modul 3: Protokol Komunikasi (UART & I2C)
### Sistem Tertanam dan Mikrokontroler
**Universitas Jenderal Soedirman — Jurusan Informatika, Fakultas Teknik**

---

# Percobaan 3A: Komunikasi Serial (UART)

## Tujuan
- Memahami komunikasi serial UART pada Arduino Uno
- Mengendalikan LED via Serial Monitor
- Memahami perbedaan `delay()` vs `millis()` dalam sistem real-time

---

## Alur Data: Keyboard → LED

```
[KEYBOARD] → [SERIAL MONITOR] → [USB/CH340] → [PIN RX ARDUINO]
     → [BUFFER SERIAL 64 byte] → [loop(): Serial.read()] → [PIN 8] → [LED]
```

| Tahap | Proses |
|---|---|
| **Input** | User ketik `'1'`/`'0'`/`'2'` di Serial Monitor |
| **Frame UART** | START BIT + 8 bit data + STOP BIT @ 9600 bps |
| **Transmisi** | Via USB → chip CH340/ATmega16U2 → pin RX |
| **Buffer** | Karakter ditampung di RX buffer (64 byte) |
| **Program** | `Serial.read()` ambil karakter → proses logika |
| **Output** | `digitalWrite()` → HIGH (5V) atau LOW (0V) → LED ON/OFF |


---

## Serial.available()

Mengembalikan jumlah byte yang tersedia di buffer serial. **Wajib** dipanggil sebelum `Serial.read()`.

```arduino
if (Serial.available() > 0) {
    char data = Serial.read();
}
```

**Dampak jika dihilangkan:** `Serial.read()` mengembalikan `-1` saat buffer kosong → program terus mencetak `"Perintah tidak dikenal"` secara spam dan memboroskan CPU.

---

## Kode Program (dengan Blink '2')

**Perilaku:**
- `'1'` → LED menyala statis
- `'0'` → LED mati
- `'2'` → LED berkedip terus (non-blocking) sampai ada perintah baru

```cpp
// modul3_serialmonitor_modifikasi.ino

const int PIN_LED = 8;
bool modeBlink = false;
unsigned long waktuSebelumnya = 0;
const long intervalBlink = 500;
bool statusLED = false;

void setup() {
    Serial.begin(9600);
    Serial.println("=== Kontrol LED via Serial Monitor ===");
    Serial.println("Perintah: '1' = ON | '0' = OFF | '2' = BLINK");
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
}

void loop() {
    // Bagian 1: Baca input serial
    if (Serial.available() > 0) {
        char data = Serial.read();

        if (data == '1') {
            modeBlink = false; statusLED = true;
            digitalWrite(PIN_LED, HIGH);
            Serial.println(">> LED ON");

        } else if (data == '0') {
            modeBlink = false; statusLED = false;
            digitalWrite(PIN_LED, LOW);
            Serial.println(">> LED OFF");

        } else if (data == '2') {
            modeBlink = true;
            waktuSebelumnya = millis();
            Serial.println(">> LED BLINK (kirim '0'/'1' untuk berhenti)");

        } else if (data != '\n' && data != '\r') {
            Serial.print(">> Perintah tidak dikenal: '");
            Serial.print(data); Serial.println("'");
        }
    }

    // Bagian 2: Blink non-blocking
    if (modeBlink) {
        unsigned long waktuSekarang = millis();
        if (waktuSekarang - waktuSebelumnya >= intervalBlink) {
            waktuSebelumnya = waktuSekarang;
            statusLED = !statusLED;
            digitalWrite(PIN_LED, statusLED ? HIGH : LOW);
        }
    }
}
```

---

## delay() vs millis()

| Aspek | `delay()` | `millis()` |
|---|---|---|
| Cara kerja | **Blocking** — program berhenti | **Non-blocking** — program terus jalan |
| Responsivitas input | ❌ Input bisa terlewat | ✅ Input selalu terbaca |
| Multi-tasking | ❌ Tidak bisa | ✅ Bisa |
| Kemudahan | ✅ Sederhana | ⚠️ Lebih kompleks |
| Standar industri | ❌ Hanya prototipe | ✅ Embedded system |

**Kesimpulan:** Gunakan `millis()` karena LED harus bisa dihentikan kapan saja. Dengan `delay()`, perintah baru dari Serial diabaikan selama fase jeda berlangsung.

---

# Percobaan 3B: Inter-Integrated Circuit (I2C)

## Tujuan
- Memahami protokol I2C antara Arduino dan LCD
- Membaca nilai analog potensiometer via ADC
- Menampilkan data sebagai bar visual di LCD I2C
- Menggabungkan UART (Serial Monitor) dan I2C (LCD) dalam satu sistem

---

## Cara Kerja Komunikasi I2C

**Analogi:** Resepsionis (Arduino/Master) menelepon unit apartemen tertentu (LCD/Slave, alamat `0x27`) melalui satu saluran bersama.

| Jalur | Fungsi | Pin Arduino Uno |
|---|---|---|
| **SDA** | Data dua arah | A4 |
| **SCL** | Clock sinkronisasi | A5 |

**Alur komunikasi:**
1. **START** — Arduino tarik SDA: HIGH → LOW (saat SCL HIGH)
2. **Alamat + R/W** — Kirim `0x27` + bit Write
3. **ACK** — LCD balas: "saya siap"
4. **DATA** — Kirim byte karakter/perintah
5. **STOP** — Arduino tarik SDA: LOW → HIGH

Library `LiquidCrystal_I2C` menyederhanakan semua ini menjadi `lcd.print()`, `lcd.setCursor()`, dll.

---

## Potensiometer: Fungsi Pin dan Efek Jika Tertukar

Potensiometer bekerja sebagai **voltage divider** (pembagi tegangan).

| Kaki | Koneksi | Fungsi |
|---|---|---|
| Kiri | GND (0V) | Referensi bawah |
| Tengah (Wiper) | Pin A0 | Output tegangan variabel (0–5V) |
| Kanan | 5V (VCC) | Referensi atas |

**Jika kaki kiri dan kanan tertukar:**
- Nilai ADC **terbalik** → putar kiri = nilai besar, putar kanan = nilai kecil
- Bar di LCD **terbalik** arahnya
- Hardware **tidak rusak** (hanya masalah logika/orientasi)


---

## Kode Program (UART + I2C)

```cpp
// modul3_i2c_uart_gabungan.ino

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat 0x27, LCD 16x2
const int pinPot = A0;
const float TEGANGAN_REF = 5.0;
const int ADC_MAX = 1023;
const int BAR_MAX = 16;

void setup() {
    Serial.begin(9600);
    Serial.println("========================================");
    Serial.println(" Sistem Monitor Potensiometer (ADC)");
    Serial.println("Format: ADC: xxx  Volt: x.xx V  Persen: xx%");
    Serial.println("========================================");

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("  Memulai...    ");
    delay(1000);
    lcd.clear();
}

void loop() {
    // Baca ADC
    int nilaiADC = analogRead(pinPot);
    float tegangan = (nilaiADC / (float)ADC_MAX) * TEGANGAN_REF;
    int persen = map(nilaiADC, 0, ADC_MAX, 0, 100);
    int panjangBar = map(nilaiADC, 0, ADC_MAX, 0, BAR_MAX);

    // Output ke Serial Monitor (UART)
    Serial.print("ADC: "); Serial.print(nilaiADC);
    Serial.print("\tVolt: "); Serial.print(tegangan, 2);
    Serial.print(" V\tPersen: "); Serial.print(persen); Serial.println("%");

    // Baris 1 LCD: nilai ADC dan persen
    lcd.setCursor(0, 0);
    lcd.print("ADC:"); lcd.print(nilaiADC); lcd.print("  ");
    lcd.setCursor(8, 0);
    lcd.print(persen); lcd.print("% ");

    // Baris 2 LCD: bar level visual
    lcd.setCursor(0, 1);
    for (int i = 0; i < BAR_MAX; i++) {
        lcd.print(i < panjangBar ? (char)255 : ' ');
    }

    delay(200); // ~5 update per detik, cukup responsif
}
```

**Contoh output Serial Monitor:**
```
ADC: 512    Volt: 2.50 V    Persen: 50%
```

**Tampilan LCD:**
```
┌────────────────┐
│ADC:512  50%    │
│████████        │
└────────────────┘
```

---

## Tabel Hasil Pengamatan

Formula: `Volt = (ADC / 1023) × 5.0` | `Persen = (ADC / 1023) × 100`

| ADC | Volt (V) | Persen (%) | Kondisi Potensiometer |
|:---:|:---:|:---:|:---|
| 1 | 0.00 | 0% | Paling kiri (hampir GND) |
| 49 | 0.24 | 5% | ~5% dari rentang |
| 256 | 1.25 | 25% | Seperempat putaran |
| 512 | 2.50 | 50% | Posisi tengah |
| 768 | 3.75 | 75% | Tiga perempat putaran |
| 1023 | 5.00 | 100% | Paling kanan (5V penuh) |

> **Catatan:** Variasi ±1–3 ADC pada hardware nyata adalah normal akibat noise dan toleransi komponen.

---
