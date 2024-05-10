#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;
constexpr uint8_t SERVO_PIN = D2;
constexpr uint8_t IR_PIN_1 = D1;
constexpr uint8_t IR_PIN_2 = D0;

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myservo.attach(SERVO_PIN);
  pinMode(IR_PIN_1, INPUT);
  pinMode(IR_PIN_2, INPUT);
}

void loop() {
  int parking1 = digitalRead(IR_PIN_1);
  int parking2 = digitalRead(IR_PIN_2);
  bool gate = false;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Card UID : ");
    Serial.println(" ");
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    if(parking1 || parking2){
      gate = true;
      Serial.println(" ");
      Serial.println("Welcome");
    }
    else{
      Serial.println("Parking Occupied");
    }
  }
  if (gate) {
    myservo.write(90);
    delay(5000);
    myservo.write(0);
    delay(5000);
  }
}
