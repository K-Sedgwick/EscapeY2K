// Load Required libraries
#include <string.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define D1 5
#define D2 4
#define D3 0
#define D4 2

// ---- WIFI SECTION ----
const char *ssid = "Whitefire";//EscapeY2K
const char *password = "R00tb33R";//caNY0u3scAp3?!
WiFiServer server(1234);

// ---- GENERAL SECTION ----
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long TickDelayTime = 1000;
bool ledStatus = false;

// ---- MP3 SECTION ----
#define MP3_RX D4
#define MP3_TX D3

// ---- Sound Effect Indexes ----
// 001 - spacejvke
// 002 - knock
// 003 - footsteps
// 004 - growl
// 005 - humming
// 006 - monster footsteps
// 007 - scream
// 008 - whispering
// 009 - rick
// 011 - 
// 012 - 
// 013 - 

// Define the required MP3 Player Commands:
// Select storage device to TF card
static int8_t select_SD_card[] = {0x7e, 0x03, 0X35, 0x01, 0xef}; // 7E 03 35 01 EF
// Play with index: /01/00{SONG_NUM_HERE}xxx.mp3 (in this case 3)
static int8_t play_default_song[] = {0x7e, 0x04, 0x41, 0x00, 0x03, 0xef}; // 7E 04 41 00 03 EF
// Play the song.
static int8_t play[] = {0x7e, 0x02, 0x01, 0xef};
// Pause the song.
static int8_t pause[] = {0x7e, 0x02, 0x02, 0xef};
// + Volume
static int8_t volUp[] = {0x7e, 0x02, 0x05, 0xef};
// - Volume
static int8_t volDown[] = {0x7e, 0x02, 0x06, 0xef};
// Set Volume
static int8_t volMin[] = {0x7e, 0x03, 0x31, 0x01, 0xef};

// Define the Serial MP3 Player Module.
SoftwareSerial MP3(MP3_RX, MP3_TX);

// ---- SETUP AND LOOP ----

void setup()
{
	// start serial connection
	connectToWifi();

	//Setup serial communication
	Serial.begin(115200);

  // Initiate the Serial MP3 Player Module.
  MP3.begin(9600);

	// Select the SD Card.
  send_command_to_MP3_player(select_SD_card, 6);
	//And turn the volume down because I think it starts at max volume
	send_command_to_MP3_player(volMin, 5);
  send_command_to_MP3_player(play_default_song, 6);

	//Setup pin for LED so we can test stuff
	pinMode(LED_BUILTIN, OUTPUT); 
	digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
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
    previousTime = millis();
    if(ledStatus == true){
      ledStatus = false;
      digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    }
    else{
      ledStatus = true;
      digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
    }
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
					if (header.indexOf("GET /?play=") >= 0)
					{
						//TODO: Break up the string, parse the val sent as an int, then send that value over serial

            String intToParse = "";
            int parsedInt = -1;
            //We start at 11 because the "GET /?play=" is 11 characters
            int index = 11;
            //To get the whole number we have to loop dynamically
            while(true){
              //Get the value that could be an int
              String asciiSongNumVal = String(header[header.indexOf("GET /?play=")+index]);
              //Try to parse it
              parsedInt = lowByte(asciiSongNumVal.toInt());
              //If its a 0 (which means it couldn't parse) then break out of the loop
              if(parsedInt == 0){
                break;
              }
              index++;
              intToParse.concat(parsedInt);
            }
            int8_t selectedSong = lowByte(intToParse.toInt());
						int8_t play_selected_song[] = {0x7e, 0x04, 0x42, 0x01, selectedSong, 0xef}; // 7E 04 41 04 01 EF
						send_command_to_MP3_player(play_selected_song, 6);
            fullMessage = fullMessage + ",\"playing\":\"" + selectedSong + "\",\"status\":\"Playing\"";
					}
          //This play is different, it just plays from where we paused instead of restarting the track
          else if (header.indexOf("GET /?song=play") >= 0)
					{
  						send_command_to_MP3_player(play, 4);
              fullMessage = fullMessage + ",\"status\":\"Playing\"";
					}
					else if (header.indexOf("GET /?song=pause") >= 0)
					{
  						send_command_to_MP3_player(pause, 4);
              fullMessage = fullMessage + ",\"status\":\"Paused\"";
					}
					else if (header.indexOf("GET /?vol=up") >= 0)
					{
						send_command_to_MP3_player(volUp, 4);
            fullMessage = fullMessage + ",\"volume\":\"Raised\"";
					}
					else if (header.indexOf("GET /?vol=down") >= 0)
					{
						send_command_to_MP3_player(volDown, 4);
            fullMessage = fullMessage + ",\"volume\":\"Lowered\"";
					}
          else if (header.indexOf("GET /?vol=min") >= 0)
					{
						send_command_to_MP3_player(volMin, 5);
            fullMessage = fullMessage + ",\"volume\":\"Min\"";
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
	//Serial.println("Client disconnected.");
	//Serial.println("");
}

void send_command_to_MP3_player(int8_t command[], int len){
  Serial.print("\nMP3 Command => ");
  for(int i=0;i<len;i++){
    MP3.write(command[i]);
    Serial.print(command[i], HEX);
    delay(1);
  }
  delay(1000);
}

void connectToWifi()
{
	// Connect to Wi-Fi network with SSID and password
	//Serial.print("Connecting to ");
	//Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		//Serial.print(".");
	}
	// Print local IP address and start web server
	//Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
}
