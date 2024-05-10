#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define SENDER_EMAIL "*****" // Use your mail
#define SENDER_PASSWORD "****"// generate the password after 2 step verification
#define RECIPIENT_EMAIL "*****" // enter recievers email
#define RECIPIENT_NAME "Parking"

constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;
constexpr uint8_t SERVO_PIN = D2;
constexpr uint8_t IR_PIN_1 = D1;
constexpr uint8_t IR_PIN_2 = D0;

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo;

const char* ssid = "*****"; // wifi name
const char* password = "******"; // password

enum Transfer_Encoding { enc_7bit, enc_base64 }; // Renamed enum

SMTPSession smtp;

void setup() {
  Serial.begin(115200);
  delay(500);
  SPI.begin();
  rfid.PCD_Init();
  myservo.attach(SERVO_PIN);
  pinMode(IR_PIN_1, INPUT);
  pinMode(IR_PIN_2, INPUT);

  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting to WiFi");  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int parking1 = digitalRead(IR_PIN_1);
  int parking2 = digitalRead(IR_PIN_2);
  bool gate = false;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Card UID:");
    Serial.println("");
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    if(parking1 || parking2){
      gate = true;
      Serial.println("");
      Serial.println("Welcome");
      sendEmail(1);
    }
    else{
      Serial.println("Parking Occupied");
      sendEmail(2);
    }
  }
  if (gate) {
    myservo.write(90);
    delay(5000);
    myservo.write(0);
    delay(5000);
  }
}

void sendEmail(int tempp) {
  ESP_Mail_Session session;
  session.server.host_name = SMTP_SERVER;
  session.server.port = SMTP_PORT;
  session.login.email = SENDER_EMAIL;
  session.login.password = SENDER_PASSWORD;

  SMTP_Message message;
  message.sender.name = "DOOR BELL";
  message.sender.email = SENDER_EMAIL;
  message.subject = "IOT DOORBELL";
  message.addRecipient(RECIPIENT_NAME, RECIPIENT_EMAIL);
  
  String emailBody;
  if (tempp == 1){
    emailBody = "Access Allowed: Parking Available";
  }
  else if(tempp == 2){
    emailBody = "Parking Occupied - Access Denied";
  }

  message.text.content = emailBody.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email: " + smtp.errorReason());
  else
    Serial.println("Email sent successfully!");
}
