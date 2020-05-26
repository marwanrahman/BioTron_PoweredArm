#include <Servo.h>
#include "MyoController.h"

#define FIST_PIN 4
#define WAVEIN_PIN 5
#define WAVEOUT_PIN 6
#define FINGERSSPREAD_PIN 7
#define DOUBLETAP_PIN 8

// create servo object to control a servo
Servo servoI; //Index finger  
Servo servoM; //Middle finger 
Servo servoR; //Ring finger 
Servo servoP; //Pinky finger 
Servo servoT; //Thumb 
// twelve servo objects can be created on most boards

bool isTest = false; //boolean to determine if test functionality should be enabled

MyoController myo = MyoController();

void setup() {
  Serial.begin(9600);
  
  servoI.attach(11);
  servoM.attach(10);
  servoR.attach(9);
  servoP.attach(6);
  servoT.attach(5); // attaches the servo on pin _ to the servo object

  Serial.print("Would you like to enable testing functionality? (enter 'y' or 'n')");
    while (Serial.available()== 0){
      }
    char testInput = Serial.read();
    if(tolower(testInput) == 'y'){
      isTest = true;
      Serial.print("\nTest functionality enabled");
    }
    else if(tolower(testInput) == 'n'){
      isTest = false;

      pinMode(FIST_PIN, OUTPUT);
      pinMode(WAVEIN_PIN, OUTPUT);
      pinMode(WAVEOUT_PIN, OUTPUT);
      pinMode(FINGERSSPREAD_PIN, OUTPUT);
      pinMode(DOUBLETAP_PIN, OUTPUT);
      
      myo.initMyo();

      Serial.print("\nTest functionality disabled");
    }
  
}

void loop() {
    //No testing functionality. Opening and closing hand only
    if(!isTest){ 
      Serial.print("\nPlug in and calibrate the Myo to begin testing");

      myo.updatePose();
      switch ( myo.getCurrentPose() ) {
        case rest:
          digitalWrite(FIST_PIN,LOW); 
          digitalWrite(WAVEIN_PIN,LOW);
          digitalWrite(WAVEOUT_PIN,LOW);
          digitalWrite(FINGERSSPREAD_PIN,LOW);
          digitalWrite(DOUBLETAP_PIN,LOW);
          break;
        case fist:
          digitalWrite(FIST_PIN,HIGH);
          break;
        case waveIn:
          digitalWrite(WAVEIN_PIN,HIGH);
          break;
        case waveOut:
          digitalWrite(WAVEOUT_PIN,HIGH);
          break;
        case fingersSpread:
          digitalWrite(FINGERSSPREAD_PIN,HIGH);
          break;
        case doubleTap:
          digitalWrite(DOUBLETAP_PIN,HIGH);
          break;
      } 
     }
     //Testing functionality. Opening and closing hand, as well as individual fingers
     else if(isTest){ 
      Serial.print("\nenter 'o' to open hand, 'c' to close hand, or 'i', 'm', 'r', 'p', or 't' to move individual fingers");
      //wait for user to enter any value to run servo
      while (Serial.available()== 0){}
      char input = Serial.read();
      if(tolower(input) =='o'){
        Serial.print("\nOpening hand");
        servoI.write(0);
        servoM.write(0);
        servoR.write(0);
        servoP.write(0);
        servoT.write(0);
      }else if(tolower(input) == 'c') {
        Serial.print("\nClosing hand");
        servoI.write(90);
        servoM.write(90);
        servoR.write(90);
        servoP.write(90);
        servoT.write(90);
      }else if(tolower(input) == 'i') {
        if(servoI.read() > 45){
          Serial.print("\nOpening index finger");
          servoI.write(0);
        }else{
          Serial.print("\nClosing index finger");
          servoI.write(90);
        }
      }
      else if(tolower(input) == 'm'){
        if(servoM.read() > 45){
          Serial.print("\nOpening middle finger");
          servoM.write(0);
        }else{
          Serial.print("\nClosing middle finger");
          servoM.write(90);
        }
      }
      else if(tolower(input) == 'r'){
        if(servoR.read() > 45){
          Serial.print("\nOpening ring finger");
          servoR.write(0);
        }else{
          Serial.print("\nClosing ring finger");
          servoR.write(90);
        }
      }
      else if(tolower(input) == 'p'){
        if(servoP.read() > 45){
          Serial.print("\nOpening pinky finger");
          servoP.write(0);
        }else{
          Serial.print("\nClosing pinky finger");
          servoP.write(90);
        }
      }
      else if(tolower(input) == 't'){
        if(servoT.read() > 45){
          Serial.print("\nOpening thumb");
          servoT.write(0);
        }else{
          Serial.print("\nClosing thumb");
          servoT.write(90);
        }
      }
     }

    delay(1000);
    while (Serial.available() > 0) Serial.read();
}
