/*
 * Copy the RFID card data into variables and then 
 * scan the second empty card to copy all the data
 * ----------------------------------------------------------------------------
 * Example sketch/program which will try the most used default keys listed in 
 * https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys to dump the
 * block 0 of a MIFARE RFID card using a RFID-RC522 reader.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <String.h>

#define RST_PIN 5  // Configurable, see typical pin layout above
#define SS_PIN 53  // Configurable, see typical pin layout above
#define LED_1 2
#define LED_2 4
#define LED_LIGHT_MOTION 6
#define MOTION_SENSOR 13
#define DOUT_MQ7 8
#define LED_GAS 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
Servo myServo;

int lightSensorValue;
int temperatureValue;
int motionDetected;
float gasSensorValue = 0;
int limit = 0;
int Celsius = 0;


void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  myServo.attach(3);
  myServo.write(0);
  analogReference(DEFAULT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_LIGHT_MOTION, OUTPUT);
  pinMode(LED_GAS, OUTPUT);
  pinMode(MOTION_SENSOR, INPUT);
  pinMode(DOUT_MQ7, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  //digitalWrite(LED_LIGHT_SENSOR, LOW);
  Serial.println(F("Put your card to the reader..."));
  Serial.println();
}

void loop() {

   lightSensorValue = analogRead(A0);
   motionDetected = digitalRead(MOTION_SENSOR);
   //Serial.println(motionDetected);
  if(lightSensorValue > 100 && motionDetected == 1){
    digitalWrite(LED_LIGHT_MOTION, HIGH);
  }
  else 
  {
    digitalWrite(LED_LIGHT_MOTION, LOW);
  }

  gasSensorValue = analogRead(A2);
  limit = digitalRead(DOUT_MQ7);
     //Serial.print("GAS: ");
   // Serial.println(gasSensorValue);
  if(gasSensorValue == 160){
 
    digitalWrite(LED_GAS, HIGH);
  }
  else{
    digitalWrite(LED_GAS, LOW);
  }

  temperatureValue = analogRead(A1);
    temperatureValue = temperatureValue * 0.48828125;
    Serial.print("Temperature detected : ");
    //Celsius =(int)((temperatureValue - 32) * 5 ) / 9;
    //Serial.print(temperatureValue * 0.48828125);
    Serial.println(temperatureValue);
  //delay(1000);
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID tag : ");
  String content = "";
  byte letter;

  for (byte i = 0; i < 4; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content);
  if (content.substring(1) == "72 BA 29 1E") {
    Serial.println("Authorized access");
    Serial.println();
    delay(500);
    digitalWrite(LED_1, HIGH);
    delay(300);
    myServo.write(180);
    delay(5000);
    myServo.write(0);
    digitalWrite(LED_1, LOW);
  } 
  else {
    Serial.println("Access denied");
    digitalWrite(LED_2, HIGH);
    delay(1000);
    digitalWrite(LED_2, LOW);
  }


  // Dump debug info about the card; PICC_HaltA() is automatically called
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}