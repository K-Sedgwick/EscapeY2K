#include <Servo.h>

Servo clockServo;         // create servo object to control a servo -- twelve servo objects can be created on most boards.
const int TenHourInput = 2;
const int TwelveHourInput = 3;
const int LedPin = 13;

int servoPosition = 0;
int TEMP = 0;

void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(TenHourInput, INPUT_PULLUP);
  pinMode(TwelveHourInput, INPUT_PULLUP);
  pinMode(LedPin, OUTPUT);

  //Servo attaching code
  clockServo.attach(10); // attaches the servo on pin 9 to the servo object
}

void loop() {
  //read the pushbutton value into a variable
  int TenHourSwitchVal = digitalRead(TenHourInput);
  int TwelveHourSwitchVal = digitalRead(TwelveHourInput);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (TenHourSwitchVal == LOW) {
    //print out the value of the pushbutton
    Serial.println("It's 10 o'clock!");
    digitalWrite(LedPin, LOW);
    
    for (TEMP = 180; servoPosition >= 0; servoPosition -= 1) { // goes from 180 degrees to 0 degrees
      clockServo.write(servoPosition);              // tell servo to go to position in variable 'pos'

      //If the servo gets to the other clock position tell it to stop counting up
      if (digitalRead(TwelveHourInput) == LOW){
        break;
      }

      delay(15);                       // waits 15 ms for the servo to reach the position
    }

    delay(100);
  } else if (TwelveHourSwitchVal == LOW) {
    //print out the value of the pushbutton
    Serial.println("It's 12 o'clock!");
    digitalWrite(LedPin, LOW);

    for (TEMP = 0; servoPosition <= 180; servoPosition += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree

      //If the servo gets to the other clock position tell it to stop counting up
      if (digitalRead(TenHourInput) == LOW){
        break;
      }

      clockServo.write(servoPosition);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }

    delay(100);
  }
  else {
    digitalWrite(LedPin, HIGH);
  }
}
