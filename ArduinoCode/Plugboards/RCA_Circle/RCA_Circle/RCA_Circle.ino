// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

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

// ---- WIFI SECTION ----
const char *ssid = "Whitefire";//EscapeY2K
const char *password = "R00tb33R";//caNY0u3scAp3?!
WiFiServer server(1234);

String tvIP = "192.168.1.211:8001"; // 10.0.0.64 at Jakes house
String plugTwoIP = "10.0.0.174:1234";

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 200;

// ---- Puzzle Stuff ----
String status = "Unsolved";
bool solved1 = false;
bool solved2 = false;
bool solved3 = false;

//Plugboard pin declaration FOR PUZZLE 1
const int plug1Pin = D1;
const int plug2Pin = D2;
const int plug3Pin = D3;
const int plug4Pin = D4;

//LED pin declaration PUZZLE 1
const int plug1LED = D5;
const int plug2LED = D6;
const int plug3LED = D7;

// ---- PLUGBOARD STATES SECTION ----
int plug1 = 0;
int plug2 = 0;
int plug3 = 0;
int plug4 = 0;

void setup() {
  // start serial connection
	Serial.begin(9600);

	connectToWifi();
  // initialize the LED pin as an output:
  pinMode(plug1LED, OUTPUT);
  pinMode(plug2LED, OUTPUT);
  pinMode(plug3LED, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(plug1Pin, INPUT_PULLUP);
  pinMode(plug2Pin, INPUT_PULLUP);
  pinMode(plug3Pin, INPUT_PULLUP);
  pinMode(plug4Pin, INPUT_PULLUP);
}

// the loop function runs over and over again forever
void loop() {
  WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

  // read the interruptor values into variables
  if(solved1 == false){
    patternLogic();
  }
  else if(solved1 == true && solved2 == true){
    checkFourthPlug();
  }
  

  //This is just for ticking
  if ((millis() - previousTime) > TickDelayTime)
  {
    previousTime = millis();

    //Check for solved stuff
    if(solved3 == true){
      //TODO: Send a message to the other ESP so its puzzle can be solved
      allOn();
      delay(200);
      resetLEDS();
      delay(200);
    }
  }
}

// ---- HELPER METHODS ----

void patternLogic(){
  // FIRST PLUG CHECK
  plug1 = digitalRead(plug1Pin);
  if(plug1 == HIGH){
    digitalWrite(plug1LED, LOW);
  }
  else{
    digitalWrite(plug1LED, HIGH);
  }

  // SECOND PLUG CHECK
  plug2 = digitalRead(plug2Pin);
  if(plug2 == HIGH){
    digitalWrite(plug2LED, LOW);
  }
  else{
    digitalWrite(plug2LED, HIGH);
  }

  // THIRD PLUG CHECK
  plug3 = digitalRead(plug3Pin);
  if(plug3 == HIGH){
    digitalWrite(plug3LED, LOW);
  }
  else{
    digitalWrite(plug3LED, HIGH);
  }

  if(plug1 == LOW && plug2 == LOW && plug3 == LOW){
    if(solved1 == false){
      status = "Solved";
      sendMessageToESP("firstSolved=true", plugTwoIP);
      flashLEDSFirstPattern();
    }
    solved1 = true;
  }
}

void checkFourthPlug(){
  plug4 = digitalRead(plug4Pin);
  if(plug4 == LOW){
    if(solved3 == false){
      Serial.println("Send message to two that final is solved");
      sendMessageToESP("finalSolved=true", plugTwoIP);
    }
    solved3 = true;
  }
}

void flashLEDSFirstPattern(){
  int delayTime = 200;
  resetLEDS();
  passTheLight(delayTime);
  delay(delayTime);
  allOn();
  delay(delayTime);
  resetLEDS();
  delay(delayTime);
  allOn();
  delay(delayTime);
}

void flashLEDSSecondPattern(){
  flashLEDSFirstPattern();
}

// ---- LED PATTERN HELPERS ----
void resetLEDS(){
  if(!solved1){
    digitalWrite(plug1LED, LOW);
    digitalWrite(plug2LED, LOW);
    digitalWrite(plug3LED, LOW);
  }
}

void allOn(){
  digitalWrite(plug1LED, HIGH);
  digitalWrite(plug2LED, HIGH);
  digitalWrite(plug3LED, HIGH);
}

void loadToFull(int delayTime){
  digitalWrite(plug1LED, HIGH);
  delay(delayTime);
  digitalWrite(plug2LED, HIGH);
  delay(delayTime);
  digitalWrite(plug3LED, HIGH);
  delay(delayTime);
}

void passTheLight(int delayTime){
  digitalWrite(plug1LED, HIGH);
  delay(delayTime);
  digitalWrite(plug1LED, LOW);
  digitalWrite(plug2LED, HIGH);
  delay(delayTime);
  digitalWrite(plug2LED, LOW);
  digitalWrite(plug3LED, HIGH);
  delay(delayTime);
  digitalWrite(plug3LED, LOW);
}

// ---- WIFI HELPERS ----
void handleClientConnected(WiFiClient rcvClient)
{
  //ONLY FOR PATTERN SO ITS AT THE SAME TIME
  bool secondSolved = false;

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
					if (header.indexOf("GET /?secondSolved=true") >= 0)
					{
            flashLEDSSecondPattern();
            Serial.println("Got second solved message!");
            solved2 = true;
					}
          else if (header.indexOf("GET /?secondSolved=false") >= 0)
					{
            solved2 = false;
					}
          else if (header.indexOf("GET /?reset=reset") >= 0)
					{
            solved1 = false;
            solved2 = false;
            solved3 = false;
					}
          if (header.indexOf("GET /?finalSolved=true") >= 0)
					{
            solved3 = true;
					}

          String fullMessage = "{\"status\":\"" + status + "\",\"solved1\":\"" + solved1 + "\",\"solved2\":\"" + solved2 + "\",\"solved3\":\"" + solved3 + "\"}";
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

  if(secondSolved == true){
    flashLEDSFirstPattern();
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