// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//9 rotary encoder counts is one loop

// Includes the Arduino Stepper Library
#include <Stepper.h>

//ESP pins
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define TXPIN 1
#define RXPIN 3

// Rotary Encoder Inputs
#define A D5
#define B D6

//Encoder parameters
int rotaryCounter = 0;
int currentStateA;
int lastStateA;
String currentDir ="";

// ---- WIFI SECTION ----
const char *ssid = "EscapeY2K";//EscapeY2K
const char *password = "caNY0u3scAp3?!";//caNY0u3scAp3?!
WiFiServer server(1234);

//IP addresses of the devices that need to know about what time it is
const int numOfMidnightDependentDevices = 1;
String tvIP = "192.168.1.211:8001"; // 10.0.0.64 at Jakes house, 192.168.1.211:8001 on escapeY2K
String dialIP = "192.168.1.225:1234";
String potIP = "192.168.1.59:1234";
String numberPuzzleIP = "";
String midnightDependentIPs[numOfMidnightDependentDevices] = {tvIP};

// ---- ROTARY ENCODER SECTION ----
const int countInOneRotation = 10;
const int midnightCount = 200;

// ---- STEPPER SECTION ----
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// How far to go in one "tick" of the clock
const int tickSteps = 100;
const int stepperSpeed = 10;
const int reverseSteps = -10;
const int fastForwardSteps = 10;
Stepper clockStepper = Stepper(stepsPerRevolution, 3, 4, 5, 2); // Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

// ---- SLOT INTERRUPTOR SECTION ----
const int Interruptor = D3;
const int Interruptor2 = D7;
bool startingPosition = false;
bool midnight = false;
bool timeCurrentlyControlledByUser = false;
bool directionReverse = false;
bool directionFastForward = false;
bool tvAtStart = false;

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 500;
// This is whether we have told all of the devices about midnight that need to know so we don't just keep screaming it into the void
bool informed = false;
bool pause = true;
bool reset = false;
bool initialized = false;
String status = "starting...";

//Help us keep track of which puzzles are enabled and which ones arent
bool dialEnabled = false;
bool potsEnabled = false;
bool numberPuzzleEnabled = false;

// ---- SETUP AND LOOP ----
int numLoops = 0;

void setup()
{
	// start serial connection
	Serial.begin(9600);

	connectToWifi();

	// configure pin 2 and 3 as an input and enable the internal pull-up resistor
	pinMode(Interruptor, INPUT_PULLUP);
  pinMode(Interruptor2, INPUT_PULLUP);

  //Setup pins for stepper
	pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  // Read the initial state of A
	lastStateA = digitalRead(A);

  // Set encoder pins as inputs
	pinMode(A,INPUT);
	pinMode(B,INPUT);

  //Setup pin for LED so we can test stuff
  // pinMode(LedPin, OUTPUT); 

	// Rotate CW quickly at 10 RPM
	// TODO: Figure out how to change direction
	clockStepper.setSpeed(stepperSpeed);
  status = "Paused";
}

void loop()
{
	WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

  //Handle logic that should happen as often as possible
  //But limit it a little bit because it doesnt need to happen so much that it makes the motor chug weird
  if(numLoops > 10){
    numLoops = 0;
    handleRotaryLogic();
    processInterruptorSwitches();
  }

  //Handle reset here because it is a bit different
  if (reset)
  {
    clockStepper.step(reverseSteps); // TODO: Figure out what direction is positive and what is negative
  }

  //If the admin hasnt requested control just operate normally
  else if(!pause){
    //If it's midnight send messages to all of the devices that need to know and dont allow the user to do any more adjusting of the time
    if(midnight){
      if(!informed){
        informed = true;
        //InformAllDevicesOfTime();
        InformAllDevicesMidnight();
      }
    }
    // If the user hasnt selected a different time just tick normally
    else if (!timeCurrentlyControlledByUser && (millis() - previousTime) > TickDelayTime)
    {
      //Serial.print("Tick");
      clockStepper.step(tickSteps);
      previousTime = millis();
    }
    // If they selected reverse, tell the stepper to reverse until they ask to be done
    else if (directionReverse)
    {
      clockStepper.step(reverseSteps); // TODO: Figure out what direction is positive and what is negative
    }
    else if (directionFastForward)
    {
      clockStepper.step(10);
    }
  }
  else{
    //TODO: Decide if we want to add anything special here? Probably not, im assuming we just dont want the clock running rn :P
  }

  numLoops++;
}

