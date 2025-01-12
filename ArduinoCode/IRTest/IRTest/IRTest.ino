#include <IRremote.h>

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ---- WIFI SECTION ----
const char *ssid = "Whitefire";	   // EscapeY2K
const char *password = "R00tb33R"; // caNY0u3scAp3?!
WiFiServer server(1234);
String tvIp = "192.168.1.211:8001";
String clockIp = "192.168.1.50:1234";

// ESP pins
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define TXPIN 1
#define RXPIN 3

// IR Stuff
#define IR_RECEIVE_PIN D1
String previousCmd = "Unknown";

bool initialized = false;

void setup()
{
	Serial.begin(9600);

	connectToWifi();

	IrReceiver.begin(IR_RECEIVE_PIN);

  // pinMode(LED_BUILTIN, OUTPUT);

	// digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
	// delay(500);
	// digitalWrite(LED_BUILTIN, HIGH);
}
void loop()
{
	WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

	handleIRLogic();

	// Add in other functionality here if you so desire
	// This will run once every TickTimeDelays
	//  if ((millis() - previousTime) > TickDelayTime)
	//  {
	//    //DONT REMOVE THIS LINE! Its super important
	//    previousTime = millis();

	// }
}

// ---- HELPER METHODS ----
void handleIRLogic()
{
	if (IrReceiver.decode())
	{
		IrReceiver.resume();
		int cmd = IrReceiver.decodedIRData.command;
    Serial.println(cmd);
    
		if (cmd == 21 && previousCmd != "Play")
		{
			previousCmd = "Play";
			Serial.println(previousCmd);
      //sendMessageToESP("clockmode=tick", tvIp);
		}
		else if (cmd == 25 && previousCmd != "Reverse")
		{
			previousCmd = "Reverse";
			Serial.println(previousCmd);
      //sendMessageToESP("clockmode=reverse", tvIp);
		}
		else if (cmd == 19 && previousCmd != "Fast forward")
		{
			previousCmd = "Fast forward";
			Serial.println(previousCmd);
      //sendMessageToESP("clockmode=fastForward", tvIp);
		}
    else if (cmd == 18 && initialized == false){
      initialized = true;
      Serial.println("Power");
      sendMessageToESP("initialize=true", tvIp)
    }
	}
}

void handleClientConnected(WiFiClient rcvClient)
{
	// SETUP VARIABLES
	String header = "";
	String currentLine = ""; // make a String to hold incoming data from the client
	int handleClientTimeout = 2000;
	unsigned long clientConnectedTime = millis();
	int previousClientConnectedTime = clientConnectedTime;

	// Serial.println("New Client."); // print a message out in the serial port
	while (rcvClient.connected() && clientConnectedTime - previousClientConnectedTime <= handleClientTimeout)
	{ // loop while the client's connected
		clientConnectedTime = millis();
		if (rcvClient.available())
		{							   // if there's bytes to read from the client,
			char c = rcvClient.read(); // read a byte, then
			// Serial.write(c);		   // print it out the serial monitor
			header += c;
			if (c == '\n')
			{ // if the byte is a newline character
				// if the current line is blank, you got two newline characters in a row.
				// that's the end of the client HTTP request, so send a response:
				if (currentLine.length() == 0)
				{
					String fullMessage = "{\"message\":\"received\"";

					if (header.indexOf("GET /?previous=get") >= 0)
					{
						fullMessage = fullMessage + ",\"previous\":\"" + previousCmd + "\"";
					}
          else if (header.indexOf("GET /?reset=reset") >= 0)
					{
            initialized = false;
						fullMessage = fullMessage + ",\"resetting\":\"true\"";
					}
          else if (header.indexOf("GET /reset") >= 0)
					{
						initialized = false;
						fullMessage = fullMessage + ",\"resetting\":\"true\"";
					}


					// This allows us to add any other properties we may want to add and then still close the response when were done
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

		// Change timeout so its not so long (5 seconds for now, maybe change later)
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
