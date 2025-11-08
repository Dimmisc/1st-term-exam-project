// Пины, к которым подключен сдвиговый регистр
const int latchPin = 8;  // Пин STCP (12)
const int clockPin = 12; // Пин SHCP (11)
const int dataPin = 11;  // Пин DS (14)

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  // Зажжем все светодиоды по очереди
  for (int i = 0; i < 8; i++) {
    // Готовим байт данных (например, 00000001, потом 00000010 и т.д.)
    byte ledState = 1 << i; // Сдвигаем единицу на i позиций
    updateShiftRegister(ledState);
    delay(100);
  }
}

// Функция для отправки данных в регистр
void updateShiftRegister(byte data) {
  // Устанавливаем latchPin в LOW перед отправкой данных
  digitalWrite(latchPin, LOW);
  
  // Отправляем 8 бит данных
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  
  // Устанавливаем latchPin в HIGH, чтобы "защелкнуть" данные на выходах
  digitalWrite(latchPin, HIGH);
}
