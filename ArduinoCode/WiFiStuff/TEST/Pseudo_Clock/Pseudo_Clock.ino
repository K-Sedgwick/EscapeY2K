/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char *ssid = "EscapeY2K";
const char *password = "caNY0u3scAp3?!";

// THIS IS THE PORT FOR THIS ESP! When you send it requests you have to include the port!
WiFiServer server(1234);

//============= VARIABLES =============

//These are for the code so the clock stuff only fires once every second
unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0; // Previous time
const long timeoutTime = 1000; // Define timeout time in milliseconds (example: 1000ms = 1s)

//VARIABLES NEEDED FOR CLOCK TO FUNCTION
const int MAXCLOCKVALUE = 3600; //Number of seconds in an hour
const int JUMPSIZE = 2; //Tells how many time units we are going to advance per timeoutTime.
int currentGameClockTime = 0; //Current time displayed on the player clock (in seconds)
/*
    There are going to be various "modes" that the clock can be in...
        0: Stationary. Nothing is moving, mostly just for testing (and also for starting)
        1: Normal operation. This is when the clock is ticking once every "timeoutTime"
        2: Reverse: While in reverse mode we are going to be moving the servo as fast as possible BACKWARDS
        3: Fast forward: While in reverse mode we are going to be moving the servo as fast as possible FORWARDS
*/
int clockMode = 0;

//============= END VARIABLES =============

//============= HELPER FUNCTIONS =============

//This is where we are going to turn on the stepper motor until we hit the slot interruptor.`
//For now it just sets the currentGameClockTime value to 0.
void resetClock(){
    currentGameClockTime = 0;
}

string processClientRequest(){
    // PLACEHOLDER: turns the GPIOs on and off
    string response;

    if (header.indexOf("GET /?mode=0") >= 0)
    {
        mode = 0;
        //Turn motor off to reset from previous command

        Serial.println("Mode set to 0");
        response = "{'mode':'0'}";
    }
    else if (header.indexOf("GET /?mode=1") >= 0)
    {
        mode = 1;
        //Turn motor off to reset from previous command

        Serial.println("Mode set to 1");
        response = "{'mode':'1'}";
    }
    else if (header.indexOf("GET /?mode=2") >= 0)
    {
        mode = 2;
        //Turn motor off to reset from previous command

        Serial.println("Mode set to 2");
        response = "{'mode':'2'}";
    }
    else if (header.indexOf("GET /?mode=3") >= 0)
    {
        mode = 3;
        //Turn motor off to reset from previous command

        Serial.println("Mode set to 3");
        response = "{'mode':'3'}";
    }
    else{
        response = "{'command':'unknown'}"
    }

    return response;
}

//============= END HELPER FUNCTIONS =============

void setup()
{
    Serial.begin(115200);
    // Initialize the built-in LED
    pinMode(LED_BUILTIN, OUTPUT); 
    //Turn the LED off, theres no reason for it to be on rn.
    digitalWrite(LED_BUILTIN, HIGH);

    //Reset the clock
    resetClock();

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

void loop()
{
    WiFiClient rcvClient = server.available(); // Listen for incoming clients

    if (rcvClient)
    {                                  // If a new client connects,
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        currentTime = millis();
        previousTime = currentTime;
        while (rcvClient.connected() && currentTime - previousTime <= timeoutTime)
        { // loop while the client's connected
            currentTime = millis();
            if (rcvClient.available())
            {                              // if there's bytes to read from the client,
                char c = rcvClient.read(); // read a byte, then
                Serial.write(c);           // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        string response = processClientRequest();
                        // The HTTP response ends with another blank line
                        unsigned int responseLength = response.length + 2; //Add two here because we technically need to include the quotation marks

                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        rcvClient.println("HTTP/1.1 200 OK");
                        rcvClient.println("Content-type: application/json");
                        rcvClient.println("Content-Length: " + responseLength); //THIS NEEDS TO BE DYNAMIC
                        rcvClient.println("Access-Control-Allow-Origin: *");
                        rcvClient.println();

                        
                        rcvClient.println(response);
                        // Break out of the while loop
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
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
    else
    {
        //First check for the mode we are currently in
        switch(mode){
            //Stationary
            case 0:
                //Stop motor movement.
            break;
            //Normal Operation
            case 1:
                // Once per second I want the "clock" to advance
                if ((millis() - lastTime) > timerDelay)
                {
                    //First, check clock timer to make sure its within the interval range.
                    if(0 <= currentGameClockTime < MAXCLOCKVALUE){
                        currentGameClockTime += JUMPSIZE;
                    }
                    else{
                        //Stop all motor movement, were where we shouldnt be. I dont think this will happen, but good to prepare.
                    }
                    //Then, we have to send messages to all of the WiFi modules that need to know what time it is in-game

                    //END: Reset the lastTime so the if block doesnt just keep getting hit every cycle
                    lastTime = millis();
                }
            break;
            //Reverse
            case 2:
                //Turn motor on backwards
            break;
            //Fast Forward
            case 3:
                //Turn motor on forwards
            break;
            default:
                //Stop motor movement again? Idk what the default should be.
            break;
        }
    }
}
