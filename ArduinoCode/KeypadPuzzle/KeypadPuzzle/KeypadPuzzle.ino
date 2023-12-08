#include <Keypad.h>

const int ledZero = A0;
const int ledOne = A1;
const int ledTwo = A2;
const int ledThree = A3;
const int ledFour = A4;

//Pull these pins low when we want to talk to the ESP to tell it there was a wrong/right combo.
const int wrongPin = 10;
const int correctPin = 11;
const int resetPin = 12;

int resetVal = HIGH;
bool resetTracker = false;

#define numLEDS 4
int ledPins [numLEDS] = {ledZero, ledOne, ledTwo, ledThree};

const byte ROWS = 4; 
const byte COLS = 4;
char previousKey = ' ';
int charsEntered = 0;
char combo [numLEDS] = {' ', ' ', ' ', ' '};


#define numCombos 10
// These are the combos that are correct!
char successfulCombos[numCombos][numLEDS] = {
  {'1', '2', '3', '4'},
  {'2', '3', '9', '2'},
  {'7', '3', '9', '5'},
  {'6', '5', '3', '0'},
  {'7', '2', '1', '0'},
  {'4', '6', '8', '4'},
  {'3', '5', '1', '4'},
  {'1', '0', '7', '1'},
  {'7', '6', '2', '4'},
  {'1', '8', '5', '0'},
};

char successfulCombosCOPY[numCombos][numLEDS] = {
  {'1', '2', '3', '4'},
  {'2', '3', '9', '2'},
  {'7', '3', '9', '5'},
  {'6', '5', '3', '0'},
  {'7', '2', '1', '0'},
  {'4', '6', '8', '4'},
  {'3', '5', '1', '4'},
  {'1', '0', '7', '1'},
  {'7', '6', '2', '4'},
  {'1', '8', '5', '0'},
};

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 200;
int ledStep = 0;

// This is the layout of the pad
char hexaKeys[ROWS][COLS] = {
  {'A', '1', '2', '3'},
  {'B', '4', '5', '6'},
  {'C', '7', '8', '9'},
  {'D', '*', '0', '#'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);

  pinMode(ledZero, OUTPUT);
  pinMode(ledOne, OUTPUT);
  pinMode(ledTwo, OUTPUT);
  pinMode(ledThree, OUTPUT);
  pinMode(ledFour, OUTPUT);

  pinMode(wrongPin, OUTPUT);
  digitalWrite(wrongPin, HIGH);
  pinMode(correctPin, OUTPUT);
  digitalWrite(correctPin, HIGH);
  pinMode(resetPin, INPUT_PULLUP);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    handlePressLogic(customKey);
  }

  resetVal = digitalRead(resetPin);
  if(resetVal == LOW && resetTracker == false){
    int l = 0;
    int k = 0;
    for(l = 0; l < numCombos; l++){
      for(k = 0; k < numLEDS; k++){
        successfulCombos[l][k] = successfulCombosCOPY[l][k];
      }
    }

    resetTracker = true;
  }
  else{
    resetTracker == false;
  }

  //This is just for ticking
  if ((millis() - previousTime) > TickDelayTime)
  {
    previousTime = millis();

    if(charsEntered == numLEDS){
      loadToFull();
    }
  }
}

void handlePressLogic(char pressedKey){
  if(pressedKey != previousKey){
    //Now we handle the combination logic stuff
    //* means reset
    if(pressedKey == '*'){
      Serial.println("Reset");
      reset();
    }
    //# means submit
    else if(pressedKey == '#' && charsEntered == numLEDS){
      Serial.println("Send data");
      bool success = checkCombo();
      if(success == true){
        Serial.println("YAY");
        digitalWrite(correctPin, LOW);
        // Help with signal debouncing
        delay(5);
        digitalWrite(correctPin, HIGH);
      }
      else{
        Serial.println("WRONG");
        digitalWrite(wrongPin, LOW);
        // Help with signal debouncing
        delay(5);
        digitalWrite(wrongPin, HIGH);
      }
      reset();
    }
    else if(charsEntered < numLEDS){
      Serial.println(pressedKey);
      previousKey = pressedKey;
      combo[charsEntered] = pressedKey;
      turnLEDOn(ledPins[charsEntered]);
      charsEntered++;
    }
  }
}

