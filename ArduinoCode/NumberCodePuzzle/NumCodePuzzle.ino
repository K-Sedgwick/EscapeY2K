// This is a testing program for the microservo and solenoid latch system

// constants won't change. They're used here to set pin numbers:
// THESE PINS ARE MULTI-INPUT ON A BREAD BOARD
// Because the arduino does not have enough binary pins to use, we need to have several ones and 
// zeroes grouped together on a breadboard or circuit board to properly signal high or low.
const int digit_1 = 4;
const int digit_2 = 5;
const int digit_3 = 6;
const int digit_4 = 7;
const int digit_5 = 8;
const int digit_6 = 9;
const int xero_1 = 2;
const int xero_2 = 3;
const int out = 13;

//int read0 = 0;
//int read1 = 0;
//int read2 = 0;
//int read3 = 0;

void setup() {
  // put your setup code here, to run once:
  // initialize the pushbutton pin as an input:
  pinMode(digit_1, INPUT);
  pinMode(digit_2, INPUT);
  pinMode(digit_3, INPUT);
  pinMode(digit_4, INPUT);
  pinMode(digit_5, INPUT);
  pinMode(digit_6, INPUT);

  pinMode(xero_1, INPUT);
  pinMode(xero_2, INPUT);

  pinMode(out, OUTPUT);

  digitalWrite(out, LOW);
}

void loop() {
//  read0 = analogRead(A0);
//  read1 = analogRead(A1);
//  read2 = analogRead(A2);
//  read3 = analogRead(A3);
  // put your main code here, to run repeatedly:

  delay(10);
  if ((digitalRead(digit_1) == LOW && digitalRead(digit_2) == LOW && digitalRead(digit_3) == LOW && digitalRead(digit_4) == LOW && digitalRead(digit_5) == LOW && digitalRead(digit_6) == LOW) && (digitalRead(xero_1) == HIGH && digitalRead(xero_2) == HIGH)) {
    digitalWrite(out, HIGH);
  }
  else {
    digitalWrite(out, LOW);
  }
}
