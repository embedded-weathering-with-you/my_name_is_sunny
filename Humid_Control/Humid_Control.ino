#include <Adafruit_Sensor.h>

#include <Servo.h>
#include <LedControl.h>
#include <DHT11.h>

// 서보모터 핀
#define SERVO_PIN 9
// MAX7219 핀
#define MAX7219_DIN_PIN 11
#define MAX7219_CS_PIN 10
#define MAX7219_CLK_PIN 13
// DHT11 핀
#define DHT_PIN 2
// 버튼 핀
#define BUTTON_PIN 3

// 객체 생성
Servo myServo;
LedControl lc = LedControl(MAX7219_DIN_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, 1);
DHT11 dht(DHT_PIN); // DHT11 객체 생성 및 핀 번호 전달

// 버튼 및 서보 상태 변수
bool buttonPressed = false;
bool servoState = false;

void setup() {
  // 핀 설정
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // 서보모터 초기화
  myServo.attach(SERVO_PIN);
  myServo.write(0); // 초기 위치

  // MAX7219 초기화
  lc.shutdown(0, false); // 모듈 활성화
  lc.setIntensity(0, 8); // 밝기 설정 (0~15)
  lc.clearDisplay(0);    // 초기화
  displayFace();         // ^^ 표정 표시

  // 시리얼 모니터 초기화
  Serial.begin(9600);
  Serial.println("System ready");
}

void loop() {
  // 버튼 상태 확인
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && !buttonPressed) { // 버튼 눌림
    buttonPressed = true;
    if (servoState) {
      myServo.write(0);    // 0도로 이동
      servoState = false;  // 상태 변경
    } else {
      myServo.write(180);  // 180도로 이동
      servoState = true;   // 상태 변경
    }
    delay(500); // 안정화를 위한 딜레이
  } else if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false; // 버튼 상태 초기화
  }

  // DHT11 데이터 읽기
  int temperature, humidity;
  int result = dht.readTemperatureHumidity(temperature, humidity);

  if (result == 0) { // 데이터 읽기 성공
    // 시리얼 모니터에 출력
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  } else {
    // 에러 메시지 출력
    Serial.print("DHT11 error: ");
    Serial.println(dht.getErrorString(result));
  }

  delay(1000); // 데이터 업데이트 주기
}

// MAX7219에 ^^ 표정을 표시하는 함수
void displayFace() {
  byte smile[8] = {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100
  };

  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, smile[row]);
  }
}