// HELPER METHODS
bool checkCombo(){
  Serial.println("COMBO CHECKING");
  bool success = false;
  int i = 0;
  int j = 0;
  int p = 0;
  int successCount = 0;

  //Display combo
  // Serial.print("COMBO: ");
  // for(p = 0; p < numLEDS; p++){
  //   Serial.print(combo[p]);
  // }

  for (i = 0; i < numCombos; i++){
    successCount = 0;
    for(j = 0; j < numLEDS; j++){
      // Serial.print("Comparing: ");
      // Serial.print(successfulCombos[i][j]);
      // Serial.print(" - ");
      // Serial.print(combo[j]);
      if(successfulCombos[i][j] == combo[j]){
        successCount++;
        if(successCount == numLEDS){
          success = true;
          successfulCombos[i][j] = 'u';
        }
        continue;
      }

      break;
    }
    if(success == true){
      break;
    }
    
  }

  return success;
}

void reset(){
  for(int i = 0; i < numLEDS; i++){
    combo[i] = ' ';
  }
  charsEntered = 0;
  ledStep = 0;
  resetLEDS();
}

// ---- LED PATTERN HELPERS ----
void successCombo(){
}

void failCombo(){

}

void resetLEDS(){
  digitalWrite(ledZero, LOW);
  digitalWrite(ledOne, LOW);
  digitalWrite(ledTwo, LOW);
  digitalWrite(ledThree, LOW);
  digitalWrite(ledFour, LOW);
}

void turnLEDOn(int pinNum){
  digitalWrite(pinNum, HIGH);
}

void allOn(){
  digitalWrite(ledZero, HIGH);
  digitalWrite(ledOne, HIGH);
  digitalWrite(ledTwo, HIGH);
  digitalWrite(ledThree, HIGH);
  digitalWrite(ledFour, HIGH);
}

void loadToFull(){
  switch(ledStep){
    case 0:
      resetLEDS();
      ledStep = 1;
      break;
    case 1:
      digitalWrite(ledZero, HIGH);
      ledStep = 2;
      break;
    case 2:
      digitalWrite(ledOne, HIGH);
      ledStep = 3;
      break;
    case 3:
      digitalWrite(ledTwo, HIGH);
      ledStep = 4;
      break;
    case 4:
      digitalWrite(ledThree, HIGH);
      ledStep = 5;
      break;
    case 5:
      digitalWrite(ledFour, HIGH);
      ledStep = 0;
      break;
    default:
      resetLEDS();
      ledStep = 0;
      break;
  }
}

void passTheLight(){
  switch (ledStep) {
    case 0:
      digitalWrite(ledFour, LOW);
      digitalWrite(ledZero, HIGH);
      ledStep = 1;
      break;
    case 1:
      digitalWrite(ledZero, LOW);
      digitalWrite(ledOne, HIGH);
      ledStep = 2;
      break;
    case 2:
      digitalWrite(ledOne, LOW);
      digitalWrite(ledTwo, HIGH);
      ledStep = 3;
      break;
    case 3:
      digitalWrite(ledTwo, LOW);
      digitalWrite(ledThree, HIGH);
      ledStep = 4;
      break;
    case 4:
      digitalWrite(ledThree, LOW);
      digitalWrite(ledFour, HIGH);
      ledStep = 0;
      break;
    // case 5:
    //   digitalWrite(ledFour, LOW);
    //   digitalWrite(ledZero, HIGH);
    //   ledStep = 0;
    //   break;
    default:
      resetLEDS();
      ledStep = 0;
      break;
  }
}