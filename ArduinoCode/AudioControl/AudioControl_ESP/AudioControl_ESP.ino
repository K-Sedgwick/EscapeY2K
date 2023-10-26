// Load Required libraries
#include <string.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// ---- WIFI SECTION ----
const char *ssid = "EscapeY2K";//EscapeY2K
const char *password = "caNY0u3scAp3?!";//caNY0u3scAp3?!
WiFiServer server(1234);

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 500;

// ---- MP3 SECTION ----
// Define the RX and TX pins to establish UART communication with the MP3 Player Module.
#define MP3_RX 3 // to TX
#define MP3_TX 1 // to RX

// Define the required MP3 Player Commands:

// Select storage device to TF card
static int8_t select_SD_card[] = {0x7e, 0x03, 0X35, 0x01, 0xef}; // 7E 03 35 01 EF
// Play with index: /01/00{SONG_NUM_HERE}xxx.mp3
static int8_t play_default_song[] = {0x7e, 0x04, 0x41, 0x00, 0x01, 0xef}; // 7E 04 41 00 01 EF
// Play the song.
static int8_t play[] = {0x7e, 0x02, 0x01, 0xef}; // 7E 02 01 EF
// Pause the song.
static int8_t pause[] = {0x7e, 0x02, 0x02, 0xef}; // 7E 02 02 EF
// + Volume
static int8_t volUp[] = {0x7e, 0x02, 0x05, 0xef}; // 7E 02 02 EF
// - Volume
static int8_t volDown[] = {0x7e, 0x02, 0x06, 0xef}; // 7E 02 02 EF

// Define the Serial MP3 Player Module.
SoftwareSerial MP3(MP3_RX, MP3_TX);

// ---- SETUP AND LOOP ----

void setup()
{
	// start serial connection
	connectToWifi();

	//Setup serial communication
	Serial.begin(9600);

	// Initiate the Serial MP3 Player Module.
  	MP3.begin(9600);
	// Select the SD Card.
  send_command_to_MP3_player(select_SD_card, 5);
	//And turn the volume down because I think it starts at max volume
	send_command_to_MP3_player(volDown, 4);
	send_command_to_MP3_player(volDown, 4);
	send_command_to_MP3_player(volDown, 4);
	send_command_to_MP3_player(volDown, 4);
	send_command_to_MP3_player(volDown, 4);

	//Setup pin for LED so we can test stuff
	pinMode(LED_BUILTIN, OUTPUT); 
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

					if (header.indexOf("GET /?play=#") >= 0)
					{
						//TODO: Break up the string, parse the val sent as an int, then send that value over serial
						int songNum = header[header.indexOf("GET /?song=play")+1];
						static int8_t play_selected_song[6];
						memcpy(play_selected_song, play_default_song, sizeof(play_default_song));
						play_selected_song[4] = songNum;
						send_command_to_MP3_player(play_selected_song, 6);
					}
          //This play is different, it just plays from where we paused instead of restarting the track
          else if (header.indexOf("GET /?song=play") >= 0)
					{
  						send_command_to_MP3_player(play, 4);
					}
					else if (header.indexOf("GET /?song=pause") >= 0)
					{
  						send_command_to_MP3_player(pause, 4);
					}
					else if (header.indexOf("GET /?vol=up") >= 0)
					{
						send_command_to_MP3_player(volUp, 4);
					}
					else if (header.indexOf("GET /?vol=down") >= 0)
					{
						send_command_to_MP3_player(volDown, 4);
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

void send_command_to_MP3_player(int8_t command[], int len){
  Serial.print("\nMP3 Command => ");
  for(int i=0;i<len;i++){ MP3.write(command[i]); Serial.print(command[i], HEX); }
  delay(1000);
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
