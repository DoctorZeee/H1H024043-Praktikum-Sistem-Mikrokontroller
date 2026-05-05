# 🔬 Praktikum 4 — Kendali Servo & LED dengan Potensiometer (PWM & ADC)

# 🔄 Bagian 4A — Kendali Sudut Servo dengan Potensiometer

## 🖼️ Foto Rangkaian

### Foto Rangkaian Fisik
![Foto Rangkaian 4A]<img width="2160" height="2880" alt="IMG_4185" src="https://github.com/user-attachments/assets/8a848cbe-2a8f-4c07-9303-132b897876d6" />


---

## ❓ Jawaban Pertanyaan 4A

### 1. Apa fungsi perintah `analogRead()` pada rangkaian praktikum ini?

Perintah `analogRead()` berfungsi untuk **membaca tegangan analog** dari pin input Arduino yang terhubung ke **potensiometer**.

Arduino memiliki konverter **ADC (Analog-to-Digital Converter)** 10-bit bawaan yang mengubah tegangan input (0V – 5V) menjadi nilai integer **0 sampai 1023**, merepresentasikan posisi poros potensiometer:

- Poros paling kiri → `0`
- Poros paling kanan → `1023`

```cpp
int nilaiADC = analogRead(A0);  // Menghasilkan nilai 0 – 1023
```

Nilai ADC ini digunakan sebagai **acuan** untuk menentukan sudut putaran servo motor.

---

### 2. Mengapa diperlukan fungsi `map()` dalam program tersebut?

Fungsi `map()` diperlukan karena **rentang nilai ADC dan rentang sudut servo berbeda**, sehingga perlu dilakukan pemetaan linear.

| Parameter | Rentang |
|-----------|---------|
| ADC (`analogRead()`) | 0 – 1023 |
| Sudut Servo (`servo.write()`) | 0° – 180° |

Tanpa `map()`, nilai ADC hingga 1023 akan langsung dikirim ke servo padahal maksimum sudut yang valid hanya 180° — ini bisa menyebabkan perilaku tidak terduga atau kerusakan servo.

```cpp
int sudut = map(nilaiADC, 0, 1023, 0, 180);
```

$$\text{Sudut} = \frac{\text{ADC}}{1023} \times 180$$

**Contoh konversi:**

| Nilai ADC | Sudut Servo |
|-----------|-------------|
| 0         | 0°          |
| 512       | ≈ 90°       |
| 1023      | 180°        |

---

### 3. Modifikasi Program: Servo Hanya Bergerak 30° – 150°

#### 🔧 Program

```cpp
#include <Servo.h>

const int pinPot   = A0;  // Pin potensiometer
const int pinServo = 9;   // Pin sinyal servo

Servo myServo;

void setup() {
  myServo.attach(pinServo);
  Serial.begin(9600);
}

void loop() {
  // Baca nilai ADC dari potensiometer (0 - 1023)
  int nilaiADC = analogRead(pinPot);

  // MODIFIKASI: Petakan ADC ke rentang sudut 30° – 150°
  int sudut = map(nilaiADC, 0, 1023, 30, 150);

  // Gerakkan servo ke sudut hasil pemetaan
  myServo.write(sudut);

  Serial.print("ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" | Sudut: ");
  Serial.print(sudut);
  Serial.println("°");

  delay(50);
}
```

#### 📖 Penjelasan Program

| Bagian | Penjelasan |
|--------|------------|
| `#include <Servo.h>` | Menyertakan library Servo bawaan Arduino |
| `myServo.attach(pinServo)` | Menghubungkan objek servo ke pin PWM |
| `analogRead(pinPot)` | Membaca posisi potensiometer sebagai nilai 0–1023 |
| `map(nilaiADC, 0, 1023, 30, 150)` | **Inti modifikasi:** memetakan rentang ADC penuh ke sudut 30°–150° |
| `myServo.write(sudut)` | Mengirim perintah servo bergerak ke sudut tertentu |

#### 📊 Tabel Pemetaan Nilai

| Posisi Potensiometer | Nilai ADC | Sudut Servo |
|---------------------|-----------|-------------|
| Minimum (kiri penuh) | 0        | 30°         |
| Tengah               | 512      | ≈ 90°       |
| Maksimum (kanan penuh) | 1023   | 150°        |

> Dengan mengubah parameter output `map()` dari `(0, 180)` menjadi `(30, 150)`, seluruh rentang potensiometer tetap berfungsi normal namun gerakan servo otomatis terbatas antara 30° dan 150° — tanpa perlu kondisi `if` tambahan.

---
---

# 🔆 Bagian 4B — Kendali Kecerahan LED dengan PWM & Potensiometer

## 🖼️ Foto Rangkaian

### Foto Rangkaian Fisik
![Foto Rangkaian 4B]<img width="2160" height="2880" alt="IMG_4189" src="https://github.com/user-attachments/assets/d09394eb-b248-451f-9853-12f4221aa1a4" />


## 🎥 Demo GIF

