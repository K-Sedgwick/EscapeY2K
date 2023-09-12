// This is a testing program for the microservo and solenoid latch system

#include <Servo.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;  // the number of the pushbutton pin
const int buttonPin2 = 3; // the number of the pushbutton pin
Servo latchServo;         // create servo object to control a servo -- twelve servo objects can be created on most boards.
Servo latchServo2;
const int relayPin = 4;
const int relayPin2 = 7;

int servoPos = 0;  // variable to store the servo position
int servoPos2 = 0; // variable to store the servo position

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int buttonState2 = 0; // variable for reading the pushbutton status

void setup()
{
    // put your setup code here, to run once:
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(buttonPin2, INPUT);
    pinMode(relayPin2, OUTPUT);
    latchServo.attach(9); // attaches the servo on pin 9 to the servo object
    latchServo2.attach(10);
}

void loop()
{
    // put your main code here, to run repeatedly:

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (digitalRead(buttonPin))
    {
        buttonState = !buttonState;
        delay(10);
        if (buttonState == 1)
        {
            servoPos = 90;
            digitalWrite(relayPin, HIGH);
        }
        else
        {
            servoPos = 0;
        }
        latchServo.write(servoPos);
        delay(250);
        digitalWrite(relayPin, LOW);
    }

    if (digitalRead(buttonPin2))
    {
        buttonState2 = !buttonState2;
        delay(10);
        if (buttonState2 == 1)
        {
            servoPos2 = 90;
            digitalWrite(relayPin2, HIGH);
        }
        else
        {
            servoPos2 = 0;
        }
        latchServo2.write(servoPos2);
        delay(250);
        digitalWrite(relayPin2, LOW);
    }
}