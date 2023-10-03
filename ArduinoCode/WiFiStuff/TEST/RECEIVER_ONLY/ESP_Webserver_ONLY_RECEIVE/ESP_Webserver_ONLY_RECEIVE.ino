// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ---- WIFI SECTION ----
const char *ssid = "EscapeY2K";//EscapeY2K
const char *password = "caNY0u3scAp3?!";//caNY0u3scAp3?!
WiFiServer server(1234);

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 500;
bool midnight = false;
int buttonPin = 0;

// ---- SETUP AND LOOP ----

void setup()
{
	// start serial connection
	Serial.begin(115200);

	connectToWifi();

  //Setup pin for LED so we can test stuff
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(buttonPin, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
}

void loop()
{
	WiFiClient rcvClient = server.available(); // Listen for incoming clients

	if (rcvClient)
	{
		handleClientConnected(rcvClient);
	}

  //Add in other functionality here if you so desire
  //This will run once every TickTimeDelays
  if ((millis() - previousTime) > TickDelayTime)
  {
    
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
					String fullMessage = "{";

					if (header.indexOf("GET /?midnight=on") >= 0)
					{
						Serial.println("MIDNIGHT: OPEN BOX");
            midnight = true;
            digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
            digitalWrite(buttonPin, HIGH);
            delay(200);
            digitalWrite(buttonPin, LOW);
            fullMessage = fullMessage + "\"box\":\"open\"";
					}
					else if (header.indexOf("GET /?midnight=off") >= 0)
					{
						Serial.println("Clock is fast forwarding");
            midnight = false;
           digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
           digitalWrite(buttonPin, HIGH);
            delay(200);
            digitalWrite(buttonPin, LOW);
            fullMessage = fullMessage + "\"box\":\"closed\"";
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
