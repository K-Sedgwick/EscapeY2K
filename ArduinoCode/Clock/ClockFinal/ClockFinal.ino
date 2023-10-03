// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Includes the Arduino Stepper Library
#include <Stepper.h>

// ---- WIFI SECTION ----
const char *ssid = "EscapeY2K";//EscapeY2K
const char *password = "caNY0u3scAp3?!";//caNY0u3scAp3?!
WiFiServer server(1234);

//IP addresses of the devices that need to know about what time it is
const int numOfTimeDependentDevices = 1;
String timeDependentIPs[numOfTimeDependentDevices] = {"192.168.1.202:1234"};

// ---- STEPPER SECTION ----
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
const int tickSteps = 100;
const int stepperSpeed = 10;
const int reverseSteps = -10;
const int fastForwardSteps = 10;
unsigned long ticksCompleted = 0;
Stepper clockStepper = Stepper(stepsPerRevolution, 3, 4, 5, 2); // Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

// ---- SLOT INTERRUPTOR SECTION ----
const int InterruptorAtOne = 2;
const int InterruptorAtTwelve = 0;
bool timeCurrentlyControlledByUser = false;
bool directionReverse = false;
bool directionFastForward = false;

// ---- GENERAL SECTION ----
const int LedPin = 13;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 500;
bool midnight = false;
bool pause = false;
bool manualOverride = false; //If the room admin has to do stuff allow them extra control while in this mode

// ---- SETUP AND LOOP ----

void setup()
{
	// start serial connection
	Serial.begin(115200);

	connectToWifi();

	// configure pin 2 and 3 as an input and enable the internal pull-up resistor
	pinMode(InterruptorAtOne, INPUT_PULLUP);
	pinMode(InterruptorAtTwelve, INPUT_PULLUP);

  //Setup pins for stepper
	pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  //Setup pin for LED so we can test stuff
  // pinMode(LedPin, OUTPUT); 

	// Rotate CW quickly at 10 RPM
	// TODO: Figure out how to change direction
	clockStepper.setSpeed(stepperSpeed);
}

void loop()
{
	WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

  processInterruptorSwitches();

  //If the admin hasnt requested control just operate normally
  if(!manualOverride && !pause){
    //If it's midnight send messages to all of the devices that need to know and dont allow the user to do any more adjusting of the time
    if(midnight){
      //InformAllDevicesOfTime();
    }
    // If the user hasnt selected a different time just tick normally
    else if (!timeCurrentlyControlledByUser && (millis() - previousTime) > TickDelayTime)
    {
      //Serial.print("Tick");
      clockStepper.step(tickSteps);
      ticksCompleted += tickSteps;
      previousTime = millis();
    }
    // If they selected reverse, tell the stepper to reverse until they ask to be done
    else if (directionReverse)
    {
      clockStepper.step(reverseSteps); // TODO: Figure out what direction is positive and what is negative
      ticksCompleted += reverseSteps;
    }
    else if (directionFastForward)
    {
      clockStepper.step(10);
      ticksCompleted += fastForwardSteps;
    }
  }
  else{
    //TODO: Decide if we want to add anything special here? Probably not, im assuming we just dont want the clock running rn :P
  }
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
					String messageStart = "{\"ticksCompleted\":\"";
					String messageData = messageStart + ticksCompleted;
					String fullMessage = messageData + "\"";

					if (header.indexOf("GET /?reverse=on") >= 0)
					{
						Serial.println("Clock is reversing");
						timeCurrentlyControlledByUser = true;
						directionFastForward = false;
						directionReverse = true;
            fullMessage = fullMessage + ",\"mode\":\"Reverse\"";
					}
					else if (header.indexOf("GET /?fastForward=on") >= 0)
					{
						Serial.println("Clock is fast forwarding");
						timeCurrentlyControlledByUser = true;
						directionReverse = false;
						directionFastForward = true;
            fullMessage = fullMessage + ",\"mode\":\"Fast Forward\"";
					}
					else if (header.indexOf("GET /?normal=on") >= 0)
					{
						Serial.println("Clock is running normally");
						// Reset the variables that control clock function
						directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            fullMessage = fullMessage + ",\"mode\":\"Normal\"";
					}
          else if (header.indexOf("GET /?reset=on") >= 0)
					{
						Serial.println("Clock is running normally");
						// Reset the variables that control clock function
						directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            ticksCompleted = 0;
            fullMessage = fullMessage + ",\"mode\":\"Normal\"";
					}
          else if (header.indexOf("GET /?manualOverride=on") >= 0)
					{
						Serial.println("ADMIN MODE ON");
            //First, let the system know the admin is in charge of the room currently
            manualOverride = true;
						// Reset the variables that control clock function
						directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            fullMessage = fullMessage + ",\"mode\":\"ADMIN\"";
					}
          else if (header.indexOf("GET /?manualOverride=off") >= 0)
					{
						Serial.println("ADMIN MODE OFF");
            //First, let the system know the admin is no longer in charge of the room currently
            manualOverride = false;
						// Reset the variables that control clock function
						directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
					}
          else if (header.indexOf("GET /?pause=on") >= 0)
					{
						Serial.println("PAUSE ON");
            pause = true;
            manualOverride = true;
            fullMessage = fullMessage + ",\"mode\":\"Paused\"";
					}
          else if (header.indexOf("GET /?pause=off") >= 0)
					{
						Serial.println("PAUSE OFF");
            pause = false;
            manualOverride = false;
            directionReverse = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
            fullMessage = fullMessage + ",\"mode\":\"Normal\"";
					}

          //This allows us to add any other properties we may want to add and then still close the response when were done
          fullMessage = fullMessage + "}";
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

void processInterruptorSwitches()
{
	// read the interruptor values into variables
	//int InterruptorValAtOne = digitalRead(InterruptorAtOne);
	int InterruptorValAtTwelve = digitalRead(InterruptorAtTwelve);

	// Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
	// HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
	// button's pressed, and off when it's not:
	if (HIGH == LOW) //switch to InterruptorValAtOne when were ready to turn on
	{
		// print out the value of the pushbutton
		Serial.println("It's 1 o'clock!");
		timeCurrentlyControlledByUser = false;
		ticksCompleted = 0;
	}
	else if (!midnight && InterruptorValAtTwelve == LOW)
	{
		// print out the value of the pushbutton
		Serial.println("It's 12 o'clock!");
		timeCurrentlyControlledByUser = false;
		midnight = true;
	}
}

//This makes sending the time to every ESP that needs to be aware of it easier. This might need to be offloaded to a different ESP if it becomes too slow
void InformAllDevicesOfTime(){
  String ticksCompletedString = String(ticksCompleted);
  String currentTime  = midnight ? "midnight=on" : "ticksCompleted=" + ticksCompletedString;

  for(int i = 0; i < numOfTimeDependentDevices; i++){
    sendMessageToESP(currentTime, timeDependentIPs[i]);
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
