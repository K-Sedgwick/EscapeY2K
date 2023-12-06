int read1 = 0;
int read2 = 0;
int read3 = 0;
int out1 = LOW;
int out2 = LOW;
int out3 = LOW;

const int LED_initial = 8;
const int LED_2 = 9;
const int LED_3 = 10;
const int LED_4 = 11;
const int LED_final = 12;

const int output = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_initial, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_final, OUTPUT);

  digitalWrite(LED_initial, HIGH);
  digitalWrite(LED_2, out1);
  digitalWrite(LED_3, out2);
  digitalWrite(LED_4, out3);
  digitalWrite(LED_final, LOW);
  digitalWrite(output, HIGH;)
}

void loop() {
  // put your main code here, to run repeatedly:
  read1 = analogRead(A0);
  read2 = analogRead(A1);
  read3 = analogRead(A2);

  Serial.println(read1);
  Serial.println(read2);
  Serial.println(read3);
  Serial.println("");
  Serial.println(out1 && out2 && out3);
  Serial.println("");

  if(read1 > 400 && read1 < 500)
  {
    out1 = HIGH;
  }
  else
  {
    out1 = LOW;
  }

  digitalWrite(LED_2, out1);

  if(read2 > 600 && read2 < 700)
  {
    out2= HIGH;
  }
  else
  {
    out2 = LOW;
  }

  digitalWrite(LED_3, out2);
  delay(50);

  if(read3 > 700 && read3 < 800)
  {
    out3= HIGH;
  }
  else
  {
    out3 = LOW;
  }

  digitalWrite(LED_4, out3);

  if(out1 && out2 && out3)
  {
    digitalWrite(LED_final, HIGH);
    digitalWrite(output, LOW);
  }
  else
  {
    digitalWrite(LED_final, LOW);
    digitalWrite(output, HIGH);
  }

}
