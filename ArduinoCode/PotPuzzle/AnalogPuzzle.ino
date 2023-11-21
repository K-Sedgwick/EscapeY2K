// This is a testing program for the microservo and solenoid latch system

//#include <Servo.h> 

// constants won't change. They're used here to set pin numbers:
// THESE PINS ARE MULTI-INPUT ON A BREAD BOARD
// Because the arduino does not have enough binary pins to use, we need to have several ones and 
// zeroes grouped together on a breadboard or circuit board to properly signal high or low.
const int digits_1 = 5;
const int digits_2 = 6;
const int digits_3 = 7;
//const int digit_4 = 5;
//const int digit_5 = 2;
//const int xero_1 = 8;
//const int xero_2 = 9;
//const int xero_3 = 10;
//const int xero_4 = 11;
const int out = 13;

int read0 = 0;
int read1 = 0;
int read2 = 0;
int read3 = 0;

void setup() {
  // put your setup code here, to run once:
  // initialize the pushbutton pin as an input:
  pinMode(digits_1, INPUT);
  pinMode(digits_2, INPUT);
  //pinMode(digit_3, INPUT);
  //pinMode(digit_4, INPUT);
  //pinMode(digit_5, INPUT);
  //pinMode(digit_6, INPUT);
  //pinMode(xero_1, INPUT);
  //pinMode(xero_2, INPUT);
  //pinMode(xero_3, INPUT);
  pinMode(out, OUTPUT);

  digitalWrite(out, LOW);
}

void loop() {
  read0 = analogRead(A0);
  read1 = analogRead(A1);
  read2 = analogRead(A2);
  read3 = analogRead(A3);
  // put your main code here, to run repeatedly:

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  //if (digitalRead(digit_1))
  //{
    delay(10);
    if ((digitalRead(digits_1) == HIGH && digitalRead(digits_2) == HIGH && digitalRead(digits_3) == HIGH) && read0 < 900 && read1 < 900 && read2 < 900 && read3 < 900) {
      digitalWrite(out, HIGH);
    }
    else {
      digitalWrite(out, LOW);
    }
  //}
}
