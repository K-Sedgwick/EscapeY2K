//Includes the Arduino Stepper Library
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
const int tickSteps = 100;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 11, 9, 10, 8);

void setup() {
    // Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {	
	// Rotate CW quickly at 10 RPM
	myStepper.setSpeed(10);
	// myStepper.step(tickSteps);
  // delay(500);

  myStepper.step(1);
  delay(1);
}
