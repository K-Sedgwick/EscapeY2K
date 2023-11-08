//ESP pins
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define TXPIN 1
#define RXPIN 3

// Rotary Encoder Inputs
#define A D5
#define B D6

int counter = 0;
int currentStateA;
int lastStateA;
String currentDir ="";

void setup() {
  // Setup Serial Monitor
	Serial.begin(9600); 
  
  // Set encoder pins as inputs
	pinMode(A,INPUT);
	pinMode(B,INPUT);

  // Read the initial state of A
	lastStateA = digitalRead(A);
  Serial.println("Setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
  handleRotaryLogic();
}

void handleRotaryLogic() {
        
	// Read the current state of A
	currentStateA = digitalRead(A);

	// If last and current state of A are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateA != lastStateA){

    //Code that takes care of updating counter and direction
		if (digitalRead(B) != currentStateA) {
			counter ++;
			currentDir ="CCW";
      Serial.println("Rotating Counter-Clockwise! Count: " + counter);
		} else {
			// Encoder is rotating CW so increment
			counter --;
			currentDir ="CW";
      Serial.println("Rotating Clockwise! Count: " + counter);
		}
    Serial.print("Direction: " + currentDir + "Count: ");
    Serial.println(counter);
	}

  // Remember last A state
	lastStateA = currentStateA;

	// Put in a slight delay to help debounce the reading
  delay(5);
}
