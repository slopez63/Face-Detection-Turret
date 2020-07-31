
#include <Servo.h>

//Pin Layout
const int servoXOUT = 3;
const int servoYOUT = 11;

const int motorEN1 = 5;
const int motorOUT1 = 6;
const int motorOUT2 = 7;

const int motorEN2 = 10;
const int motorOUT3 = 8;
const int motorOUT4 = 9;

const int ledOUT = 12;


//Servo Objects
Servo servoX;  // Yaw
Servo servoY;  // Pitch

//Initial Servo Positions
int servoXPosition = 90;
int servoYPosition = 90;

//Storing data structure
const int NUMBER_OF_FIELDS = 3;
int fieldIndex = 0;
int values[NUMBER_OF_FIELDS]; 
int value = 0;
int sign = 1;

//Trigger
int trigger = 0;

//Serial Communication
boolean reading = false;

void setup() {
  //Attach Servos
  servoX.attach(servoXOUT);  
  servoY.attach(servoYOUT);

  //Motor pin modes
  pinMode(motorEN1, OUTPUT);
  pinMode(motorOUT1, OUTPUT);
  pinMode(motorOUT2, OUTPUT);
  pinMode(motorOUT3, OUTPUT);
  pinMode(motorOUT4, OUTPUT);
  pinMode(motorEN2, OUTPUT);
  pinMode(ledOUT, OUTPUT);
  
  digitalWrite(ledOUT, HIGH);
  
  analogWrite(motorEN1, 0);
  analogWrite(motorEN2, 0);
  digitalWrite(motorOUT1, LOW);
  digitalWrite(motorOUT2, LOW);
  digitalWrite(motorOUT3, LOW);
  digitalWrite(motorOUT4, LOW);

  //Serial monitor for debug
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()){
    receiveSerial();
  }

  //if(!reading){
    int trigger = values[2];

    if(trigger == 1){
      turnOnMotor1();
      digitalWrite(ledOUT, HIGH);
      //turnOnMotor2();
    }else{
      turnOffMotor1();
      digitalWrite(ledOUT, LOW);
      //turnOffMotor2();
    }

    changeServoValue('X', values[0]);
    changeServoValue('Y', values[1]);
    delay(20);
  //}

}

void receiveSerial(){
  char ch = Serial.read();
  
  if(!reading && ch == 'A'){
    reading = true;
  }else if(reading && ch == 'F'){
    reading = false;
  }else if(reading){
    if(ch >= '0' && ch <= '9'){
     value = (value * 10) + (ch - '0');
    }else if(ch == '-'){
      sign = -1;
    }else{
      //Finalize value and add it to the array
      value = value * sign;
      values[fieldIndex] = value;

      //Change to next field
      if(fieldIndex == NUMBER_OF_FIELDS - 1){
        fieldIndex = 0;
      }else{
        fieldIndex++;
      }

      //Reset variables
      value = 0;
      sign = 1;
    }
  }
  
}

void turnOnMotor1(){
  digitalWrite(motorOUT1, LOW);
  digitalWrite(motorOUT2, HIGH);
  analogWrite(motorEN1, 200);
}

void turnOffMotor1(){
  digitalWrite(motorOUT1, LOW);
  digitalWrite(motorOUT2, LOW);
  analogWrite(motorEN1, 0);
}

void turnOnMotor2(){
  digitalWrite(motorOUT3, LOW);
  digitalWrite(motorOUT4, HIGH);
  analogWrite(motorEN2, 100);
}

void turnOffMotor2(){
  digitalWrite(motorOUT3, LOW);
  digitalWrite(motorOUT4, LOW);
  analogWrite(motorEN2, 0);
}

void changeServoValue(char angle, int value){
  if(value == -1 || value == 1){
    setNewServoState(angle, value);
  }else if(value == -9){
    resetServos();
  }
}

void setNewServoState(char angle, int value){
  if(angle == 'X'){
    if(servoXPosition + value >= 0 && servoXPosition + value <= 180){
      servoXPosition += value;
      servoX.write(servoXPosition);
    }
  }else if(angle == 'Y'){
    if(servoYPosition + value >= 0 && servoYPosition + value <= 180){
      servoYPosition += value;
      servoY.write(servoYPosition);
    }
  }
}

void resetServos(){
  //Slowly turn until both servos are at 90 (CENTER)
  while((servoXPosition < 90 || servoXPosition > 90) || (servoYPosition < 90 || servoYPosition > 90)){
    if(servoXPosition < 90){
      servoXPosition = servoXPosition + 1;
      servoX.write(servoXPosition);
    }

    if(servoXPosition > 90){
      servoXPosition = servoXPosition - 1;
      servoX.write(servoXPosition);
    }

    if(servoYPosition < 90){
      servoYPosition = servoYPosition + 1;
      servoY.write(servoYPosition);
    }

    if(servoYPosition > 90){
      servoYPosition = servoYPosition - 1;
      servoY.write(servoYPosition);
    }

    delay(15);
  }

}