// ---- HELPER METHODS ----
void handleClientConnected(WiFiClient rcvClient)
{
	// SETUP VARIABLES
  String header = "";
	String currentLine = ""; // make a String to hold incoming data from the client
	int handleClientTimeout = 2000;
	unsigned long clientConnectedTime = millis();
	int previousClientConnectedTime = clientConnectedTime;

	Serial.println("New Client."); // print a message out in the serial port
	while (rcvClient.connected() && clientConnectedTime - previousClientConnectedTime <= handleClientTimeout)
	{ // loop while the client's connected
		clientConnectedTime = millis();
		if (rcvClient.available())
		{							   // if there's bytes to read from the client,
			char c = rcvClient.read(); // read a byte, then
			//Serial.write(c);		   // print it out the serial monitor
			header += c;
			if (c == '\n')
			{ // if the byte is a newline character
				// if the current line is blank, you got two newline characters in a row.
				// that's the end of the client HTTP request, so send a response:
				if (currentLine.length() == 0)
				{
					String messageStart = "{\"rotaryCounter\":\"";
					String messageData = messageStart + rotaryCounter;
					String fullMessage = messageData + "\"";

					if (header.indexOf("GET /?reverse=on") >= 0)
					{
						//Serial.println("Clock is reversing");
						timeCurrentlyControlledByUser = true;
						directionFastForward = false;
						directionReverse = true;
            pause = false;
            status = "Reverse";
					}
					else if (header.indexOf("GET /?fastForward=on") >= 0)
					{
						//Serial.println("Clock is fast forwarding");
						timeCurrentlyControlledByUser = true;
						directionReverse = false;
						directionFastForward = true;
            pause = false;
            status = "Fast Forward";
					}
					else if (header.indexOf("GET /?normal=on") >= 0)
					{
						//Serial.println("Clock is running normally");
						// Reset the variables that control clock function
						directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            status = "Normal";
            pause = false;
            initialized = true;
					}
          else if (header.indexOf("GET /?reset=on") >= 0)
					{
						resetClockFromESP();
					}
          else if (header.indexOf("GET /?reset=reset") >= 0)
					{
						resetClockFromESP();
					}
          else if (header.indexOf("GET /reset") >= 0)
					{
						resetClockFromESP();
					}
          else if (header.indexOf("GET /?pause=on") >= 0)
					{
						//Serial.println("PAUSE ON");
            pause = true;
            status = "Paused";
            fullMessage = fullMessage + ",\"mode\":\"\"";
					}
          else if (header.indexOf("GET /?pause=off") >= 0)
					{
						//Serial.println("PAUSE OFF");
            pause = false;
            directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            status = "Normal";
					}
          else if(header.indexOf("GET /?status=get" >= 0)){
            //We might need to add more metadata on to the response here someday, but for now we dont need to do anything extra here.
          }

          fullMessage = fullMessage + ",\"status\":\"" + status + "\"}";
          String contentLengthString = "Content-Length: " + fullMessage.length() + 2;

					rcvClient.println("HTTP/1.1 200 OK");
					rcvClient.println("Content-type: application/json");
					rcvClient.println(contentLengthString);
					rcvClient.println("Access-Control-Allow-Origin: *");
					rcvClient.println();

					// The HTTP response ends with another blank line
					rcvClient.println(fullMessage); // CONTENT LENGTH DYNAMIC BASED ON HOW LONG THIS IS (IN CHARACTERS) + 1
					// Break out of the while loop
					break;
				}
				else
				{ // if you got a newline, then clear currentLine
					currentLine = "";
				}
			}
			else if (c != '\r')
			{					  // if you got anything else but a carriage return character,
				currentLine += c; // add it to the end of the currentLine
			}
		}
	}
	// Clear the header variable
	header = "";
	// Close the connection
	rcvClient.stop();
	Serial.println("Client disconnected.");
	Serial.println("");
}

void resetClockFromESP(){
  //Serial.println("Clock is running normally");
  // Reset the variables that control clock function
  directionReverse = false;
  directionFastForward = false;
  timeCurrentlyControlledByUser = true;
  reset = true;
  pause = true;
  status = "Resetting";
  initialized = false;
}

void processInterruptorSwitches()
{
	// read the interruptor values into variables
	int interruptorVal = digitalRead(Interruptor);
  int interruptor2Val = digitalRead(Interruptor2);

	// Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
	// HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
	// button's pressed, and off when it's not:


  // Interruptor at midnight is on D3 (Interruptor(1))
  if (interruptorVal == LOW && midnight == false)
  {
    //Serial.println("We just got to mignight");
    timeCurrentlyControlledByUser = false;
    midnight = true;
  }
  else if(interruptor2Val == LOW && startingPosition == false){
    //Serial.println("We just got to the starting position");
    timeCurrentlyControlledByUser = false;
    startingPosition = true;
    reset = false;

    if(initialized == false){
      pause = true;
      status = "paused";
    }
    else{
      directionReverse = false;
      directionFastForward = false;
      timeCurrentlyControlledByUser = false;
      status = "Normal";
      pause = false;
      initialized = true;
      if(tvAtStart == false){
        sendMessageToESP("clockmode=start", tvIP);
        tvAtStart = true;
      }
    }
    rotaryCounter = 0;
  }
  else{
    startingPosition = false;
  }
	
}

