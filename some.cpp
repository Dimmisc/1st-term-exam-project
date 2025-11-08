// Подключаем необходимые библиотеки
#include <AFMotor.h> // Библиотека для управления драйвером моторов
#include <Servo.h>   // Библиотека для управления сервоприводом

// ---- Определение пинов и констант ----
#define GREEN_LED_PIN 7 // Пин для зеленого светодиода
#define RED_LED_PIN   8 // Пин для красного светодиода
#define BUTTON_PIN    2 // Пин для кнопки
#define SERVO_PIN    10 // Пин для сервопривода (должен быть с ШИМ)

#define MOTOR_SPEED 200 // Скорость моторов (0-255)
#define TURN_DELAY  700 // Время для поворота направо (в мс), подбирается экспериментально

// ---- Создание объектов для управления устройствами ----
AF_DCMotor motorLeft(1);  // Левый мотор на клемме M1
AF_DCMotor motorRight(2); // Правый мотор на клемме M2
Servo lidarServo;         // Объект для сервопривода "лидара"

// ---- Глобальные переменные для отслеживания состояния ----
bool isRunning = false; // Флаг, показывающий, запущена ли программа

// Переменные для неблокирующей работы (без использования delay())
unsigned long lastBlinkTime = 0; // Время последнего мигания светодиода
bool greenLedState = false;      // Текущее состояние зеленого светодиода

unsigned long lastServoMoveTime = 0; // Время последнего движения серво
int servoAngle = 90;                 // Текущий угол серво
int servoDirection = 5;              // Направление и шаг изменения угла

void setup() {
  // Настройка пинов
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Кнопка с внутренним подтягивающим резистором

  // Подключаем сервопривод к пину
  lidarServo.attach(SERVO_PIN);
  lidarServo.write(90); // Устанавливаем серво в центральное положение

  // Устанавливаем скорость моторов
  motorLeft.setSpeed(MOTOR_SPEED);
  motorRight.setSpeed(MOTOR_SPEED);
  
  // ---- Начальное состояние (Режим ожидания) ----
  digitalWrite(GREEN_LED_PIN, HIGH); // Постоянно горит зеленый светодиод
  digitalWrite(RED_LED_PIN, LOW);    // Красный выключен
}

void loop() {
  // Проверяем, была ли нажата кнопка и программа еще не запущена
  if (!isRunning && digitalRead(BUTTON_PIN) == LOW) {
    // Небольшая задержка для "антидребезга" кнопки
    delay(50); 
    // Убедимся, что кнопку уже отпустили
    while(digitalRead(BUTTON_PIN) == LOW); 
    
    isRunning = true; // Устанавливаем флаг, что программа запущена
    
    // Вызываем основную функцию движения
    executeMovementSequence();
  }
  
  // Если программа не запущена, мы остаемся в режиме ожидания (зеленый горит)
  // Этот код в loop() будет выполняться только до нажатия кнопки.
}

// ---- Основная последовательность действий ----
void executeMovementSequence() {
  // Выключаем зеленый светодиод (он начнет мигать)
  digitalWrite(GREEN_LED_PIN, LOW);

  // 1. Движение вперёд 4 секунды
  // Во время этого движения мигает светодиод и работает сервопривод
  moveForwardFor(4000);

  // 2. Поворот направо
  turnRight();

  // 3. Движение вперёд 4 секунды
  moveForwardFor(4000);

  // 4. Остановка
  stopMotors();

  // 5. Окончание программы (световая индикация)
  endSequence();
}

// Функция движения вперед с одновременной работой других систем
void moveForwardFor(unsigned long duration) {
  unsigned long startTime = millis(); // Запоминаем время старта

  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);

  // Цикл работает, пока не пройдет заданное время (duration)
  while (millis() - startTime < duration) {
    handleBlinking(); // Обработка мигания светодиода
    handleServo();    // Обработка работы сервопривода
  }
}

// Функция поворота направо
void turnRight() {
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);
  
  // Во время поворота также обеспечиваем работу индикации
  unsigned long turnStartTime = millis();
  while(millis() - turnStartTime < TURN_DELAY) {
    handleBlinking();
    handleServo();
  }
}

// Функция остановки моторов
void stopMotors() {
  motorLeft.run(RELEASE);  // Отключаем моторы
  motorRight.run(RELEASE);
  digitalWrite(GREEN_LED_PIN, LOW); // Гасим зеленый светодиод
}

// Функция для мигания зеленым светодиодом (неблокирующая)
void handleBlinking() {
  // Проверяем, прошла ли 1 секунда (1000 мс) с последнего мигания
  if (millis() - lastBlinkTime > 1000) {
    lastBlinkTime = millis(); // Обновляем время последнего мигания
    greenLedState = !greenLedState; // Инвертируем состояние (вкл/выкл)
    digitalWrite(GREEN_LED_PIN, greenLedState);
  }
}

// Функция для вращения сервопривода (неблокирующая)
void handleServo() {
  // Проверяем, прошло ли 20 мс с последнего движения
  if (millis() - lastServoMoveTime > 20) {
    lastServoMoveTime = millis(); // Обновляем время
    
    servoAngle += servoDirection; // Изменяем угол
    lidarServo.write(servoAngle); // Посылаем команду серво
    
    // Если достигли края, меняем направление
    if (servoAngle <= 45 || servoAngle >= 135) {
      servoDirection = -servoDirection; // Инвертируем направление
    }
  }
}

// Финальная последовательность
void endSequence() {
  // Мигаем красным светодиодом 3 раза
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED_PIN, HIGH);
    delay(300);
    digitalWrite(RED_LED_PIN, LOW);
    delay(300);
  }

  // Включаем красный светодиод на 5 секунд
  digitalWrite(RED_LED_PIN, HIGH);
  delay(5000);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Бесконечный цикл, чтобы программа не началась заново
  while(true);
}
