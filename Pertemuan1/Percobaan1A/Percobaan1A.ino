const int ledPin = 6;
int timeDelay = 1000;
void setup(){
  pinMode(ledPin, OUTPUT);
}

void loop(){
  // Nyalakan LED
  digitalWrite(ledPin, HIGH);
  delay(timeDelay);
  // Matikan LED
  digitalWrite(ledPin, LOW);
  delay(timeDelay);
  // Baru ubah delay (setelah 1 siklus kedip)
  if (timeDelay <= 100)
  {
    delay(3000);
    // jeda sebelum reset
    timeDelay = 1000;
    // reset ke awal
  }
  else
  {
    timeDelay -= 100;
    // percepatan bertahap
  }
}