![Demo LED PWM]<img width="480" height="854" alt="IMG_4188" src="https://github.com/user-attachments/assets/1edc59cb-566f-482f-a997-0b3d6ff82ec5" />


---

## ❓ Jawaban Pertanyaan 4B

### 1. Mengapa LED dapat diatur kecerahannya menggunakan fungsi `analogWrite()`?

Fungsi `analogWrite()` menghasilkan sinyal **PWM (Pulse Width Modulation)** — sinyal digital yang berulang cepat antara HIGH (5V) dan LOW (0V). Kecerahan LED diatur melalui **duty cycle**, yaitu persentase waktu sinyal berada pada kondisi HIGH dalam satu periode.

| Duty Cycle | Nilai PWM | Kecerahan LED |
|------------|-----------|---------------|
| 0%         | 0         | Mati          |
| 50%        | 127       | Setengah terang |
| 100%       | 255       | Penuh terang  |

Karena frekuensi PWM sangat tinggi (~490 Hz), mata manusia tidak menangkap kedipan dan LED terlihat menyala dengan kecerahan yang bervariasi secara halus.

---

### 2. Apa hubungan antara nilai ADC (0–1023) dan nilai PWM (0–255)?

| Parameter | Resolusi | Rentang |
|-----------|----------|---------|
| ADC (`analogRead()`) | 10-bit | 0 – 1023 |
| PWM (`analogWrite()`) | 8-bit  | 0 – 255  |

Karena resolusinya berbeda, diperlukan konversi menggunakan `map()`:

```cpp
int nilaiPWM = map(nilaiADC, 0, 1023, 0, 255);
```

$$\text{PWM} = \frac{\text{ADC}}{1023} \times 255$$

**Contoh konversi:**

| Nilai ADC | Nilai PWM |
|-----------|-----------|
| 0         | 0         |
| 512       | ≈ 128     |
| 1023      | 255       |

---

### 3. Modifikasi Program: LED Hanya Menyala pada Rentang PWM 50–200

#### 🔧 Program

```cpp
const int pinPot = A0;  // Pin potensiometer
const int pinLED = 9;   // Pin LED (harus pin PWM)

void setup() {
  pinMode(pinLED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Baca nilai ADC dari potensiometer (0 - 1023)
  int nilaiADC = analogRead(pinPot);

  // Konversi nilai ADC ke rentang PWM (0 - 255)
  int nilaiPWM = map(nilaiADC, 0, 1023, 0, 255);

  // MODIFIKASI: LED hanya menyala jika PWM berada antara 50 dan 200
  if (nilaiPWM >= 50 && nilaiPWM <= 200) {
    analogWrite(pinLED, nilaiPWM);  // Nyalakan LED sesuai kecerahan
  } else {
    analogWrite(pinLED, 0);         // Matikan LED di luar rentang
  }

  Serial.print("ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" | PWM: ");
  Serial.println(nilaiPWM);

  delay(50);
}
```

#### 📖 Penjelasan Program

| Bagian | Penjelasan |
|--------|------------|
| `analogRead(pinPot)` | Membaca tegangan potensiometer, menghasilkan nilai 0–1023 |
| `map(nilaiADC, 0, 1023, 0, 255)` | Mengonversi nilai ADC 10-bit ke nilai PWM 8-bit secara proporsional |
| `if (nilaiPWM >= 50 && nilaiPWM <= 200)` | Kondisi pembatas: LED hanya aktif di rentang kecerahan sedang |
| `analogWrite(pinLED, nilaiPWM)` | Mengirim sinyal PWM ke LED untuk mengatur kecerahan |
| `analogWrite(pinLED, 0)` | Mematikan LED jika nilai PWM di luar batas (< 50 atau > 200) |

#### 📊 Tabel Perilaku Program

| Kondisi Potensiometer | Nilai ADC   | Nilai PWM   | Status LED              |
|----------------------|-------------|-------------|-------------------------|
| Hampir minimum       | 0 – 199     | 0 – 49      | ❌ Mati                 |
| Rentang sedang       | 200 – 800   | 50 – 200    | ✅ Menyala (bervariasi) |
| Hampir maksimum      | 801 – 1023  | 201 – 255   | ❌ Mati                 |

> LED hanya aktif pada kecerahan **sedang** dan akan mati ketika potensiometer diputar terlalu rendah maupun terlalu tinggi. Ini berguna untuk aplikasi yang membutuhkan rentang operasi tertentu, misalnya lampu yang tidak boleh terlalu redup atau terlalu silau.

---

## 🛠️ Komponen yang Digunakan

| Komponen | 4A | 4B |
|----------|----|----|
| Arduino Uno / Nano | ✅ | ✅ |
| Potensiometer 10kΩ | ✅ | ✅ |
| Servo Motor SG90 | ✅ | ❌ |
| LED | ❌ | ✅ |
| Resistor 220Ω | ❌ | ✅ |
| Breadboard & Kabel jumper | ✅ | ✅ |
