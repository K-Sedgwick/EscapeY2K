// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ---- WIFI SECTION ----
const char *ssid = "Whitefire";//EscapeY2K
const char *password = "R00tb33R";//caNY0u3scAp3?!
WiFiServer server(1234);

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
#define A D1
#define B D2
#define BTN D6

int counter = 0;
bool ledEnabled = false;
bool solved = false;
int currentStateA;
int lastStateA;
String currentDir ="";

//Button stuff
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
  // Setup Serial Monitor
	Serial.begin(9600);

  connectToWifi();
        
	// Set encoder pins as inputs
	pinMode(A,INPUT);
	pinMode(B,INPUT);
  pinMode(D5, INPUT_PULLUP);

  pinMode(D0, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  digitalWrite(D0, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

	// Read the initial state of A
	lastStateA = digitalRead(A);

  //Set the lastButtonPress so the button doesnt think it was pressed right off the bat
	lastButtonPress = millis();
}

void loop()
{
	WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

  //We have to handle the dial puzzle logic as fast as possible, so dont put it in the delay stuff
  handleDialPuzzleLogic();

  //Add in other functionality here if you so desire
  //This will run once every TickTimeDelays
  // if ((millis() - previousTime) > TickDelayTime)
  // {
  //   //DONT REMOVE THIS LINE! Its super important
  //   previousTime = millis();
    
  // }
}

void handleDialPuzzleLogic() {
        
	// Read the current state of A
	currentStateA = digitalRead(A);

	// If last and current state of A are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateA != lastStateA  && currentStateA == 1){

    //Code that takes care of updating counter and direction
		if (digitalRead(B) != currentStateA) {
			counter ++;

      //If the dial gets spun a lot one direction or the other it will never actually count as "solving".
      //So, in order to reset that we have to keep counter bounded between -100 and 100
      if(counter >= 100){
        counter = 0;
      }
      if(counter <= -100){
        counter = 0;
      }

			currentDir ="CCW";

      //Checks to see if the players are rotating the correct direction and hit the correct value
      if(checkpoints[0] == 0 && counter == firstVal){
        Serial.println("First checkpoint passed");
        if(ledEnabled){
          digitalWrite(D4, HIGH);
        }
        checkpoints[0] = 1;
      }
      //Check to see if we should reset the checkpoints
      else if(checkpoints[1] == 0 && counter > firstVal){
        Serial.println("Checkpoints reset");
        checkpoints[0] = 0;
        checkpoints[1] = 0;
        resetLEDS();
      }
      else if(checkpoints[1] == 1 && counter == thirdVal){
        Serial.println("You did it!");
        solved = true;
        if(ledEnabled){
          digitalWrite(D0, HIGH);
          digitalWrite(D4, HIGH);
          digitalWrite(D3, HIGH);
        }
        //Let a box know that the puzzle has been solved
        sendMessageToESP("play=3", "192.168.1.127");
      }
		} else {
			// Encoder is rotating CW so increment
			counter --;
			currentDir ="CW";

      if(checkpoints[0] == 1 && counter == secondVal){
        checkpoints[1] = 1;
        Serial.println("Second checkpoint passed");
        if(ledEnabled){
          digitalWrite(D4, HIGH);
          digitalWrite(D3, HIGH);
        }
      }
      //If we already set the second checkpoint and we're going CCW then reset the checkpoints
      else if(checkpoints[1] == 1){
        Serial.println("Checkpoints reset");
        checkpoints[0] = 0;
        checkpoints[1] = 0;
        resetLEDS();
      }
		}
	}

	// Remember last A state
	lastStateA = currentStateA;

	// Read the button state
	int btnState = digitalRead(BTN);

  if(millis() - lastButtonPress > 500 && millis() - lastButtonPress < 502){
    flashLEDS();
  }

	//If we detect LOW signal, button is pressed
	if (btnState == LOW) {
		//if 500ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 500) {
      counter = 0;
		}

		// Remember last button press event
		lastButtonPress = millis();
	}

	// Put in a slight delay to help debounce the reading
  delay(1);
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

	//Serial.println("New Client."); // print a message out in the serial port
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
					String fullMessage = "{\"message\":\"received\"";

				  if (header.indexOf("GET /?leds=on") >= 0)
					{
            ledEnabled = true;
            fullMessage = fullMessage + ",\"leds\":\"Enabled\"";
					}
					else if (header.indexOf("GET /?leds=off") >= 0)
					{
            ledEnabled = false;
            resetLEDS();
            fullMessage = fullMessage + ",\"leds\":\"Disabled\"";
					}
					else if (header.indexOf("GET /?leds=flash") >= 0)
					{
            //Not sure what this is going to do just yet
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
			Serial.println("HTTP Response code: ");
			Serial.println(httpResponseCode);
			response = http.getString();
			Serial.println(response);
		}
		else
		{
			Serial.println("Error code: ");
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
	Serial.println("Connecting to ");
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

void flashLEDS(){
  if(!solved){
    digitalWrite(D0, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH);
    delay(70);
    resetLEDS();
    delay(70);
    digitalWrite(D0, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH);
    delay(70);
    resetLEDS();
  }
}

void resetLEDS(){
  if(!solved){
    digitalWrite(D0, LOW);
    digitalWrite(D3, LOW);
    digitalWrite(D4, LOW);
  }
}