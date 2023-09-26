// Rotary Encoder Inputs
#define A 2
#define B 3
#define BUTTON 4

int counter = 0;
int currentStateA;
int lastStateA;
String currentDir ="";
unsigned long lastButtonPress = 0;

//NOTE: The way this puzzle is set up requires the first number to be from
// a clockwise rotation, then counterclockwise, then clockwise.
//Keep that in mind when chosing values

//Data values for the puzzle logic
int checkpoints[2] {0, 0};
int firstVal = 35;
int secondVal = -28;
int thirdVal = 12;

void setup() {
        
	// Set encoder pins as inputs
	pinMode(A,INPUT);
	pinMode(B,INPUT);
	pinMode(BUTTON, INPUT_PULLUP);

	// Setup Serial Monitor
	Serial.begin(9600);

	// Read the initial state of A
	lastStateA = digitalRead(A);
}

void loop() {
        
	// Read the current state of A
	currentStateA = digitalRead(A);

	// If last and current state of A are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateA != lastStateA  && currentStateA == 1){

    //Code that takes care of updating counter and direction
		if (digitalRead(B) != currentStateA) {
			counter ++;
			currentDir ="CCW";

      //Checks to see if the players are rotating the correct direction and hit the correct value
      if(checkpoints[0] == 0 && counter == firstVal){
        Serial.print("First checkpoint passed");
        checkpoints[0] = 1;
      }
      //Check to see if we should reset the checkpoints
      else if(checkpoints[1] == 0 && counter > firstVal){
        Serial.print("Checkpoints reset");
        checkpoints[0] = 0;
        checkpoints[1] = 0;
      }
      else if(checkpoints[1] == 1 && counter == thirdVal){
        Serial.print("You did it!");
      }
		} else {
			// Encoder is rotating CW so increment
			counter --;
			currentDir ="CW";

      if(checkpoints[0] == 1 && counter == secondVal){
        checkpoints[1] = 1;
        Serial.print("Second checkpoint passed");
      }
      //If we already set the second checkpoint and we're going CCW then reset the checkpoints
      else if(checkpoints[1] == 1){
        Serial.print("Checkpoints reset");
        checkpoints[0] = 0;
        checkpoints[1] = 0;
      }
		}
	}

	// Remember last A state
	lastStateA = currentStateA;

	// // Read the button state
	// int btnState = digitalRead(BUTTON);

	// //If we detect LOW signal, button is pressed
	// if (btnState == LOW) {
	// 	//if 50ms have passed since last LOW pulse, it means that the
	// 	//button has been pressed, released and pressed again
	// 	if (millis() - lastButtonPress > 50) {
	// 		Serial.println("Button pressed!");
	// 	}

	// 	// Remember last button press event
	// 	lastButtonPress = millis();
	// }

	// Put in a slight delay to help debounce the reading
  delay(1);
}