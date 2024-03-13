#include <Servo.h>
#include <NewPing.h>
#include <BlynkSimpleStream.h>
#include <SoftwareSerial.h>
#include <VoiceRecognitionV3.h>

// Blynk Auth Token
char auth[] = "Your_Blynk_Auth_Token";

// Pins for the ultrasonic sensors and servo
const int LID_TRIG_PIN = 5;
const int LID_ECHO_PIN = 6;
const int FILL_TRIG_PIN = 9;
const int FILL_ECHO_PIN = 10;
const int SERVO_PIN = 7;
const int MAX_DISTANCE = 200; // Maximum distance for ultrasonic sensors (cm)

// Voice Recognition Module Pins
const int VR_RX_PIN = 2; // Connect to TX of Voice Recognition Module
const int VR_TX_PIN = 3; // Connect to RX of Voice Recognition Module

Servo lidServo;
NewPing sonarLid(LID_TRIG_PIN, LID_ECHO_PIN, MAX_DISTANCE);
NewPing sonarFill(FILL_TRIG_PIN, FILL_ECHO_PIN, MAX_DISTANCE);

SoftwareSerial voiceSerial(VR_RX_PIN, VR_TX_PIN); // RX, TX
VR myVR(voiceSerial);

uint8_t records[7]; // Save recognized records
uint8_t recordCount = 7; // Max number of records

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, Serial);

  voiceSerial.begin(9600);
  if (myVR.clear() == 0) {
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("Not found VoiceRecognitionModule.");
    while (1);
  }

  // Load voice commands here, replace '0' and '1' with the actual command indexes you trained
  myVR.load((uint8_t)0); // Load "Open" command
  myVR.load((uint8_t)1); // Load "Close" command

  lidServo.attach(SERVO_PIN);
  lidServo.write(0); // Close the lid by default
}

void loop() {
  Blynk.run();

  int lidDistance = sonarLid.ping_cm();
  int fillDistance = sonarFill.ping_cm();

  // Lid control based on proximity
  if (lidDistance > 0 && lidDistance < 50) {
    lidServo.write(90); // Open the lid
  } else {
    lidServo.write(0); // Close the lid
  }

  // Fill level monitoring and notification
  if (fillDistance > 0 && fillDistance < 20) {
    Blynk.notify("Dustbin is almost full!");
  }

  // Voice command recognition
  int ret = myVR.recognize(records, recordCount);
  if (ret > 0) {
    switch (records[0]) {
      case 0: // Index of "Open" command
        lidServo.write(90);
        break;
      case 1: // Index of "Close" command
        lidServo.write(0);
        break;
      // Add more cases for additional commands
    }
  }

  delay(1000); // Adjust based on required responsiveness and to minimize sensor trigger frequency
}