void handleRotaryLogic() {
	// Read the current state of A
	currentStateA = digitalRead(A);

	// If last and current state of A are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateA != lastStateA){

    //Code that takes care of updating rotaryCounter and direction
		if (digitalRead(B) == currentStateA) {
			rotaryCounter ++;
			currentDir ="CCW";
		} else {
			// Encoder is rotating CW so increment
			rotaryCounter --;
			currentDir ="CW";
		}
    //Serial.print("Direction: " + currentDir + "Count: ");
    Serial.println(rotaryCounter);

    if(rotaryCounter == 3){
      tvAtStart = false;
    }

    //Send messages to various ESP's depending on whether they should be activated or not
    //DIAL TIME FRAME
    if(rotaryCounter > 10 && rotaryCounter < 30){
      if(dialEnabled == false){
        dialEnabled = true;
        sendMessageToESP("leds=on", dialIP);
      }
    }
    else{
      if(dialEnabled == true){
        sendMessageToESP("leds=off", dialIP);
        dialEnabled = false;
      }
    }

    //POT TIME FRAME
    if(rotaryCounter > 31 && rotaryCounter < 50){
      if(potsEnabled == false){
        potsEnabled = true;
        sendMessageToESP("enabled=true", potIP);
      }
    }
    else{
      if(potsEnabled == true){
        sendMessageToESP("enabled=false", potIP);
        potsEnabled = false;
      }
    }

    //NUMBER PUZZLE TIME FRAME
    if(rotaryCounter > 51 && rotaryCounter < 70){
      if(numberPuzzleEnabled == false){
        numberPuzzleEnabled = true;
        sendMessageToESP("enabled=true", numberPuzzleIP);
      }
    }
    else{
      if(numberPuzzleEnabled == true){
        sendMessageToESP("enabled=false", numberPuzzleIP);
        numberPuzzleEnabled = false;
      }
    }
	}

  // Remember last A state
	lastStateA = currentStateA;
  // else if(rotaryCounter == midnightCount){
  //   midnight = true;
  // }

	// Put in a slight delay to help debounce the reading (maybe remove)
  delay(2);
}

//This makes sending the time to every ESP that needs to be aware of it easier. This might need to be offloaded to a different ESP if it becomes too slow
void InformAllDevicesMidnight(){
  String midnightMessage  = midnight ? "midnight=true" : "midnight=false";

  for(int i = 0; i < numOfMidnightDependentDevices; i++){
    sendMessageToESP(midnightMessage, midnightDependentIPs[i]);
  }
}

/// @brief This simplifies sending a message to a server.
/// @param command What do you want to tell the server? Dont include the "/?" at the beginning, but if you're going to do a long query dont forget to add the extra "&" signs.
/// @param address IP Address of the ESP module you want to communicate with. "IP:PORT"
/// @return The string response that was retrieved from the server or an error message if an error occured.
String sendMessageToESP(String command, String address)
{
	String response = "";

	// Check WiFi connection status
	if (WiFi.status() == WL_CONNECTED)
	{
		WiFiClient sendClient;
		HTTPClient http;

		String serverPath = "http://" + address + "/?" + command;

		// Your Domain name with URL path or IP address with path
		http.begin(sendClient, serverPath.c_str());

    //Change timeout so its not so long (5 seconds for now, maybe change later)
    http.setTimeout(5000);
		// Send HTTP GET request
		int httpResponseCode = http.GET();

		if (httpResponseCode > 0)
		{
			Serial.print("HTTP Response code: ");
			Serial.println(httpResponseCode);
			response = http.getString();
			Serial.println(response);
		}
		else
		{
			Serial.print("Error code: ");
			Serial.println(httpResponseCode);
			response = "An error occured. Please try again. Error code: " + httpResponseCode;
		}
		// Free resources
		http.end();
	}
	else
	{
		Serial.println("WiFi Disconnected");
		response = "This ESP has been disconnected from WiFi.";
	}

	return response;
}

void connectToWifi()
{
	// Connect to Wi-Fi network with SSID and password
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	// Print local IP address and start web server
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
}
