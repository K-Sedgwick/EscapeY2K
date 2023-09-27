// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Includes the Arduino Stepper Library
#include <Stepper.h>

// ---- WIFI SECTION ----
const char *ssid = "EscapeY2K";
const char *password = "caNY0u3scAp3?!";
WiFiServer server(1234);

// ---- STEPPER SECTION ----
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
const int tickSteps = 100;
const int stepperSpeed = 10;
const int reverseSteps = -10;
const int fastForwardSteps = 10;
unsigned long ticksCompleted = 0;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper clockStepper = Stepper(stepsPerRevolution, 11, 9, 10, 8);

// ---- SLOT INTERRUPTOR SECTION ----
const int InterruptorAtOne = 2;
const int InterruptorAtTwelve = 3;
bool timeCurrentlyControlledByUser = false;
bool directionReverse = false;
bool directionFastForward = false;

// ---- GENERAL SECTION ----
const int LedPin = 13;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 500;
bool midnight = false;

// ---- SETUP AND LOOP ----

void setup()
{
	// start serial connection
	Serial.begin(115200);

	connectToWifi();

	// configure pin 2 and 3 as an input and enable the internal pull-up resistor
	pinMode(InterruptorAtOne, INPUT_PULLUP);
	pinMode(InterruptorAtTwelve, INPUT_PULLUP);
	pinMode(LedPin, OUTPUT);

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

	// If the user hasnt selected a different time just tick normally
	if (!timeCurrentlyControlledByUser && (millis() - previousTime) > TickDelayTime)
	{
		Serial.print("Tick");
		clockStepper.step(tickSteps);
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
			Serial.write(c);		   // print it out the serial monitor
			header += c;
			if (c == '\n')
			{ // if the byte is a newline character
				// if the current line is blank, you got two newline characters in a row.
				// that's the end of the client HTTP request, so send a response:
				if (currentLine.length() == 0)
				{
					String messageStart = "{'ticksCompleted':'";
					String messageData = messageStart + ticksCompleted;
					String fullMessage = messageData + "'}";

					String contentLengthString = "Content-Length: " + fullMessage.length() + 2;

					rcvClient.println("HTTP/1.1 200 OK");
					rcvClient.println("Content-type: application/json");
					rcvClient.println(contentLengthString);
					rcvClient.println("Access-Control-Allow-Origin: *");
					rcvClient.println();

					// PUT YOUR LOGIC HERE!
					// Change the strings after GET to what youre hoping the URL will have in it
					if (header.indexOf("GET /?reverse=on") >= 0)
					{
						Serial.println("Clock is reversing");
						timeCurrentlyControlledByUser = true;
						directionFastForward = false;
						directionReverse = true;
						digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
					}
					else if (header.indexOf("GET /?fastForward=on") >= 0)
					{
						Serial.println("Clock is reversing");
						timeCurrentlyControlledByUser = true;
						directionReverse = false;
						directionFastForward = true;
						digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
					}
					else if (header.indexOf("GET /?normal=on") >= 0)
					{
						Serial.println("Clock is reversing");
						// Reset the variables that control clock function
						directionFastForward = false;
						directionFastForward = false;
						timeCurrentlyControlledByUser = false;
						digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
					}

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
	int InterruptorValAtOne = digitalRead(InterruptorAtOne);
	int InterruptorValAtTwelve = digitalRead(InterruptorAtTwelve);

	// Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
	// HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
	// button's pressed, and off when it's not:
	if (InterruptorValAtOne == LOW)
	{
		// print out the value of the pushbutton
		Serial.println("It's 1 o'clock!");
		timeCurrentlyControlledByUser = false;
		ticksCompleted = 0;
		digitalWrite(LedPin, LOW);
	}
	else if (InterruptorValAtTwelve == LOW)
	{
		// print out the value of the pushbutton
		Serial.println("It's 12 o'clock!");
		timeCurrentlyControlledByUser = false;
		midnight = 1;
		digitalWrite(LedPin, LOW);
	}
	else
	{
		digitalWrite(LedPin, HIGH);
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
