/*-------------------------------------
  Module Name : Humid_Control.ino

  REVISION HISTORY in GitHub
  Link : https://github.com/embedded-weathering-with-you/my_name_is_sunny

  Description : 
  This module is the implementation code 
  for the Smart Teru Teru Bozu Bot project presented 
  in the Embedded Software lecture.
-------------------------------------*/

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

// 객체 생성
Servo myServo;
LedControl lc = LedControl(MAX7219_DIN_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, 1);
DHT11 dht(DHT_PIN); // DHT11 객체 생성 및 핀 번호 전달

void setup() {
  // 서보모터 초기화
  myServo.attach(SERVO_PIN);
  myServo.write(0); // 초기 위치

  // MAX7219 초기화
  lc.shutdown(0, false); // 모듈 활성화
  lc.setIntensity(0, 8);  // 밝기 설정 (0~15)
  lc.clearDisplay(0);     // 화면 초기화
  displayFace(0);         // 초기 표정 표시 (중립)

  // 시리얼 모니터 초기화
  Serial.begin(9600);
}

void loop() {
  // DHT11 습도 센서 데이터 읽기
  int temperature, humidity;
  int result = dht.readTemperatureHumidity(temperature, humidity);

  if (result == 0) { // 데이터 읽기 성공
    // 시리얼 모니터에 출력
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    // 습도에 따라 얼굴 및 서보모터 동작 변경
    if (humidity >= 60) {
      displayFace(2); // 우는 표정
      myServo.write(180); // 180도 이동
    } else if (humidity >= 30) {
      displayFace(0); // 무표정
      myServo.write(0); // 0도 이동
    } else {
      displayFace(1); // 웃는 표정
      myServo.write(0); // 0도 이동
    }
  } else {
    // 에러 메시지 출력
    Serial.print("DHT11 error: ");
    Serial.println(dht.getErrorString(result));
  }

  delay(1000); // 데이터 업데이트 주기
}

// 8*8 도트 매트릭스 표정 함수
void displayFace(int faceType) {
  // 표정 데이터
  byte neutralFace[8] = { // 무표정
    B00000000,
    B00001110,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B00001110,
    B00000000
  };

  byte smileFace[8] = { // 웃는 얼굴
    B00000100,
    B00000010,
    B00100100,
    B01000000,
    B01000000,
    B00100100,
    B00000010,
    B00000100
  };

  byte sadFace[8] = { // 우는 표정
    B00000100,
    B00011100,
    B01000100,
    B00100000,
    B00100000,
    B01000100,
    B00011100,
    B00000100
  };

  // 표정 선택
  byte* selectedFace;
  if (faceType == 2) {
    selectedFace = sadFace`  
  } else if (faceType == 1) {
    selectedFace = smileFace; 
  } else {
    selectedFace = neutralFace; 
  }

  // 도트 매트릭스에에 표정 출력
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, selectedFace[row]);
  }
}
